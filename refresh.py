# -*- coding: utf-8 -*-
"""
Created on Thu May  6 18:49:46 2021

@author: Karoline
"""

import pandas as pd
from datetime import datetime

df = pd.read_csv(r"/home/pi/hybelsystem/securitysystem/entrypindb2.csv", index_col = [0])
today = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
df.drop(df.loc[df.index < today].index, inplace=True)
df.to_csv(r"/home/pi/hybelsystem/securitysystem/entrypindb2.csv")