#!/usr/bin/env python3


import threading
import socket
import random
import string
import time
import sys

PORT = 43212

if len(sys.argv) == 2:
    PORT = int(sys.argv[1])

print('flood port %d' % PORT)


def random_string():
    n = int(random.random()*3000)
    s = '1'
    for i in range(n):
        s += random.choice(string.ascii_letters)
    return s


class Worker(threading.Thread):

    def run(self):
        global PORT
        self.count = 0
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        try:
            s.connect(('localhost', PORT))
        except:
            return
        for i in range(1000):
            data = random_string().encode('ascii')
            self.count += len(data)
            s.sendall(data)
            s.recv(4024)
        s.close()

    def join(self):
        super(Worker, self).join()
        return self.count

workers = []
start = time.time()
for i in range(5):
    w = Worker()
    w.start()
    workers.append(w)

count = 0
for i in range(5):
    count += workers[i].join()

end = time.time()

print('time: %s, total: %s' %(int(end-start), count))
