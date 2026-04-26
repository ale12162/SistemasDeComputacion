import requests
import ctypes
import os

LIB_PATH = os.path.join(os.path.dirname(__file__), "libgini.so")
lib = ctypes.CDLL(LIB_PATH)

lib.float_to_int.argtypes = [ctypes.c_float]
lib.float_to_int.restype  = ctypes.c_int

lib.gini_index.argtypes = [ctypes.c_float]
lib.gini_index.restype  = ctypes.c_int

API_URL = (
    "https://api.worldbank.org/v2/country/ARG/indicator/SI.POV.GINI"
    "?format=json&date=2000:2023&per_page=100"
)

def fetch_gini():
    response = requests.get(API_URL, timeout=10)
    response.raise_for_status()
    data = response.json()
    records = data[1]
    results = [
        (r["date"], float(r["value"]))
        for r in records
        if r["value"] is not None
    ]
    results.sort(key=lambda x: x[0])
    return results

def main():
    print("Obteniendo datos de GINI para Argentina...")
    records = fetch_gini()

    if not records:
        print("No se encontraron datos.")
        return

    print(f"\n{'Año':<8} {'GINI (float)':<18} {'GINI (entero)':<18} {'GINI (+1)'}")
    print("-" * 58)
    for year, value in records:
        gini_int = lib.float_to_int(ctypes.c_float(value))
        gini_p1  = lib.gini_index(ctypes.c_float(value))
        print(f"{year:<8} {value:<18.2f} {gini_int:<18} {gini_p1}")

if __name__ == "__main__":
    main()
