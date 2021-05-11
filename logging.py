# -*- coding: utf-8 -*-
"""
Created on Sun May  9 16:08:42 2021

@author: Karoline
"""

import pandas as pd
from datetime import datetime

columns1 = ['avg_prod', 'avg_price']
columns2 = ['date', 'avg_prod', 'avg_price']

df = pd.read_csv(r"/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv", index_col = [0])
df_log = pd.read_csv(r"/home/pi/hybelsystem/logging/loggingdb.csv", index_col = [0])

# Finner verdier som skal inn i databasen logging.py
today = datetime.today().strftime('%Y-%m-%d')
mean_prod = df['produced'].mean()
mean_price = df['spot_price'].mean()

if df_log.empty:
    datelist = []
    datelist.append(today)
    prodlist = []
    prodlist.append(mean_prod)
    pricelist = []
    pricelist.append(mean_price)
    
    df1 = pd.DataFrame(index = datelist, columns = columns1)
    df1['avg_prod'] = prodlist
    df1['avg_price'] = pricelist
    df1.index.names = ['date']
    df1.to_csv(r"/home/pi/hybelsystem/logging/loggingdb.csv")
    df2 = pd.DataFrame(columns = columns2)
    df2.to_csv(r"/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv", index = False)
else:
    date = df_log['date']
    datelist = date.tolist()
    prod = df_log['avg_prod']
    prodlist = prod.tolist()
    price = df_log['price']
    pricelist = price.tolist()
    
    datelist.append(today)
    prodlist.append(mean_prod)
    pricelist.append(mean_price)    
    
    df1 = pd.DataFrame(index = datelist, columns = columns1)
    df1['avg_prod'] = prodlist
    df1['avg_price'] = pricelist
    df1.index.names = ['date']
    df1.to_csv(r"/home/pi/hybelsystem/logging/loggingdb.csv")
    
    df2 = pd.DataFrame(columns = columns2)
    df2.to_csv(r"/home/pi/hybelsystem/weatherenergy/weatherenergydb.csv", index = False)
