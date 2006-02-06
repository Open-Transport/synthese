#!/usr/bin/env python2.4
# -*- coding:iso8859-1 -*-

import string,struct,sys,datetime

#charconv = ( "                                 "
#             "!\"#$%&'()*+,-./0123456789:;<=>?@"
#             "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
#             "ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~   "
#             "                                 "
#             "                              AA "
#             " A C  E I I   OOO    U      A A A"
#             "  CEEEE  II    O O  U UU   " )
charconv = ( "                                 "
             "               0123456789       "
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ      "
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ       "
             "                                 "
             "                              AA "
             " A C  E I I   OOO    U      A A A"
             "  CEEEE  II    O O  U UU   " )

version = 150
title = 'Transports Toulousains'
inchar = [chr(65+x) for x in range(26)] # ajouter les chiffres ?
inword = [
('IMPASSE','IMP'),
('RESIDENCE','RES'),
('PORTE','PT'),
('ROUTE','RT'),
('BOULEVARD','BD'),
('CHEMIN','CH'),
('AVENUE','AV'),
('PLACE','PL'),
('GRAND','GD'),
('SAINT','ST'),
('TOULOUSE','TLSE')
]
layer = ['+commune','+lieu','*communes','*lieux','-arret','-lieu']
nodeidx = {}
nodeid = [[] for x in layer]
node = []
link = []
ilink = []
olink = []
dict = []
ordered = []

# ATTENTION: a fixer en fonction de la machine cible
endian = '>' # > powerpc ; < intel

################ 

def addnode(lid,label):
	if label not in dict:
		dict.append(label)
	nodeid[lid].append(label)
	nodeidx[(lid,label)] = len(node)
	node.append((lid,dict.index(label)))
	ilink.append([])
	olink.append([])

def addlink((l1,i1),(l2,i2),weight):
	srcid = nodeidx[(l1,i1)]
	dstid = nodeidx[(l2,i2)]
	if srcid not in ilink[dstid]:
		link.append((srcid,dstid,weight))
		ilink[dstid].append((srcid,weight))
		olink[srcid].append((dstid,weight))

################ 

input = open('arrets.csv','r')
for char in inchar:
	addnode(0,char)
	addnode(1,char)
for word,alias in inword:
	addnode(0,alias)
	addnode(1,alias)
for line in input.readlines():
	try:
		[city,stop,best,pade] = line.rstrip().split(':')
	except:
		print 'erreur sur',line
		sys.exit(1)
	convcity = string.translate(city,charconv)
	convstop = string.translate(stop,charconv)
	tolink = [(0,2,convcity),(1,3,convstop)]
	for word in string.translate(best,charconv).split():
		if len(word) > 2:
			tolink.append((1,3,word))
	for l1,l2,label in tolink:
		if label not in nodeid[l2]:
			addnode(l2,label)
			for word,alias in inword:
				if word in label:
					addlink((l1,alias),(l2,label),len(word))
			for char in label:
				if char in string.ascii_letters:
					addlink((l1,char),(l2,label),1)
	final = pade # city+':'+stop # pade
	if pade[0] == '*': dstlyr = 4 # un arret
	else: dstlyr = 5 # un lieu
	if final not in nodeid[dstlyr]:
		addnode(dstlyr,final)
	addlink((2,convcity),(dstlyr,final),1)
	addlink((3,convstop),(dstlyr,final),1)
	for l1,l2,label in tolink[2:]:
		addlink((l2,label),(dstlyr,final),2)
input.close()
for lid in range(len(layer)):
	for label in nodeid[lid]:
		ordered.append(nodeidx[(lid,label)])


################ 

if len(node) > 65535:
	print "too much nodes for 16bit mlp"
	sys.exit(2)

################ 

ascii = open('arrets.net','w')
ascii.write('H mlp:'+str(version)+':'+title+'\n')
ascii.write('H layers:'+layer[0])
for label in layer[1:]: ascii.write(','+label)
ascii.write('\n')
ascii.write('H infos:'+str(len(dict))+'\n')
ascii.write('H alias:'+str(len(inword))+'\n')
ascii.write('H nodes:'+str(len(node))+'\n')
ascii.write('H links:'+str(len(link))+'\n')
did = 0
for word in dict:
	ascii.write('I %X %s\n' % (did,word))
	did += 1
aid = 0
for word,alias in inword:
	ascii.write('A %X %s %s\n' % (aid,word,alias))
	aid += 1
sid = 0
for nid in ordered:
	lid,did = node[nid]
	ascii.write('N %X %X %X # %s\n' % (sid,lid,did,dict[did]))
	sid += 1
lid = 0
for dstid in ordered:
	for nid,weight in ilink[dstid]:
		src = dict[node[nid][1]]
		dst = dict[node[dstid][1]]
		srcid = ordered.index(nid)
		ascii.write('L %X %X %X %d # %s->%s\n' % (lid,srcid,dstid,weight,src,dst))
		lid += 1
ascii.close()

################ 

output = open('arrets.bin','w')
output.write(struct.pack('64s',title))
now = datetime.date.today()
sign = ((now.year%100)<<24) + (now.month<<16) + (now.day<<8) + version
output.write(struct.pack(endian+'LHHHHL',sign,len(dict),len(inword),len(layer),len(node),len(link)))
for word in dict:
	output.write(word+'\0')
for word,alias in inword:
	output.write(word+'\0')
	output.write(alias+'\0')
offset = 0
for label in layer:
	size = len(nodeid[layer.index(label)])
	output.write(struct.pack('16s',label))
	output.write(struct.pack(endian+'HH',offset,size))
	offset += size
offset = 0
for nid in ordered:
	size = len(ilink[nid])
	did = node[nid][1]
	output.write(struct.pack(endian+'HHLL',did,0,offset,size))
	offset += size
for dstid in ordered:
	for srcid,weight in ilink[dstid]:
		output.write(struct.pack(endian+'Hh',ordered.index(srcid),weight))

output.close()
