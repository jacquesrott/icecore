#!/usr/bin/env python

import socket
import uuid


TCP_IP = '127.0.0.1'
TCP_PORT = 6666
BUFFER_SIZE = 1024
MESSAGE = "Hello world {}".format(uuid.uuid1())


while True:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print "[CONNECT] {}:{} - {}".format(TCP_IP, TCP_PORT, s)
    try:
        s.connect((TCP_IP, TCP_PORT))
        s.send(MESSAGE)
        data = s.recv(BUFFER_SIZE)
        print "[WRITE] {}".format(data)
    except socket.error as e:
        print e
    finally:
        s.close()
        print "[CLOSED] {}:{}".format(TCP_IP, TCP_PORT)
