#!/bin/python

from socket import *
import sys

def chat(connection_socket, username):
    # begins a chat session
    sep = "> "
    input = ""
    while 1:
        while len(input) == 0 or len(input) > 500:
            input = raw_input("{}> ".format(username))
        if input == "\quit":
            print "Connection closed"
            print "Waiting for new connection"
            break
        
        to_send = "".join((username,sep,input))
        connection_socket.send(to_send)
        received = connection_socket.recv(501)
        if received == "":
            print "Connection closed"
            print "Waiting for new connection"
            break
        print "{}".format(received)
        input = ""

def handshake(connection_socket):
    # get the client's first message
    print "{}".format(connection_socket.recv(1024))

if __name__ == "__main__":
    # check for correct number of args
    if len(sys.argv) != 2:
        print "You must give the server a port number"
        exit(1)
    serverport = sys.argv[1]
    serversocket = socket(AF_INET, SOCK_STREAM)
    # bind the socket to the port
    serversocket.bind(('', int(serverport)))
    # listen on the port
    serversocket.listen(1)
    # get the username
    username = ""
    while len(username) == 0 or len(username) > 10:
        username = raw_input("name must be 10 characters or less: ")
        print "chat session began"
    while 1:
        # loop until sig interupt
        # create a new socket for a new connection
        connection_socket, address = serversocket.accept()
        # received a connection
        print "new connection - address {}".format(address)
        # receive initial port message
        handshake(connection_socket)
        # start chat loop
        chat(connection_socket, username)
        # close the connection
        connection_socket.close()
