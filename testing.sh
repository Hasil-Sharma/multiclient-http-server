#!/usr/bin/env bash

set -x

HOST='127.0.0.1'
PORT=8888
HTTP_REQ_DELIM='\r\n'
HTTP_REQ_END='\r\n\r\n'

# Normal Requests
(echo -en "GET / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET / HTTP/1.0${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET / HTTP/1.1${HTTP_REQ_DELIM}Connection: keep-alive${HTTP_REQ_END}GET / HTTP/1.1${HTTP_REQ_DELIM}Connection: keep-alive${HTTP_REQ_END}") | nc $HOST $PORT

(echo -en "POST / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_DELIM}Content-Length: 2${HTTP_REQ_END}12") |  nc $HOST $PORT
(echo -en "POST / HTTP/1.1${HTTP_REQ_DELIM}Content-Length: 2${HTTP_REQ_DELIM}Connection: keep-alive${HTTP_REQ_END}12GET / HTTP/1.1${HTTP_REQ_DELIM}Connection: keep-alive${HTTP_REQ_END}") | nc $HOST $PORT

(echo -en "GET /Temp/ HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET /Temp/index.html HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
# HTTP Version Error
(echo -en "GET / HTTP/1.2${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET / HTTP/1.2${HTTP_REQ_END}") | nc $HOST $PORT

(echo -en "POST / HTTP/1.2${HTTP_REQ_DELIM}Content-Length: 2\r\nConnection: Close${HTTP_REQ_END}12") | nc $HOST $PORT
(echo -en "POST / HTTP/1.2${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST / HTTP/1.2${HTTP_REQ_END}") | nc $HOST $PORT

# Not Implemented Error
(echo -en "GETS / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "OPTIONS / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POSTS / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "HELLO / HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT

(echo -en "GETS / HTTP/1.0${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "OPTIONS / HTTP/1.0${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POSTS / HTTP/1.0${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "HELLO / HTTP/2.0${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT

# Invalid URI Error
(echo -en "GET /new|.txt HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET new.txt HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET /hello/try!<> HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT

(echo -en "POST /new|.txt HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST new.txt HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /hello/try!<> HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT

# Invalid File Type 
(echo -en "GET /new.txt2 HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /new.txt2 HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /new.kpg HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT

#Not Found Error
(echo -en "GET /new.html HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /new.html HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "GET /temp/new.html HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /temp/ HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
(echo -en "POST /temp/ HTTP/1.1${HTTP_REQ_DELIM}Connection: Close${HTTP_REQ_END}") | nc $HOST $PORT
set +x


