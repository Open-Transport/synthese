#!/usr/bin/env python2.4


import os,sys,socket,datetime

HOST = 'localhost'
PORT = 3591

#SITE = "684321060640005"
SITE = "184B02H89030914"

def service(query):
	try:
		print 'connecting...'
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((HOST, PORT))
		welcome=s.recv(64)
		print welcome
	except socket.error, msg:
		s.close()
		print 'could not open socket'
		sys.exit(1)
	now=datetime.datetime.now()
	f = open(now.strftime("%H%M%S")+ "_%s.html" % now.microsecond, 'w')
	print '>',query
	s.send(query+'\n')
	print 'waiting answer...'
	get = s.recv(1024)
	while get != '':
		f.write(get)
		get = s.recv(1024)
	print 'ok.\n'
	f.close()
	s.close()

##	fonctions de test


nbpa=2140

def stress():
	date=datetime.date.today().strftime("%Y%m%d")
	for depart in range(1,nbpa+1):
		for arrivee in range(1,nbpa+1):
			query="site="+SITE+"&fonction=fh&npad="+str(depart)+"&npaa="+str(arrivee)+"&date="+date+"&ndd=0&nda=0&per=0&vel=-1&han=-1&tax=-1&tar=-1"
			service(query)

def page_accueil():
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((HOST, PORT))
		welcome=s.recv(64)
	except socket.error, msg:
		s.close()
		print 'could not open socket'
		sys.exit(1)
	query="site="+SITE+"&fonction=ac"
	print '>',query
	s.send(query+'\n')
	print 'waiting answer...'
	print s.recv(10240)
	print 'ok.\n'
	s.close()


def fiche_horaire():
	depart=str(raw_input('Depart: '))
	arrivee=str(raw_input('Arrivee: '))
	date=str(raw_input('Date: '))
	if date=="":
		date=datetime.date.today().strftime("%Y%m%d")
	query="site="+SITE+"&fonction=fh&npad="+depart+"&npaa="+arrivee+"&date="+date+"&ndd=0&nda=0&per=0&vel=-1&han=-1&tax=-1&tar=-1"
	service(query)

def test_arrets():
	pass

def test_resa():
	pass

### corps du script

while 1:
	print ""
	print " 1) Page d'accueil"
	print " 2) Fiche horaire"
	print " 3) Test de tous les arrets"
	print " 4) Test resa"
	print " 5) Stress"
	print ""
	print " s) Definir le site"
	print " q) Quitter"
	print ""
	chx=raw_input('Choix: ')
	try:
		chxnb=int(chx)
		now=datetime.datetime.now()
		dir=now.strftime("%Y%m%d%H%M%S")+ "_%s_%d" % (now.microsecond,chxnb)
		os.mkdir(dir)
		os.chdir(dir)
		dir='ok'
	except:
		dir='ko'
	if chx=='1':
		page_accueil()
	if chx=='2':
		fiche_horaire()
	if chx=='3':
		test_arrets()
	if chx=='4':
		test_resa()
	if chx=='5':
		stress()
	if chx=='s':
		SITE=raw_input('Site: ')
	if chx=='q':
		break
	if dir=='ok':
		os.chdir('..')

print 'Bye'


