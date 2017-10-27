all:
	gcc tcp_server.c -o webserver
clean:
	rm webserver
