import requests
GetValue_v1URL = "https://blynk.cloud/external/api/get?token=nSgd5nnVgfpmcTq2zCp5MT3FxwnaLfaH&v1"
SetValue_v1URL = "https://blynk.cloud/external/api/update?token=nSgd5nnVgfpmcTq2zCp5MT3FxwnaLfaH&v1=1"
V1_Value = requests.get(GetValue_v1URL)

while True:
    V1_Value = requests.get((GetValue_v1URL))

    if(int(V1_Value.text) == 1):
        print("V1 is ON")
    else:
        print("V1 is OFF")
