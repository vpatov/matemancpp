

import pprint
import requests
from bs4 import BeautifulSoup
import asyncio
from contextlib import closing
import aiohttp
import os


hostname = 'https://www.pgnmentor.com/'
files_endpoint = 'files.html'



r = requests.get(hostname + files_endpoint)
if (r.status_code != 200):
  print(r.status_code)
  exit()

soup = BeautifulSoup(r.content,'html.parser')
all_links = set([(hostname + '/' + link.get('href')) for link in soup.find_all('a') if '.zip' in (link.get('href') or ())]) 
print(len(all_links))
# https://www.pgnmentor.com/openings/OwenDefense.zip

def url_to_filename(url: str):
  return 'database/' + url.replace(hostname,'').replace('/','-')


async def download_file(session: aiohttp.ClientSession, link: str):
  filename = url_to_filename(link)
  try:
    stat = os.stat(filename)
    if (stat.st_size > 0):
      print("Already downloaded {}".format(filename))
      return
  except:
    pass
    
  async with session.get(link) as response:
    assert response.status == 200
    with open(filename,'wb') as f:
      contents = await response.read()
      f.write(contents)
      print("Wrote {}".format(filename))

async def download_all(session):
  for link in all_links:
    await download_file(session,link)


async def main(loop):
  connector = aiohttp.TCPConnector(limit=3)
  async with aiohttp.ClientSession(loop=loop, connector=connector) as session:
    await download_all(session)

loop = asyncio.get_event_loop()
loop.run_until_complete(main(loop))
print("Complete.")


