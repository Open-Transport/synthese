#!/usr/bin/env python2.4

##
#	@defgroup m70 70 Client terminal
#	@{

import os,sys,socket,datetime

HOST = 'localhost'
PORT = 3591


#try:
	#print 'connecting...'
	#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#s.connect((HOST, PORT))
#except socket.error, msg:
#	s.close()
#	print 'could not open socket'
#	sys.exit(1)
	
# now=datetime.datetime.now()

for i in range (1,10000):
	print 'connecting...'
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	s.send("fonction=map&toto=titi\n")
	print 'waiting answer...'
	get = s.recv(1024)
	print get
	print ''
	s.close()


