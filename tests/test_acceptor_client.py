import socket
import time

#client
sockFd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sockFd.connect(('127.0.0.1',9981))
sockFd.send('hello')
data = sockFd.recv(1024)
print data
sockFd.close()
