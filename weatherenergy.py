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

columns_temp = ['time', 'produced', 'spot_price']

# Lager database, kjøres kun en gang gjennom konsollen
def create_db(col):
    df = pd.DataFrame(columns = col)
    df.to_csv(r'/home/pi/hybelsystem/weatherenergy/weatherenergy.csv')

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
    url_w = base_url + 'q=' + city + '&appid=' + api_key
    response = requests.get(url_w)
    if response.status_code == 200:
        data = response.json()
        report = data['weather']
        report = report[0]['description']
        return report
    else:
       print("Error in the HTTP request")
       nope = 'error'
       return nope

# Laster ned og leser kildekoden til SkandiaEnergi sin nettside
def read_page(url, req):
    webpage = urllib.urlopen(req).read()
    soup = BeautifulSoup(webpage, "lxml")
    return soup

# Navigerer HTML-filen for å finne den delen av nettsiden som
# har spotprisen for midt-Norge.
def find_value(soup):
    dagens = soup.find("div", {'id':'no3'})
    content = str(dagens)
    start = content.find('<p>') + 3
    end = content.find('</p>', start)
    price = content[start:end]
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
    verdi = df['Produksjon kW pr solskinnstid'].values[0]
    thisHourProd = verdi * co
    return thisHourProd

# Klargjør data i listeformat for å legge det inn i databasen
def to_dbformat(kw, price):
    hour_price = float(price)
    hour_kw = float(kw)
    now = datetime.now()
    hour_time = now.strftime("%d/%m/%Y %H:%M:%S")
    listing = [hour_time, hour_kw, hour_price]
    return listing

# Tar inn en liste med verdier og legger dem inn i en ny rad i databasen
def to_db(listing):
    df = get_db('weatherenergy')
    df_length = len(df)
    df.loc[df_length] = listing
    df.to_csv(r'/home/pi/hybelsystem/weatherenergy/weatherenergy.csv')

def send_hourly_data(kw, price):
    kw_data = data_dictionary(prodKey, token, kw)
    price_data = data_dictionary(spotKey, token, price)
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(kw_data),headers={'Content-Type':'application/json'})
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(price_data),headers={'Content-Type':'application/json'})

time.sleep(60)
while True:
    weather_data = get_data()
    coefficient = sunOrNah(weather_data)
    kW = kW_thishour(coefficient, 'solcelle')
    soup = read_page(url_p, req)
    spotpris = find_value(soup)
    listing = to_dbformat(kW, spotpris)
    to_db(listing)
    send_hourly_data(kW, spotpris)
    time.sleep(3600)