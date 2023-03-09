#!/bin/python3.10

"""
command and control server for the the Aegis suite
"""

import socket
import sys
import threading

class Anake:
    def __init__(self):
        self.sock = socket.socket() #socket object
        self.host = '0.0.0.0' #ip addr of host machine
        self.port = 42069 #port
        self.clients = [] #list of all clients
        try:
            self.sock.bind((self.host, self.port)) #binds server to port
        except socket.error as e:
            print(f'error binding server to port- see error message below\n{e}')
            self.__destroy__()

    def server_run(self):
        """main function"""
        self.sock.listen(10) #listens for incoming connections- queue of 10 incoming connections
        while True:
            client, addr = self.sock.accept()
            print(f'[+] new client from {addr[0]}:{str(addr[1])}')
            clientthread = threading.Thread(target=self.client_handler, args=(client, addr,))
            self.clients.append(clientthread)
            clientthread.start()


    def client_handler(self, sock, addr: list):
        sock.send(b'Anake has been awaiting you...')
        sock.close()
        return


server = Anake()
server.run()
