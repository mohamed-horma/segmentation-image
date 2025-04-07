CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi -g

# Règle pour compiler image
./bin/image: ./src/image.c ./include/image.h ./include/objet.h ./include/point.h ./include/type_obj.h
	$(CC) $(CFLAGS) -Iinclude $^ -o $@


# Règle pour compiler RAG, y compris moment.c et image.c
./bin/RAG: ./src/RAG.c  ./include/RAG.h ./include/image.h
	$(CC) $(CFLAGS) -Iinclude $^ -o $@
 
./bin/main: ./src/main.c ./src/RAG.c ./src/image.c ./include/RAG.h  ./include/image.h
	$(CC) $(CFLAGS) -Iinclude $^ -o $@ -lm

# Nettoyage des fichiers générés
clean:
	rm -f ./bin/RAG ./bin/image ./bin/main