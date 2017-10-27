README FOR HTTP/1.x PROTOCOL BASED TCP WEB SERVER

About:
------------------------------------------------------------------------------

This code is a TCP based webserver that works for HTTP/1.x protocol. It uses fork to handle multiple requests.

It consists of a makefile that can be used to generate the executable

execute $make command to generate executable

To run the webserver execute the following command

$./webserver

This shall start the webserver. The code doesnot require command line arguements as it make use of a configuration file to to extract all required information

Configuration file:
-------------------------------------------------------------------------------
The code uses the configuration for the reuired information like the default webpage for the webserver, port number for the server, root location for the webserver

Donot modify the lines that donot start with # in th conf file
Also edit only the words followed by a space in lines that donot start with #

The conf file also consists of a list for supported file types, additional file types may be supported by addition to the conf file not otherwise

Working of the code:
-------------------------------------------------------------------------------
On execution the code shall extract the port number and the root directory from the configuration file and start the webserver

The clients could then connect to the webserver using the browser or applications like telnet and netcat knowing the portnumber of the webserver and the localhost ip.

Each new connection is treated seperately using fork thus providing multiple connection at once. Each request from the client is the processed. The code only supports GET and POST methods all other METHODS shall return a NOT implemented error. Also if the requested File is not found the code shall return a Not found error. The code also checks for correct HTTP protocol requested since our code only supports 1.0 and 1.1. The reply from the server shall be in the same protocol as the request. The reply from the server shall also include filetype and file length in the reply header

Errors:
-------------------------------------------------------------------------------
400 - Bad Request - For wrong Methods,HTTP format,url format
404 - Not Found -  File not found on server
501 - Not implemented - For correct method that is not imlemented in the server
500 - Internal Server error
