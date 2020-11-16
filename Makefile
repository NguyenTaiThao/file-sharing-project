all:
	clear
	gcc -o client client.c
s:
	clear
	./server
c:
	clear
	./client
clean:
	clear
	rm -fr server
	rm -fr client