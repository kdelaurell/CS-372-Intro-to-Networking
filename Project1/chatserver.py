#import needed libraries
import socket
import sys

#checks if program is being used correctly
if len(sys.argv) !=  2:
    sys.exit("USAGE: python chatserver.py port\n")


#sets message length to 500
MES_LEN = 500
s = socket.socket()
#print "Socket successfully created"

#Sets port to user given port and connects to port
port = int(sys.argv[1])
s.bind(('', port))
#print "socket binded to %s" %(port)

#allows up to 5 connections
s.listen(5)
#print "socket is listening"

#keeps server running continuously until termiante command given
while True:

    #accepts connection and receives first message
   c, addr = s.accept()
   #print 'Got connection from', addr
   message = c.recv(MES_LEN)
   print message

   #gets username on server
   username = raw_input("What is your username? ")
   YourMessage = ""

   #continuously sends and receives messages until
   #a quit command is given and then closes connection
   while YourMessage != "\quit" and message != "\quit":
       print username, " > " ,
       YourMessage = raw_input()
       if YourMessage != "\quit":
           Totalmessage = username + " > " + YourMessage
           c.send(Totalmessage)
           message = c.recv(MES_LEN)
           if message != "\quit":
               print message
   c.send(YourMessage)
   if YourMessage == "\quit":
       print "You have terminated the chat session"
   else:
       print "Your chat partner has terminated the chat session"
   c.close()
