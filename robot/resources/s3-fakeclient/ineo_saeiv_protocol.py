#!/usr/bin/python2

import socket


def connect_on(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect((host, int(port)))
    return sock


def send_sync(sock, request):
    sock.send(request.encode('iso-8859-1') + "\0")
    response = ""
    while True:
        response += sock.recv(1)
        if response[-1] == '\0':
            break
    return response[0:-1].decode('iso-8859-1')

