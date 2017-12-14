#/*****************************************
#* Author: Kyle De Laurell
#* Date: 11/26/2017
#* Description: This is the source file for
#* a program that acts as an ftp client and requests
#* files or directory list
#*******************************************/

#imports needed libraries
import socket
import sys
import select
import os


#checks if program is being used correctly
if len(sys.argv) <= 4:
    sys.exit("USAGE: python ftclient.py host port command [filename] dataport\n")
if len(sys.argv) > 6:
    sys.exit("USAGE: python ftclient.py host port command [filename] dataport\n")

#grabs necessary variables depending on usage
host = sys.argv[1]
port = int(sys.argv[2])
command = sys.argv[3]
if(command == '-g'):
    filename = sys.argv[4]
    dataport = int(sys.argv[5])
elif(command == '-1'):
    dataport = int(sys.argv[4])
else:
    if len(sys.argv) == 5:
        dataport = int(sys.argv[4])
    else:
        filename = sys.argv[4]
        dataport = int(sys.argv[5])



#forks processes and one sets up server to accept files and
#the other talks to the server
newpid = os.fork()
if newpid == 0:
    #sets up connection to server
     sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
     server_address = (host + ".engr.oregonstate.edu", port)
     newhost = host + ".engr.oregonstate.edu"
     sock.connect((host, port))
     sock.send(command)
     data = sock.recv(256)

     #gets required information for each command
     if(command == '-g'):
         if(data == 'accept'):
             sock.send(str(dataport))
             data = sock.recv(256)
         if(data == 'accept'):
             sock.send(filename)
             data = sock.recv(256)
             if(data == "ERROR! Requested File Not Found"):
                 sock.close()
                 sys.exit(host + ":" + str(dataport) + "says file not found")
     elif(command == '-1'):
         if(data == 'accept'):
             sock.send(str(dataport))
     else:
         print data
         sock.send("OK")

     data = sock.recv(256)

     sock.close()

else:
    #sets up server to accept file
     MES_LEN = 500
     s = socket.socket()
     port = int(dataport)
     s.bind(('', port))
     s.listen(5)
     connectionAlive = True

     #accepts connection and executes for each command
     while connectionAlive:
        c, addr = s.accept()
        if(command == '-g'):
            message = c.recv(MES_LEN)
            c.send("OK")
            if message != "complete":
                cwd = os.listdir(os.getcwd())
                for tF in cwd:
                    if(tF == filename):
                        print "File already exists. What do you want to name file?"
                        newFile = raw_input()
                writeFile = open(newFile, 'w')
                writeFile.write(message)
                print "Receiving \"" + filename + "\" from " + host + ":" + str(dataport)
                while(message != "zyxvw"):
                    message = c.recv(MES_LEN)
                    if message != "zyxvw":
                        c.send("OK")
                        writeFile.write(message)
                writeFile.close()
                print "File Transfer Complete"
        elif(command == '-1'):
            message = c.recv(MES_LEN)
            while(message != "complete"):
                print message
                c.send("OK")
                message = c.recv(MES_LEN)

        connectionAlive = False
     c.close()
