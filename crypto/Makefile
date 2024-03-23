all: test


LDFLAGS=-lsodium
CXXFLAGS=-O3

test: test.cc
	g++ test.cc -o test $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f test