import socket
import time

sockFd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sockFd.connect(('127.0.0.1',9981))

print 'send : hello'
sockFd.send('hello')
data = sockFd.recv(1024)
print 'recv : ',data

print 'send : nihao'
sockFd.send('nihao')
data = sockFd.recv(1024)
print 'recv : ',data

print 'send : dajiahao'
sockFd.send('dajiahao')
data = sockFd.recv(1024)
print 'recv : ',data

sockFd.close()
