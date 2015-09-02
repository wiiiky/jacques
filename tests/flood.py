#!/usr/bin/env python3


import socket
import select
import time

PORT = 43212
pfd = select.epoll()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
s.connect(('localhost', PORT))

pfd.register(s, select.EPOLLIN|select.EPOLLOUT)

s.sendall(b'hello world')

count = 0

start = time.time()
running = True

size= 1024

while running:
    events = pfd.poll()
    for fileno, event in events:
        if event & select.EPOLLIN:
            count+=len(s.recv(size))
            if count > 1024*1024*50:
                running=False
        if event & select.EPOLLOUT:
            s.sendall(('a'*size).encode('ascii'))
        if event & select.EPOLLHUP or event & select.EPOLLERR:
            break;

end = time.time()

duration = end-start

print('time: %s -- %s MB/S' % (duration, count/1024/1024/duration))
