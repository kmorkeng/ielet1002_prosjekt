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

#token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTU5In0.gf8JypyMMsWYBRo5GM58ORIeEk66AAVNzLRcD1RCGJA"
token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MTI2In0.DZ9fF-FV8PeyFZJ8aJJ7hjOIENv6L1z471tRgcHCPyo"
#tlfKey = "25972"
inputKey = '1794'
outputKey = '436'

def wassup():
    guestVar = hent(inputKey, token)
    return guestVar

# Henter verdien lagret på bestemt key
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

def check_db2(databasenum):
    df = pd.read_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(databasenum))
    return df

# Funksjon for å legge til nye entries i databasen. Kjøres i konsollen
def add_pin(new_pin, access_type, phonenum, num):
    df = check_db('securitysystem', 'entrypindb1')
    if df.empty:
        listing1 = []
        listing1.append(new_pin)
        todb(listing1, access_type, phonenum, num)
    else:
        pinlist = make_num_list(df, new_pin)
        accesslist = df['access'].tolist()
        phonelist = df['phone'].tolist()
        accesslist.append(access_type)
        phonelist.append(phonenum)
        todb(pinlist, accesslist, phonelist, num)
        
# Funksjon for å legge til nye entries i databasen. Kjøres i konsollen
def add_phone(new_phonenum, num):
    df = check_db('securitysystem', 'entrypindb2')
    today = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
    today = pd.to_datetime(today)
    validtill = today + dt.timedelta(hours = 24)
    if df.empty:
        phonelist = []
        phonelist.append(new_phonenum)
        validlist = []
        validlist.append(validtill)
        todb_phone(validlist, phonelist, today, num)
    else:
        phonelist = make_phonepinlist(df)
        phonelist.append(new_phonenum)
        validlist = df.index
        validlist = validlist.tolist()
        validlist.append(validtill)
        todb_phone(validlist, phonelist, validtill, num)
        
    
def todb_phone(validlist, phonepinlist, validtill, num, new_phonenum):
    df1 = pd.DataFrame(index = validlist, columns = ['phone'])
    df1['phone'] = phonepinlist
    df1.index.names = ['valid_till']
    df1.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num))
    print('Phonenumber ', new_phonenum, ' added. Valid until: ', validtill)
    
def make_phonepinlist(db):
    if db.empty:
        phonepinlist = []
        return phonepinlist
    else:
        phonepinlist = db['phone']
        phonepinlist = phonepinlist.tolist()
        return phonepinlist
        
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
    
def finished_process():
    response_data = data_dictionary(inputKey, token, '0')
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(response_data),headers={'Content-Type':'application/json'})
    
def simple_decode(doubledigit):
    doubledigit = str(doubledigit)
    databasenum = doubledigit[0]
    user = doubledigit[1]
    return databasenum, user

def remove_user(db, databasenum, user):
    start = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
    start = pd.to_datetime(start)
    end = start + dt.timedelta(minutes = 60)
    start1 = str(start)
    end1 = str(end)
    df = db
    if df.empty:
        print('Nothing to remove')
    else:
        df['user'] = df['user'].astype(str)
        df1 = df.loc[start1:end1]
        df2 = df1[df1["user"].str.contains("{}".format(user), na=False)]
        if df2.empty:
            print('Nothing to remove')
        else:
            dfn = df.loc[start1:end1, 'user'].str.replace('{}'.format(user),'')
            df.loc[start1:end1, 'user'] = dfn
            df.to_csv(r"/home/pi/hybelsystem/booking/romdb{}.csv".format(databasenum))
            
def send_people(people):
    response_data = data_dictionary(outputKey, token, "{}".format(people))
    requests.put('https://circusofthings.com/WriteValue',
	         data=json.dumps(response_data),headers={'Content-Type':'application/json'})

while True:
    hey = wassup()
    if hey != 0:
        num_len = len(str(hey))
        if num_len == 8:
            tlfnr = hey
            add_phone(tlfnr, 2)
            finished_process()

        elif num_len == 2:
            room_user = hey
            databasenum, user = simple_decode(room_user)
            db = check_db1(databasenum)
            remove_user(db, databasenum, user)
            finished_process()

        elif num_len == 1:
            databasenum = hey
            db = check_db1(databasenum)
            db.index = pd.to_datetime(db.index)
            now = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
            #now = datetime.today().strftime('%Y-%m-%d %H:%M')
            #seconds = ":00"
            #now_str = str(now)
            #now_str = now_str + seconds
            #now = datetime.strptime(now_str, '%Y-%m-%d %H:%M:%S')
            #df1 = db.iloc[db.index.get_loc(now, method = 'default')]
            df1 = db.iloc[db.index.get_loc(now, method = 'nearest')]
            people = df1.loc['user']
            send_people(people)
            finished_process()
    else:
        time.sleep(5)