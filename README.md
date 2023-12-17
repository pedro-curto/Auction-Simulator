# Projeto RC 2022/2023 - Grupo 44

## Compiling

The project can be compiled by running `make` in this directory.

Once compiled, two binaries, `user` and `AS` will be placed in this directory.

## Running the user

All commands work as per the specification.
available commands: 
/////////
State is not saved between sessions.
The player tries to quit the current game before exiting, even when receiving
a SIGINT or SIGTERM signal.

### Available constants

These constants, defined in `src/common/constants.hpp`, might be changed for testing,
as requested by the teachers:

- `UDP_TIMEOUT_SECONDS`: The amount of seconds between giving up on receiving a
  UDP reponse.
- `UDP_RESEND_TRIES`: The amount of times to try to send a UDP packet before
  giving up.
- `TCP_READ_TIMEOUT_SECONDS`: The read timeout for TCP connections. If the connected
  server does not write within this time period, the client closes the connection.
- `TCP_WRITE_TIMEOUT_SECONDS`: The write timeout for TCP connections. If the connected
  server does not ack within this time period, the client closes the connection.
////////////
## Running the server

The server saves data in the folders `auctions` and `users`, so while
testing it might make sense to delete the folders after each test.

The server is extremely resilient to all kinds of incorrect input and errors,
attempting to recover from them when possible.

///////
The server also handles the SIGINT signal (CTRL + C), waiting for existing TCP
connections to end. The user can press CTRL + C again to forcefully exit the
server.//////

We've decided to use threads for supporting multiple TCP connections at the same time.
We use file and dir locks to //////synchronize access to shared variables.//////////


////////////
### Available constants

These constants, defined in `src/common/constants.hpp`, might be changed for testing:

- `TCP_WORKER_POOL_SIZE`: The number of threads handling TCP connections, that is,
  the maximum number of current TCP connections supported.
- `TCP_READ_TIMEOUT_SECONDS`: The read timeout for TCP connections. If the connected
  client does not write within this time period, the server closes the connection.
- `TCP_WRITE_TIMEOUT_SECONDS`: The write timeout for TCP connections. If the connected
  client does not ack within this time period, the server closes the connection.

  /////////////