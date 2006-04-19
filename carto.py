#!/usr/bin/env python

##
#	@defgroup m70 70 Client terminal
#	@{

import os,sys,socket,datetime

HOST = 'localhost'
PORT = 3591



# Load env.xml
# Load carto.xml
f=open('./env.xml', 'r')

envtxt=f.read ();
envtxt= envtxt.replace ("\n", "")

f=open('./map1.xml', 'r')
maptxt=f.read ();
maptxt= maptxt.replace ("\n", "")

request="fonction=map&mode=2&output=html&env=" + envtxt + "&map=" + maptxt + "\n"


for i in range (1,2):	
    print 'connecting...'
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    s.send(request)
	
    print 'waiting answer...'

    size = ""
    c = s.recv (1);
    while c != ':':
        size = size + c;
        c = s.recv (1)
        
    print size
        
    received = 0
    while received < int (size) :
        s.recv (1)
        received += 1
            
            
    print "Received ", received
    s.close()


