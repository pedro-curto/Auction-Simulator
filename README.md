# Projeto RC Auction - Grupo 63 - 1º Semestre, P2, 2023/2024 LEIC-A

## Compiling

The project can be compiled by running `make` in this directory.

Once compiled, two binaries, `user` and `AS` are placed in the same directory for
the client and the server, respectively.

## Running the user

The user application can be run with:
```
./user [-n ASIP] [-p ASport]
```
where both fields between [] are optional: 
* ASIP is the IP address of the machine where the AS is running, 
and if omitted, it should run on the same machine;
* ASport is the port where the AS is listening, and if omitted, 
it should listen on port 58063 (5800 + GN), where our GN is 63.

The user can interact with the server through commands. The commands are:

- login UID password
- logout
- unregister
- open name asset_fname start_value timeactive (opens a new auction)
- close AID (an ongoingauction)
- myauctions or ma (lists the auctions started by this user)
- mybids or mb (lists the auctions for which this user made a bid)
- list or l (lists all the auctions)
- show_asset AID or sa AID (requests and receives the asset file for an auction)
- bid AID value or b AID value (makes a bid for an ongoing auction)
- show_record AID or sr AID (shows the record, relevant information, of an auction)
- exit (terminates the user application)

The user can use the list, show_asset and show_record commands without being logged in;
however, the other commands require the user to be logged in.

The user application was tested with incorrect inputs and errors and is resilient to
them, attempting to recover from them whenever possible.

The assets that can be used in the open command are located in the local_assets folder 
within the user directory, and the assets received from the server are stored in
the assets folder within the user directory.


## Running the server

The server application can be run with:
```
./AS [-p ASport] [-v]
```
The -p ASport is an optional field, similar to what was explained above.
The -v flag is an optional flag that enables verbose mode, which causes the AS
to output, for each request, the IP and port that originated that request, the request
information and the UID of the user that made the request, respectively.

The server saves data in the folders `auctions` and `users` within the server directory, 
so while testing it might make sense to delete the folders after each test. 
The server creates both these folders if they don't exist on startup; the server's 
database structure mimics the structure present on the "guia.pdf" file provided; 
the logic and contents of each file and some auxiliar functions were also based on
the "guia.pdf" file.

The server was also tested with incorrect inputs and errors and is resilient to 
them, attempting to recover from them when possible.

The server ignores the SIGPIPE and the SIGCHLD signals, ignoring when a client
closes the connection and when a child process ends, respectively, and can be terminated
with CTRL+C.

The support of multiple clients is achieved through the use of fork() that 
generates child processes whenever a new TCP connection is established to the server,
using file locks to ensure data integrity.

We also used timeouts to ensure that the server doesn't get stuck waiting for an answer
that may never arrive, and to ensure the same for the client, that it doesn't get stuck
waiting for the server to send an answer that may never come for some reason.

### Authors
Pedro Sousa - 102664
Pedro Curto - 103091
