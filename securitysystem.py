import requests
import json
import pandas as pd 
from datetime import datetime
import time

token = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MjE4In0.veykt_ZGM1KuSpmJCEmx7tNkC93vnu97P-7bIueuK84' #Minas token

outputKey = '21617'
inputKey = '9656'
pincol = ['pin', 'phone', 'access']
phonecol = ['valid_till', 'phone']

# Sender signal om at pin-koden ikke er i databasen
def deny():
    data_deny = data_dictionary(outputKey, token, "0")
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(data_deny),headers={'Content-Type':'application/json'})
    data_bye = data_dictionary(inputKey, token, "0")
    requests.put('https://circusofthings.com/WriteValue',
		data=json.dumps(data_bye),headers={'Content-Type':'application/json'})
    print("Rejected")
    
# Sender signal om at pinkoden ligger i databasen, eller skriver error dersom kontakt med CoT ikke lykkes
def accept():
    data_confirm = data_dictionary(outputKey, token, "1")
    data_finito = data_dictionary(inputKey, token, "0")
    data_error = data_dictionary(outputKey, token, "3")
    
    response1 = requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_confirm),headers={'Content-Type':'application/json'})
    requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_finito),headers={'Content-Type':'application/json'})
    if(response1.status_code==200):
       print("Welcome")
       
    else:
       print("error %d" % (response1.status_code))
       requests.put('https://circusofthings.com/WriteValue',
				data=json.dumps(data_error),headers={'Content-Type':'application/json'})
       
# Funksjon for å legge til nye entries i pinkodedatabasen. Kjøres i konsollen
def add_pin(new_pin, access_type, phonenum, num):
    df = check_db(1)
    if df.empty:  # Hvis databasen er tom lages det tom liste for å unngå error
        listing1 = []
        listing1.append(new_pin) # Legger ny verdi i liste
        todb_pin(listing1, access_type, phonenum, num)
    else:
        # Lager en liste for hver kolonne i databasen
        pinlist = make_pin_list(df, new_pin)
        accesslist = df['access'].tolist()
        phonelist = df['phone'].tolist()
        
        # Legger de nye verdiene til i korresponderende liste
        pinlist.append(new_pin)
        accesslist.append(access_type)
        phonelist.append(phonenum)
        todb_pin(pinlist, accesslist, phonelist, num)
        
# Funksjon for å legge til nye telefonnumre i databasen. Kjøres bare i konsollen,
# da brukergrensesnittet for funksjonaliteten ligger på admin-enheten (admincontrol.py)
def add_phone(new_phonenum, num):
    df = check_db(2)
    today = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
    if df.empty: # Hvis databasen er tom lages det tomme lister for å unngå error
        phonelist = []
        validlist = []
        
        # Legger til nye verdier i korrensponderende liste
        phonelist.append(new_phonenum)
        validlist.append(today)
        todb_phone(validlist, phonelist, today, num)
    else:
        # Lager lister av kolonnene i databasen, inkludert indekskolonnen
        phonelist = make_phonepinlist(df)
        validlist = df.index
        validlist = validlist.tolist()
        
        # Legger til nye verdier i korrensponderende liste
        phonelist.append(new_phonenum)
        validlist.append(today)
        todb_phone(validlist, phonelist, today, num)
        
# Legger ny pin i database
def todb_pin(pinlist, accesslist, phonelist, num):
    # Lager df med pinkoder som indeks og kolonnene phone og access
    df1 = pd.DataFrame(index = pinlist, columns = ['phone', 'access'])
    # Setter kolonnene phone og access til de oppdaterte listene
    df1['phone'] = phonelist
    df1['access'] = accesslist
    df1.index.names = ['pin']
    # Lagrer den nye df'en som gjeldende database
    df1.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num))
    print('PIN added')
  
# Legger nytt tlfnummer i databasen
def todb_phone(validlist, phonepinlist, today, num):
    # Lager df med datetime index (med "utløpsdato") og kolonnen phone
    df1 = pd.DataFrame(index = validlist, columns = ['phone'])
    # Setter kolonnen phone til den oppdaterte listen med gyldige telefonnumre
    df1['phone'] = phonepinlist
    df1.index.names = ['valid_till']
    # Lagrer den nye df'en som gjeldende database
    df1.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num))
    print('Phonenumber added. Valid until: ', today)


# Lager en dictionary med data som brukes for å skrive verdier til CoT
def data_dictionary(key, token, value):
    data_d = {"Key" : "0", "Value" : "0", "Token" : "0"}
    data_d["Key"] = key
    data_d["Token"] = token
    data_d["Value"] = value
    return data_d

# Henter databasen fra csv-filen og returnerer den i dataframe-format
def check_db(num):
    df = pd.read_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num), index_col = [0])
    return df

# Gjør database-kolonnen med alle pinkodene til listeformat om ikke pinkoden allerede finnes i databasen
def make_pin_list(df, new_pin):
    listing = df.index
    if (new_pin in listing):
        print('Pin already exists')
    else:
        listing1 = df.index.tolist()
        return listing1
    
# Leser av CoT ved bestemt key og returnerer verdien som står der
def hent(key, token):
    response = requests.get('https://circusofthings.com/ReadValue', params = {'Key':key, 'Token':token})
    response = json.loads(response.content)
    value = response.get('Value')
    return int(value)
        
# Kjøres en gang i konsollen for å opprette database
def one_time(num, col):
    df = pd.DataFrame(columns = col)
    df.to_csv(r'/home/pi/hybelsystem/securitysystem/entrypindb{}.csv'.format(num), index = False)

# Lager en liste over telefonnumrene i databasen, hvis databasen er tom opprettes en tom liste
def make_phonepinlist(db):
    if db.empty:
        phonepinlist = []
        return phonepinlist
    else:
        phonepinlist = db['phone']
        phonepinlist = phonepinlist.tolist()
        return phonepinlist

# Sjekker om det har blitt tastet inn en pinkode
def wassup():
    checkVar = hent(inputKey, token)
    return checkVar

while True:
    hey = wassup() # Sjekker om det har kommet inn nye verdier
    if hey != 0: # Setter i gang resten av programmet om det kommer inn en ny verdi
        pin = hey
        pinlength = len(str(pin)) # Finner lengden til signalet
        if (pinlength == 8): # Lengde 8 fortolkes som tlfnummer (gjestepin)
            db = check_db(2) # Henter databasen for gjestepinkoder/telefonnumre
            phonepinlist = make_phonepinlist(db) # Lager en liste over gyldige telefonnumre
            if (pin in phonepinlist): # Aksepterer dersom telefonnummeret finnes i listen fra databasen
                accept()
            else:
                deny()
        elif (pinlength == 4): # Lengde 4 fortolkes som pinkode (beboerpin)
            db = check_db(1) # Henter databasen for beboerpinkoder
            pinlist = db.index.tolist() # Lager en liste over gyldige pinkoder
            if (pin in pinlist): # Aksepterer dersom pinkoden finnes i listen fra databasen
                accept()
            else:
                deny()
    else:
        time.sleep(5) # Vent 5 sekunder før signalet sjekkes på nytt
