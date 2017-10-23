# Multiclient-HTTP-Server
HTTP Server written in C which supports multiple client connections. Program handles multiple clients via forking, and while children are processing the requests parent process is available to accept other connections. Program also supports keeping the connection alive for a given time period, after which the server gracefully shutdowns.

To build the server run : make all
To start the server run : "./server_run"

Following HTTP methods are supported by the program :
- GET
- POST


## GET Method
Program expects the request to be in standard HTTP 1.0/1.1 format, depending on whether request pipelining is to be supported on not and replies with the appropriate file requested. In case of any thing is not working fine server responds with error messages, depending on what is wrong.

## POST Method
Program expects the request to be in standard HTTP 1.0/1.1 format, depending on whether request pipelining is to eb supported or not and replies with the appropriate file requested after appending the POSTDATA in correct format. In case anything is not working as expected the program responsds with error messages, depending on what is wrong.

## Request Pipelining
Program supports the functionality in case client wishes to send multiple HTTP requests over the same connection. Server reads the request from same connection till the time all of them are served and further waits for a given time period (configurable via ws.conf) before closing the connection. This avoids the connection setup overhead each time for sequence of requests. To use this functionality the request headers should have "Connection: keep-alive" paramter.

## ws.conf
This file contains all the configuration parameters such a keep-alive time, file types supported, document index and document path relative to which server reads all the files.

## Exiting the Server
Server runs in a forever loop, and keeps on accepts the connections. In case, it receives an interrupt signal (Ctrl + C) it stops taking new connections, and exits after children are done serving the pending requests. Thus, server shutdowns gracefully after serving all the requets.

