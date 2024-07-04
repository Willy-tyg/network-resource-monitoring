CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -pthread -lstdc++fs

client: client.o
	$(CC) $(CFLAGS) $^ -o $@ -lcurl

client.o: client.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o client


