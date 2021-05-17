# -*- coding: utf-8 -*-
"""
Created on Thu May  6 18:49:46 2021

@author: Karoline
"""

import pandas as pd
from datetime import datetime

# Henter databasen for tlfnumre (gjestepinkoder)
df = pd.read_csv(r"/home/pi/hybelsystem/securitysystem/entrypindb2.csv", index_col = [0])

# Finner nåværende tidspunkt
today = datetime.today().strftime('%Y-%m-%d %H:%M:%S')

# Sletter rader i databasen som har utgått "utløpsdato"
df.drop(df.loc[df.index < today].index, inplace=True)

# Lagrer databasen
df.to_csv(r"/home/pi/hybelsystem/securitysystem/entrypindb2.csv")