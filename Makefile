all:
	gcc -o client client.c
s:
	./server
c:
	./client
clean:
	rm -fr server
	rm -fr client