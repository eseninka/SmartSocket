from bs4 import BeautifulSoup
import requests

url = 'https://www.lesk.ru/private_clients/rates/'
response = requests.get(url, verify=False)
print(response.status_code)

soup = BeautifulSoup(response.text, 'lxml')

main = soup.find_all('p', {'align': 'center'})
print(main)

