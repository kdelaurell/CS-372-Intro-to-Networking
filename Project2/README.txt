First ensure that you have all the files necessary to run the file transfer program and
and in the same directory:
-ftclient.py
-ftserver.c
-makefile
-any test txt files

If this is your first time running it type the following into the command line:
  make ftserver

then set up your server on one terminal by typing in:
  ftserver [PortNumber]

You now have your server up and running and awaiting for connections on
at your chosen port number.

Now open up a new terminal and type into the command line(make sure to use the
same port number you chose above):
  python ftclient.py [flipServer] [port] [command] [filename(optional)] [dataPortNumber]

The command:

-1 gives you a list of files in the directory that are available for transfer
-g gets the [filename] file that is in the directory
