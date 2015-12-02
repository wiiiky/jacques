#!/usr/bin/env python3


import socket
import select
import time
import sys
import random

PORT = 13221
COUNT = 500
if len(sys.argv) > 1:
    COUNT = int(sys.argv[1])
TOTAL = 1024*1024*COUNT
pfd = select.epoll()


def pack(data):
    data = bytes(data, encoding='utf8')
    length = len(data)
    l1 = bytes([length & 0XFF])
    l2 = bytes([(length >> 8) & 0XFF])
    l3 = bytes([(length >> 16) & 0XFF])
    l4 = bytes([(length >> 24) & 0XFF])
    return l1 + l2 + l3 + l4 + data


def unpack(data):
    return data[0] + (data[1] << 8) +\
        (data[2] << 16) + (data[3] << 24)

sockets = {}
for i in range(10):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    s.connect(('localhost', PORT))
    sockets[s.fileno()] = s
    pfd.register(s, select.EPOLLIN | select.EPOLLOUT)
    s.sendall(pack('hello world'))

count = 0
wcount = 0;

start = time.time()
running = True

while running:
    events = pfd.poll()
    for fileno, event in events:
        s = sockets[fileno]
        if event & select.EPOLLIN:
            count += len(s.recv(2048))
            if count > TOTAL:
                running = False
        if event & select.EPOLLOUT:
            #if wcount < TOTAL:
            b = pack('a'*random.randint(100, 2048))
            #wcount += len(b)
            s.sendall(b)
        if event & select.EPOLLHUP or event & select.EPOLLERR:
            break

end = time.time()

duration = end-start

print('%sB, %sKB, %sMB' % (count, count/1024, count/1024/1024))
print('time: %s -- %s MB/S' % (duration, count/1024/1024/duration))
