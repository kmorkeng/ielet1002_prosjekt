import pandas as pd
import requests
import json
import time
from datetime import datetime
from bs4 import BeautifulSoup
import urllib.request as urllib

# Variabler for uthenting av værdata med Current weather API fra OpenWeather
base_url = r"https://api.openweathermap.org/data/2.5/weather?"
city = "Trondheim"
api_key = "355eef546396aba57dbcd138d977b12c"

# Variabler for uthenting av data fra SkandiaEnergi sin nettside
url_p = r"https://www.skandiaenergi.no/dagens-strompriser/"
req = urllib.Request(url_p, headers={'User-Agent': 'Mozilla/5.0'})

token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTU5In0.gf8JypyMMsWYBRo5GM58ORIeEk66AAVNzLRcD1RCGJA"
prodKey = '12439'
spotKey = '9545'

# Databasekolonner
columns_temp = ['time', 'produced', 'spot_price']

# Lager database, kjøres kun en gang gjennom konsollen
def create_db(col):
    df = pd.DataFrame(columns = col)
    df.to_csv(r'/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv')

# Leser csv-filer til dataframe-format
def get_db(filename):
    df = pd.read_csv(r'/home/pi/hybelsystem/weatherenergy/{}.csv'.format(filename), encoding="latin1", index_col = [0])
    return df

# Lager en dictionary for konvertering av data til json
def data_dictionary(key, token, value):
    data_d = {"Key" : "0", "Value" : "0", "Token" : "0"}
    data_d["Key"] = key
    data_d["Token"] = token
    data_d["Value"] = value
    return data_d

# Henter værdata ved å benytte seg av en openweathermap.org sin API
def get_data():
    # Henter data gjennom Current weather API
    url_w = base_url + 'q=' + city + '&appid=' + api_key
    response = requests.get(url_w)
    # Hvis datainnhentingen er vellykket
    if response.status_code == 200:
        data = response.json()
        # Henter ut det ønskede datapunktet
        report = data['weather']
        report = report[0]['description']
        return report
    # Hvis det forekommer error
    else:
       print("Error in the HTTP request")
       nope = 'error'
       return nope

# Laster ned og leser kildekoden til SkandiaEnergi sin nettside
def read_page(url, req):
    webpage = urllib.urlopen(req).read() # Henter og leser nettsiden
    soup = BeautifulSoup(webpage, "lxml") # Gjør formatet til den
    # nedlastede nettsiden kompatibelt med BeautifulSoup-operasjoner
    return soup

# Navigerer HTML-filen for å finne den delen av nettsiden som
# har spotprisen for midt-Norge.
def find_value(soup):
    # Lokaliserer delen av HTML-koden med riktig id
    dagens = soup.find("div", {'id':'no3'})
    # content vil bli seende slik ut: <h4>Midt</h4><p>price</p>[...] hvor price tilsvarer spotprisen
    content = str(dagens)
    # Finner indeksen til stringen som tilsvarer starten på datapunktet som skal hentes ut
    start = content.find('<p>') + 3
    # Finner sluttindeksen for datapunktet
    end = content.find('</p>', start)
    # Henter ut delen av stringen mellom start og end
    price = content[start:end]
    # Bytter ut komma med punktumn for å få riktig format
    price = price.replace(',', '.')
    return price

# Lager en koeffisient for strømproduksjon gjennom værstatusen
def sunOrNah(data):
    if data == 'clear sky':
        return 1
    elif data == 'few clouds':
        return 0.95
    elif data == 'scattered clouds':
        return 0.90
    elif data == 'broken clouds':
        return 0.85
    else:
        return 0.80

# Henter inn hvor mye strøm som produseres i løpet av en solskinnstime, og ganger det med koeffisienten beregnet fra værstatus
def kW_thishour(co, filename):
    df = get_db(filename)
    verdi = df['Produksjon kW pr solskinnstid'].values[0] # Henter datapunktet med energiproduksjon per solskinnstime fra solcelle.csv
    thisHourProd = verdi * co
    return thisHourProd

# Klargjør data i listeformat for å legge det inn i databasen
def to_dbformat(kw, price):
    hour_price = float(price)
    hour_kw = float(kw)
    now = datetime.now()
    hour_time = now.strftime("%d-%m-%Y %H:%M:%S")
    return hour_time, hour_kw, hour_price

# Tar inn en liste med verdier og legger dem inn i en ny rad i databasen
def to_db(hour_time, hour_kw, hour_price):
    df = get_db('weatherenergydb')
    if df.empty: # Hvis databasen er tom lages det tomme lister for å unngå error
        # Lager en tom liste for hver av kolonnene (ink. indekskolonnen)
        indexlist = []
        kwlist = []
        pricelist = []
        
        # Legger de nye verdiene til i korresponderende liste
        indexlist.append(hour_time)
        kwlist.append(hour_kw)
        pricelist.append(hour_price)
        
        # Lager en ny df ut ifra listene og lagrer det som gjeldende database
        df1 = pd.DataFrame(index = indexlist, columns = ['produced', 'spot_price'])
        df1['produced'] = kwlist
        df1['spot_price'] = pricelist
        df1.to_csv(r'/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv')
    else:
        indexlist = df.index.tolist() # Lager en liste over tidspunktene/indeksverdiene
        kwlist = df.loc[:, 'produced'].tolist() # Lager en liste av produced-kolonnen
        pricelist = df.loc[:, 'spot_price'].tolist() # Lager en liste av spot_price-kolonnen
        
        # Legger de nye verdiene til i korresponderende liste
        indexlist.append(hour_time)
        kwlist.append(hour_kw)
        pricelist.append(hour_price)
        
        # Lager en ny df ut ifra listene og lagrer det som gjeldende database
        df1 = pd.DataFrame(index = indexlist, columns = ['produced', 'spot_price'])
        df1['produced'] = kwlist
        df1['spot_price'] = pricelist
        df1.to_csv(r'/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv')

# Sender timens energiproduksjon og spotpris til CoT så det er tilgjengelig for admin-enheten å hente ut
def send_hourly_data(kw, price):
    kw_data = data_dictionary(prodKey, token, kw)
    price_data = data_dictionary(spotKey, token, price)
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(kw_data),headers={'Content-Type':'application/json'})
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(price_data),headers={'Content-Type':'application/json'})

# main loop
while True:
    weather_data = get_data() # Henter værdata
    coefficient = sunOrNah(weather_data) # Finner koeffisient basert på værdata
    kW = kW_thishour(coefficient, 'solcelle') # Finner produsert energi denne timen ut ifra koeffisient
    soup = read_page(url_p, req) # Henter og leser av HTML-kilden til SkandiaEnergi
    spotpris = find_value(soup) # Navigerer HTML-koden for å finne spotprisen
    hour_time, hour_kw, hour_price = to_dbformat(kW, spotpris) # Gjør verdiene til riktig format
    to_db(hour_time, hour_kw, hour_price) # Setter nye verdier inn i databasen
    send_hourly_data(kW, spotpris) # Sender data om energiproduksjon og spotpris til CoT
    time.sleep(3600) # Pause i en time