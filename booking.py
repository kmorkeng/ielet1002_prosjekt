import requests
import json
import pandas as pd 
from datetime import datetime
import datetime as dt
import time

# Variabler for CoT-signaler
token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTU5In0.gf8JypyMMsWYBRo5GM58ORIeEk66AAVNzLRcD1RCGJA"
brukerKey = "25048"
romKey = "27564"
varighetKey = "23226"
minuttKey = "982"
timeKey = "30201"
bekreftKey = "16455"
uselessButtonKey = "31470"

# Kolonner for dataframe
columns1 = ['time', 'user']
columns2 = ['time', 'user2']

# Funksjon som sjekker om det har kommet inn signal fra CoT
def wassup():
    uselessVar = hent(uselessButtonKey, token)
    return uselessVar

# Henter verdien lagret på bestemt key
def hent(key, token):
    response = requests.get('https://circusofthings.com/ReadValue', params = {'Key':key, 'Token':token})
    response = json.loads(response.content)
    value = response.get('Value')
    return int(value)

# Kjøres én gang for å opprette database
def one_time(databasenum):
    df = pd.DataFrame(columns = columns1)
    df.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum), index = False)

# Lager en dictionary for konvertering av data til json
def data_dictionary(key, token, value):
    data_d = {"Key" : "0", "Value" : "0", "Token" : "0"}
    data_d["Key"] = key
    data_d["Token"] = token
    data_d["Value"] = value
    return data_d

# Bruker hent-funksjonen til å lage variabler med gjeldende verdier
def verdier(brukerKey, romKey, varighetKey, minuttKey, timeKey):
    bruker = hent(brukerKey, token)
    rom = hent(romKey, token)
    varighet = hent(varighetKey, token)
    startminutt = hent(minuttKey, token)
    starttime = hent(timeKey, token)
    return bruker, rom, varighet, startminutt, starttime


# Henter databasen ut som dataframe
def check_db(databasenum):
    df = pd.read_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum), index_col = [0])
    df['user'] = df['user'].astype(int)
    return df

# Sjekker maksgrensa for antall bookinger om gangen i et rom
def max_amount_col(databasenum):
    if databasenum == 1: # Hvis kjøkken
        limit = 2 # Maks 2 på rommet om gangen
        return limit
    elif databasenum == 2: # Hvis stue
        limit = 3 # Maks 3 på rommet om gangen
        return limit
    elif databasenum == 3: # Hvis bad
        limit = 1 # Maks 1 på rommet om gangen
        return limit
    
# Lager en datetimeindex og en liste over alle minuttene rommet er booket
def create_timelist(startdt, enddt):
    # Lager en datetimeindeks med minuttene mellom starttid og sluttid for bookingen
    indexcol = pd.date_range(start = startdt , end = enddt, freq='T')
    # Gjør datetimeindeksen til liste lagret ved egen variabel
    timelist = indexcol.tolist()
    return timelist, indexcol

# Setter sammen variablene for starttime og startminutt til datetime format med dagens dato
def startAndEnd(hour, minute, duration):
    hour = str(hour)
    minute = str(minute)
    separator = ":"
    seconds = "00"
    hourminute = hour + separator + minute + separator + seconds # Resultatet blir hour:minute:00
    today = datetime.now().date() # Finner dagens dato
    starttime1 = datetime.strptime(hourminute, "%H:%M:%S")
    start = datetime.combine(today, datetime.time(starttime1)) # Legger dato til tidspunktet
    end = start + dt.timedelta(minutes = duration) # Finner sluttid basert på starttiden + varigheten i minutter
    return start, end

# Lager en dataframe med bookingen som ble hentet fra CoT
def create_entrydf(user, startdt, enddt):
    timelist, indexcol = create_timelist(startdt, enddt) # Lager datetimeindeks og liste over minuttene i tidsrommet til bookingen
    entrydf = pd.DataFrame(columns = columns2) # Lager ny df med kolonne date og user2
    
    # Legger tidspunktene og brukeren inn i kolonnene time og user2
    entrydf["time"] = indexcol
    entrydf['user2'] = user
    
    # Setter time-kolonnen som indeks
    entrydf = entrydf.set_index('time')
    entrydf['user2'] = entrydf['user2'].astype(int)
    return entrydf, timelist

# Sjekker om tidsrommet kan bookes eller om det finnes fullbookede tidspunkter i tidsrommet
def statusCheck(dbdf, entrydf, databasenum, startdt, enddt):
    check = dbdf
    startdt = str(startdt)
    enddt = str(enddt)
    
    # Lager en df som inneholder alle radene i user-kolonnen mellom starttid og sluttid for bookingen
    check = check.loc[startdt:enddt, 'user']
    check.fillna(0, inplace=True)
    max_value = check.max() # Finner den høyeste verdien for user-kolonnen i tidsrommet
    max_value = len(str(max_value)) # Sjekker lengden til den høyeste verdien
    limit = max_amount_col(databasenum) # Finner maksgrensa for antall personer i rommet samtidig
    
    # Sjekker om bookingen vil føre til overskridelse av maksgrensa
    if max_value >= limit:
        return 0
    elif max_value < limit:
        return 1

# Lagrer dataframe til csv og sender respons til CoT
def accept(df, databasenum):
    df.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum))
    to_CoT(databasenum)

# Lager et tall som inneholder de brukerne som er booket i rommet, inkludert den nye bookingen
def to_one(x, y):
    result = (x*10) + y
    return result

# Sender respons til CoT om at bookingen ble avslått
def deny():
    data_deny = data_dictionary(bekreftKey, token, "0")
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(data_deny),headers={'Content-Type':'application/json'})
    data_deny = data_dictionary(uselessButtonKey, token, "0")
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(data_deny),headers={'Content-Type':'application/json'})
    print("Denied: Already booked")

# Sender respons til CoT om at bookingen ble godtatt
def to_CoT(databasenum):
    data_confirm = data_dictionary(bekreftKey, token, "1")
    data_finito = data_dictionary(uselessButtonKey, token, "0")
    data_error = data_dictionary(bekreftKey, token, "3")
    
    response1 = requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_confirm),headers={'Content-Type':'application/json'})
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_finito),headers={'Content-Type':'application/json'})
    if(response1.status_code==200):
       print("Successfully added to database for room {}".format(databasenum))
       
    else:
       print("error %d" % (response1.status_code))
       requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_error),headers={'Content-Type':'application/json'})
       

def accept_fastlane(dbdf, entrydf, databasenum, startdt, enddt):
    df = dbdf.join(entrydf, how = 'outer') # Joiner slik at databasen har kolonnene time(index), user, user2
    df.index = pd.to_datetime(df.index)
    df.index.names = ['time']
    
    # Setter tomme rader i user og user2 kolonnen til null
    df['user'] = df['user'].fillna(value = 0)
    df['user2'] = df['user2'].fillna(value = 0)
    index_1 = df.index
    df0 = df
    df0['user1'] = df[startdt:enddt].apply(f, axis = 1) # Legger til brukeren i user-kolonnen i radene mellom starttid og sluttid
    # Funksjonen som legger til brukeren vil ikke gjøre dette ved de radene som ikke har brukere i seg fra før
    df0.loc[df0['user1'].isnull(),'user1'] = df0['user'] # Legger til brukeren i de radene hvor det ikke er registrert andre brukere fra før
    df1 = df0.loc[:, ['user1']]
    
    # Lager ny df med datetimeindex med alle de bookede tidspunktene som skal være i databasen, og kolonnen user
    dfn = pd.DataFrame(index = index_1, columns = ['user'])
    dfn['user'] = df1 # Fyller inn user-kolonnen
    
    # Setter den nye df'en som gjeldende database
    dfn.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum))
    to_CoT(databasenum) # Sender signal om at bookingen er gjennomført til CoT
    
# Legger til en bruker på en rad ved å gange den eksisterende verdien i databasen med 10 og addere den nye brukeren
def f(x):
   y = x['user']
   z = x['user2']
   res = (10 * int(y)) + int(z)
   return int(res)
    
def find_value():
    bruker, rom, varighet, startminutt, starttime = verdier(brukerKey, romKey, varighetKey, minuttKey, timeKey)
    return bruker, rom, varighet, startminutt, starttime

# Omformer verdiene til ønsket format
def format_values(starttime, startminutt, varighet, bruker, rom):
    startdt, enddt = startAndEnd(starttime, startminutt, varighet) # Finner datetime for starttiden og sluttiden til bookingforespørselen
    entrydf, timelist = create_entrydf(bruker, startdt, enddt) # Lager dataframe av bookingforespørselen og en liste over tidspunktene som ønskes booket
    dbdf = check_db(rom) # Henter databasen
    return startdt, enddt, entrydf, dbdf, timelist

# Sammenligner to lister og returnerer en liste med alle felles listeelementene for de to listene
def intersection(lst1, lst2):
    lst3 = [value for value in lst1 if value in lst2]
    return lst3

# Returnerer antall overlappende rader mellom database og bookingforespørsel
def check_intersection(df1, timelist):
    if df1.empty: # Det vil forårsake en error om vi forsøker å lese lengden til en tom dataframe
        return 0.5
    else:
        dbseries = df1.index
        dblist = dbseries.tolist()
        dblist1 = list(map(str, dblist))
        timelist1 = list(map(str, timelist))
        common = intersection(timelist1, dblist1) # Lager en liste over alle indeksene som er til felles for entrydf og dbdf
        howoften = int(len(common)) # Finner lengden til listen og dermed hvor mange ganger entrydf og dbdf overlapper hverandre
        return howoften
    
# Sletter alle bookinger som er tidsstemplet før nåværende tidspunkt
def delete_old(df):
    if df.empty:
        return df
    else:
        df_new = df
        today = datetime.today().strftime('%Y-%m-%d %H:%M:%S') # Finner nåværende tidspunkt
        df_new.drop(df_new.loc[df_new.index < today].index, inplace=True) # Dropper rader med datetimeindeks før today
        return df_new
    
# I det tilfellet hvor databasen er tom, opprettes en ny df som bare inneholder entrydf
def first_db(entrydf, timelist, databasenum):
    index_1 = entrydf.index
    df = pd.DataFrame(index = index_1, columns = ['user'])
    df['user'] = entrydf['user2']
    # Setter den nye df'en som gjeldende database
    df.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum))
    to_CoT(databasenum)
    
while True:
    hey = wassup()
    if hey == 1: # Starter om uselessbutton er "aktivert"
        bruker, rom, varighet, startminutt, starttime = find_value() # Finner verdien tilsvarende variabelen
        startdt, enddt, entrydf, dbdf, timelist = format_values(starttime, startminutt, varighet, bruker, rom)
        delete_old(dbdf) # Sletter gammel informasjon i databasen
        what = check_intersection(dbdf, timelist) # Variabelen what vil tilsvare et tall for hvor mange rader av entrydf som også finnes i dbdf
        if what >= 1: # Hvis det er overlapp mellom bookingen og en eller flere rader i databasen
            status = statusCheck(dbdf, entrydf, rom, startdt, enddt) # Finner ut om tidsrommet kan bookes eller ikke
            if status == 0:
                deny()
            elif status == 1:
                accept_fastlane(dbdf, entrydf, rom, startdt, enddt)
        elif what == 0: # Hvis databasen var tom i det bookede tidsrommet, men ikke helt tom
            accept_fastlane(dbdf, entrydf, rom, startdt, enddt)
        elif what == 0.5: # Hvis databasen var tom
            first_db(entrydf, timelist, rom)
    else:
        time.sleep(5) # Vent 5 sekunder før signalet sjekkes på nytt