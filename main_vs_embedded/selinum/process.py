embedded = dict()
x=["hi","hello"]
website = 'embedded'
#h = int(website)
#print(h)
w1 = 'http://imgur.ex.frade.emulab.net/'
embedded[website] = []
embedded[website] = x
embedded['http://imgur.ex.frade.emulab.net/'] = x
print(embedded[website])
print(embedded['http://imgur.ex.frade.emulab.net/'])
for k,v in embedded.items():
    print(k,v)
