import requests

url = "https://cdn.freesound.org/previews/833/833127_12880153-lq.mp3"
response = requests.get(url)

with open("musica.mp3", "wb") as f:
    f.write(response.content)

print("Download concluído!")