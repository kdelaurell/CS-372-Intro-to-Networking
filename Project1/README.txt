First ensure that you have all the files necessary to run the chat program and
and in the same directory:
-chatserver.py
-chatclient.c
-makefile

If this is your first time running it type the following into the command line:
  make chatclient

then set up your server on one terminal by typing in:
  python chatserver.py [PortNumber]

You now have your server up and running and awaiting for connections on
localhost at your chosen port number.

Now open up a new terminal and type into the command line(make sure to use the
same port number you chose above):
  chatclient localhost [PortNumber]

On the ChatClient Terminal:
  1. Type in your username in on the chatclient side and then press enter.
  2. Then type in the first message you would like to send
  3. Wait until you receive a response and then you can respond
  4. When you are done with the chat type "\quit" (no quotes) as your message
  and it will terminate your chat session

On the ChatServer Terminal:
  1. Wait until you receive a connection
  2. Once you have received a connection you will be prompted for the username
  so type in your username and press enter.
  3. Then type in the message you would like to send
  4. Wait until you receive a response and then you can respond
  5. When you are done with the chat type "\quit" (no quotes) as your message
  and it will terminate your chat session
