main: server.cc client.cc
	g++ server.cc -o server -g -Wall -O3
	g++ client.cc -o client -g -Wall -O3
clean:
	rm -f server client

