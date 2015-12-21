#!/usr/bin/env python3


import socket
import select
import time
import sys
import random
import argparse


def parse_cmd():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--port', '-p', type=int, default=13221,
                        help='the port')
    parser.add_argument('--total', '-t', type=int, default=1000,
                        help='the total size of data to send, MB')
    parser.add_argument('--clients', '-c', type=int, default=5,
                        help='the count of client to send data')
    return parser.parse_args()


args = parse_cmd()
PORT = args.port
TOTAL = args.total * 1024 * 1024
CLIENTS = args.clients

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
for i in range(CLIENTS):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    s.connect(('localhost', PORT))
    sockets[s.fileno()] = s
    pfd.register(s, select.EPOLLIN | select.EPOLLOUT)
    s.sendall(pack('hello world'))

count = 0
wcount = 0

start = time.time()
running = True
writing = True

while running:
    events = pfd.poll()
    for fileno, event in events:
        s = sockets[fileno]
        if event & select.EPOLLIN:
            b = s.recv(2048)
            count += len(b)
            if count > TOTAL:
                running = False
        if writing and (event & select.EPOLLOUT):
            b = pack('a' * random.randint(100, 2048)) * random.randint(2, 5)
            wcount += len(b)
            if wcount > TOTAL:
                writing = False
                print('writing end')
            s.sendall(b)
        if event & select.EPOLLHUP or event & select.EPOLLERR:
            break

end = time.time()

duration = end - start

print('%sB, %sKB, %sMB' % (count, count / 1024, count / 1024 / 1024))
print('time: %s -- %s MB/S' % (duration, count / 1024 / 1024 / duration))
