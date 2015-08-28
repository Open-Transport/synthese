#!/usr/bin/python2

import socket


def create_request_from(messageTemplate):
    request = file(messageTemplate).read() + '\0'
    return request


def connect_on(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect((host, int(port)))
    print("connected")
    return sock


def send_sync(sock, request):
    sock.send(request + "\0")
    response = ""
    while True:
        response += sock.recv(1)
        if response[-1] == '\0':
            break
    return response[0:-1]

