# encoding=utf8


def accept(socket):
    print(socket)
    return True


def recv(socket, data):
    socket.prepare_data(data)
    return True
