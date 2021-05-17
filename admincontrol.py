# -*- coding: utf-8 -*-
"""
Created on Wed May  5 12:05:33 2021

@author: Karoline
"""
import requests
import json
import pandas as pd
from datetime import datetime
import datetime as dt
import time

# Token og keys for CoT
token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MTI2In0.DZ9fF-FV8PeyFZJ8aJJ7hjOIENv6L1z471tRgcHCPyo"
inputKey = '1794'
outputKey = '436'

# Henter inn verdien til inputsignalet
def wassup():
    inputVar = hent(inputKey, token)
    return inputVar

# Henter verdien lagret ved signalet til key
def hent(key, token):
    response = requests.get('https://circusofthings.com/ReadValue', params = {'Key':key, 'Token':token})
    response = json.loads(response.content)
    value = response.get('Value')
    return int(value)

# Lager en dictionary for konvertering av data til json
def data_dictionary(key, token, value):
    data_d = {"Key" : "0", "Value" : "0", "Token" : "0"}
    data_d["Key"] = key
    data_d["Token"] = token
    data_d["Value"] = value
    return data_d

# Henter databasen i en dataframe
def check_db(folder, filename):
    df = pd.read_csv(r"/home/pi/hybelsystem/{}/{}.csv".format(folder, filename), index_col = [0])
    return df

# Henter databasen ut som dataframe
def check_db1(databasenum):
    df = pd.read_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum), index_col = [0])
    #df['user'] = df['user'].astype(int)
    return df

# Funksjon for å legge til nye entries for beboerpinkoder i databasen. Kjøres i konsollen
def add_pin(new_pin, access_type, phonenum, num):
    db = check_db('securitysystem', 'entrypindb1')
    if db.empty: # Lager tom liste dersom databasen er tom
        listing1 = []
        listing1.append(new_pin) # Legger til ny pin i lista
        todb(listing1, access_type, phonenum, num)
    else:
        # Lager en liste for hver av kolonnene i databasen, inkludert indekskolonnen
        pinlist = make_num_list(db, new_pin)
        accesslist = db['access'].tolist()
        phonelist = db['phone'].tolist()
        
        # Legger til de nye verdiene i listene
        accesslist.append(access_type)
        phonelist.append(phonenum)
        todb(pinlist, accesslist, phonelist, num)
        
# Funksjon for å legge til nye telefonnumre i databasen
def add_phone(new_phonenum, num):
    db = check_db('securitysystem', 'entrypindb2') # Henter databasen for gjestepinkoder/telefonnumre
    today = datetime.today().strftime('%Y-%m-%d %H:%M:%S') # Finner nåværende tidspunkt
    today = pd.to_datetime(today)
    validtill = today + dt.timedelta(hours = 24) # Tidsstempel 24 timer fra nåværende tidspunkt
    if db.empty: # Hvis databasen er tom
        # Lager tomme lister tilsvarende kolonnene i databasen
        phonelist = []
        validlist = []
        
        # Legger de nye verdiene til i listene
        phonelist.append(new_phonenum)
        validlist.append(validtill)
        todb_phone(validlist, phonelist, validtill, num, new_phonenum)
    else:
        # Lager en liste for hver av kolonnene i databasen, inkludert indekskolonnen
        phonelist = make_phonepinlist(db)
        validlist = db.index
        validlist = validlist.tolist()
        
        # Legger til de nye verdiene i listene
        phonelist.append(new_phonenum)
        validlist.append(validtill)
        todb_phone(validlist, phonelist, validtill, num, new_phonenum)
        
# Oppdaterer databasen for gjestepinkoder/telefonnumre  
def todb_phone(validlist, phonepinlist, validtill, num, new_phonenum):
    # Lager ny df med indeks like validlist, med kolonnen phone
    df1 = pd.DataFrame(index = validlist, columns = ['phone'])
    df1['phone'] = phonepinlist # Fyller verdier inn i phone-kolonnen
    df1.index.names = ['valid_till']
    df1.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num))
    print('Phonenumber ', new_phonenum, ' added. Valid until: ', validtill)

# Lager en liste over alle lagrede telefonnumre i databasen
def make_phonepinlist(db):
    if db.empty: # Lager en tom liste om databasen er tom
        phonepinlist = []
        return phonepinlist
    else: # Lager en liste som tilsvarer phone-kolonnen i databasen
        phonepinlist = db['phone']
        phonepinlist = phonepinlist.tolist()
        return phonepinlist
   
# Oppdaterer databasen for beboerpinkoder    
def todb(pinlist, accesslist, phonelist, num):
    df1 = pd.DataFrame(index = pinlist, columns = ['phone', 'access'])
    df1['phone'] = phonelist
    df1['access'] = accesslist
    df1.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num))
    print('PIN added')
    
# Gjør database-kolonnen med alle pinkodene til listeformat
def make_num_list(df, new_num):
    listing = df.index
    if (new_num in listing):
        listing1 = df.index.tolist()
        return listing1
    else:
        listing1 = df.index.tolist()
        listing1.append(new_num)
        return listing1
    
# Konkluderer forespørselen ved å sette input-signalet tilbake til 0
def finished_process():
    response_data = data_dictionary(inputKey, token, '0')
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(response_data),headers={'Content-Type':'application/json'})
 
# Separerer verdiene for rom og bruker. Ved doubledigit = 36 vil rom = 3 og bruker = 6
def simple_decode(doubledigit):
    doubledigit = str(doubledigit)
    databasenum = doubledigit[0]
    user = doubledigit[1]
    return databasenum, user

# Funksjon for fjerning av bruker i database
def remove_user(db, databasenum, user):
    start = datetime.today().strftime('%Y-%m-%d %H:%M:%S') # Tidspunkt nå
    start = pd.to_datetime(start)
    end = start + dt.timedelta(minutes = 60) # Tidspunkt om en time
    start1 = str(start)
    end1 = str(end)
    df = db
    if df.empty:
        print('Nothing to remove')
    else:
        df['user'] = df['user'].astype(str)
        df1 = df.loc[start1:end1] # df for tidsperioden mellom nå og om en time
        df2 = df1[df1["user"].str.contains("{}".format(user), na=False)] # Sjekk om brukeren er booket
        if df2.empty: # Ikke booket i tidsrommet
            print('Nothing to remove')
        else: # Booket i tidsrommet, fjerner brukeren fra de radene
            dfn = df.loc[start1:end1, 'user'].str.replace('{}'.format(user),'')
            df.loc[start1:end1, 'user'] = dfn
            df.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum))

# Sender antall brukere registrert i rommet til CoT            
def send_people(people):
    response_data = data_dictionary(outputKey, token, "{}".format(people))
    requests.put('https://circusofthings.com/WriteValue',
	         data=json.dumps(response_data),headers={'Content-Type':'application/json'})

while True:
    hey = wassup()
    if hey != 0: # Starter loop hvis det er et nytt signal i CoT
        num_len = len(str(hey)) # Sjekker lengden til sugnalverdien
        if num_len == 8: # Hvis lengden er 8 fortolkes det som telefonnummer som skal legges til
            tlfnr = hey
            add_phone(tlfnr, 2)
            finished_process()

        elif num_len == 2: # Hvis lengden er to tolkes det som at en bruker skal "avbookes" et rom
            room_user = hey
            databasenum, user = simple_decode(room_user) # Setter variebel for romnummeret til første
            # siffer og variabel for bruker til andre siffer
            db = check_db1(databasenum)
            remove_user(db, databasenum, user)
            finished_process()

        elif num_len == 1: # Hvis lengden er en tolkes det som et romnummer
            databasenum = hey
            db = check_db1(databasenum)
            db.index = pd.to_datetime(db.index)
            now = datetime.today().strftime('%Y-%m-%d %H:%M:%S') # Finner nåværende tidspunkt
            df1 = db.iloc[db.index.get_loc(now, method='nearest')] # Finner raden som er nærmest nåværende tidspunkt
            people = df1.loc['user'] # Henter ut verdien i user-kolonnen
            send_people(people)
            finished_process()
    else:
        time.sleep(5) # Vent 5 sekunder før signalet sjekkes på nytt