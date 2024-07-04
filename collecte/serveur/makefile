CC = g++
CFLAGS = -Wall -Wextra -pthread  
FLAGS = -lmysqlcppconn 
serveur: serveur.o
	$(CC) $(CFLAGS) $^ $(FLAGS) -o $@

serveur.o: serveur.cpp 
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f *.o serveur 