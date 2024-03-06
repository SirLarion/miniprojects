import random
import requests
import re
from bs4 import BeautifulSoup

url = 'https://www.menneske.no/sudoku/eng/utskrift.html?number='
MAX_NUM = 675000

def scrape_puzzle(difficulty='hard'):
  puzzle = ''
  found = False
  while not found:
    num = random.randint(0, MAX_NUM)
    res = requests.get(f'{url}{num}')
    if res.status_code != 200:
      print(f'Accessing {url} failed')
    else:
      dom = BeautifulSoup(res.content, 'html.parser')
      grid = dom.select('.grid', string=re.compile(r'^Difficulty'))
      tds = dom.select('td')
      if grid[0].text.lower().find(difficulty) != -1:
        for td in tds:
          c = td.text
          if not c.isalnum():
            c = '0'
          
          puzzle += c
        
        found = True
  
  return puzzle