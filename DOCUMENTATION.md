# Documentation

This is the documentation of the IPCLib, the layout of this document is the following:
- ServerSocket
- ClientSocket
- Setting up a connection and sending data

# ServerSocket

A class that represents the serverside of a socket connection. It has functions to asynchronously connect and receive data from a client and a synchronous function to send data to a client.

## Initialize

Initializes and starts the server.

## ConnectAsync

Spawns a child thread that waits for a connection, such that the main thread is not blocked.

## AwaitClientConnection

The main thread awaits untill a client has connected to the server, this makes sure that the server cannot send data to a client that is not connected. If this function is called without ConnectAsync() the main thread will connect with a client. Otherwise the main thread will wait on the sub thread until it has connected.

## SendData

Sends data to a connected client on the main thread. This data cannot be longer than IPC_BUFFER_BYTE_SIZE.

## ReceiveDataAsync

Spawns a child thread that waits for data from a client, such that the main thread is not blocked.

## GetData

Checks if data has been received in the child thread. If data was received it will return true, with the data in the buffers. If it did not it will return false. 

## AwaitData

Waits untill data is received from the client. If this function is called without ReceiveDataAsync() the main thread will wait until data is received on the socket. Otherwise it will wait until the sub thread returned data.

## Connected()

Checks if the server is connected to a client

## Disconnect

Disconnects the current client from the server

## CloseServer

Closes the server, and disconnects any client that is connected


# ClientSocket

A class that represents the clientside of a socket connection. It has functions to asynchronously connect and receive data from a server, and a synchronous function to send data to a server. Note that the client will connect to the server in the constructor, and can't connect if no server exists.

## Initialize

Initializes and connects the client to a server.

## SendData

Sends data to the server

## ReceiveDataAsync

Spawns a child thread that waits for data from the server, such that the main thread is not blocked.

## GetData

Checks if data has been received in the child thread. If data was received it will return true with the received data in the buffers. If it did not it will return false.

## AwaitData

Waits untill data is received from the server. If this function is called without ReceiveDataAsync() the main thread will wait until data is received on the socket. Otherwise it will wait until the sub thread returned data.

## Disconnect

Disconnects the client from the server


# Setting up a connction and sending data

This section will give a simple example of setting up a connection between a server and a client.

## Server side

```
  ServerSocket server;                // creates the server on localhost
  server.Initialize();                // initializes the server
  server.ConnectAsync();              // asynchronously wait on a client connection
  server.AwaitClientConnection();     // wait untill a client is connected
  server.SendData("Hello Client",12); // sends data to the client
  server.ReceiveDataAsync();          // asynchronously receive data from the client
  char buffer[50];                    // set up a buffer for the data
  server.AwaitData(buffer,50);        // wait untill data is received
```

## Client side

```
  ClientSocket client;            	  // creates the client and connects to localhost
  client.Initialize();                // initializes the client
  client.SendData("Hello Server",12); // sends data to the server
  client.ReceiveDataAsync();          // asynchronously receives data from the server
  char buffer[50];                    // set up a buffer for the data
  client.AwaitData(buffer,50)         // wait untill data is received
```



