#!/usr/bin/env python3


import socket
import select
import time
import sys

PORT = 13221
COUNT = 500
if len(sys.argv) > 1:
    COUNT = int(sys.argv[1])
TOTAL = 1024*1024*COUNT
pfd = select.epoll()

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
s1.connect(('localhost', PORT))
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
s2.connect(('localhost', PORT))

pfd.register(s1, select.EPOLLIN|select.EPOLLOUT)
pfd.register(s2, select.EPOLLIN|select.EPOLLOUT)

s1.sendall(b'hello world')
s2.sendall(b'hello world')

count = 0

start = time.time()
running = True

size= 1024

while running:
    events = pfd.poll()
    for fileno, event in events:
        s = s1 if s1.fileno() == fileno else s2
        if event & select.EPOLLIN:
            count+=len(s.recv(size))
            if count > TOTAL:
                running=False
        if event & select.EPOLLOUT:
            s.sendall(('a'*size).encode('ascii'))
        if event & select.EPOLLHUP or event & select.EPOLLERR:
            break;

end = time.time()

duration = end-start

print('%sB, %sKB, %sMB' % (count, count/1024, count/1024/1024))
print('time: %s -- %s MB/S' % (duration, count/1024/1024/duration))
