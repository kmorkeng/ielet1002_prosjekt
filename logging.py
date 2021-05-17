# -*- coding: utf-8 -*-
"""
Created on Sun May  9 16:08:42 2021

@author: Karoline
"""

import pandas as pd
from datetime import datetime

columns1 = ['produced', 'spot_price']
columns2 = ['avg_prod', 'avg_price']

# Henter databasen til weatherenergy og loggingdatabasen
df = pd.read_csv(r"/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv", encoding = 'latin1', index_col = [0])
df_log = pd.read_csv(r"/home/pi/hybelsystem/logging/loggingdb.csv", index_col = [0])

# Finner verdiene som skal loggføres
today = datetime.today().strftime('%Y-%m-%d')
mean_prod = df['produced'].mean()
mean_price = df['spot_price'].mean()

# Funksjoner som nullstiller databasen til weatherenergy, men beholder formatet
def reset_weatherenergydb(inputdf):
    # Lager ny df som tilsvarer en tom weatherenergydb.csv
    df2 = pd.DataFrame(columns = columns1)
    # Setter den "nullstilte databasen" til gjeldende database for weatherenergy
    df2.to_csv(r"/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv")
    
# Funksjon som skriver de nye verdiene til databasen
def todb(datelist, prodlist, pricelist):
    # Legger de nye verdiene til i listene
    datelist.append(today)
    prodlist.append(mean_prod)
    pricelist.append(mean_price) 
    
    # Lager ny df med datetime index og kolonnene avg_prod og avg_price
    df1 = pd.DataFrame(index = datelist, columns = columns2)
    df1['avg_prod'] = prodlist
    df1['avg_price'] = pricelist
    df1.index.names = ['date']
    # 
    df1.to_csv(r"/home/pi/hybelsystem/logging/loggingdb.csv")

if df_log.empty: # Oppretter tomme lister om databasen er tom
    datelist = []
    prodlist = []
    pricelist = []
    
    todb(datelist, prodlist, pricelist)
    reset_weatherenergydb(df)
else: # Lager lister av kolonnene til databasen
    date = df_log.index # Lager en liste over tidspunktene/indeksverdiene
    datelist = date.tolist()
    prod = df_log['avg_prod']
    prodlist = prod.tolist()
    price = df_log['avg_price']
    pricelist = price.tolist()   
    
    todb(datelist, prodlist, pricelist)
    reset_weatherenergydb(df)
