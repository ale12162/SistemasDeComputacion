import requests
import ctypes
import os

lib = ctypes.CDLL('./libgini.so') #Se carga la shared library
lib.convertir_gini.argtypes = [ctypes.c_double]
lib.convertir_gini.restype  = ctypes.c_int

params = {
    'format': 'json',
    'date': '2011:2020',
    'per_page': 32500,
    'page': 1
}

url = 'https://api.worldbank.org/v2/en/country/all/indicator/SI.POV.GINI'

def obtener_gini_argentina():
    try:
        res = requests.get(url, params=params, timeout=10)
        res.raise_for_status() 
        lista_datos = res.json()[1]

        for registro in lista_datos:
            if registro['country']['id'] == 'AR' and registro['value'] is not None:
                return float(registro['value'])
        return None

    except requests.exceptions.RequestException:
        return None

#valor = obtener_gini_argentina()
#print(f"Índice GINI Argentina: {valor}")
#print(res)
#print(res.text)

if valor is not None:
    print(f"Valor GINI float: {valor}")
    resultado = lib.convertir_gini(valor)  # ctypes pasa el double por la ABI
    print(f"GINI convertido a entero: {resultado}")
else:
    print("No se pudo obtener el dato GINI")