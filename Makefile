main: server.cc client.cc
	g++ server.cc -o server -g -Wall
	g++ client.cc -o client -g -Wall
clean:
	rm -f server client

