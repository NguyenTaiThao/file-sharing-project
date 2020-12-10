all:
	clear
	gcc -o client client.c
	gcc -o server server.c
s:
	clear
	./server 9090
c:
	clear
	./client 127.0.0.1 9090
clean:
	clear
	rm -fr server
	rm -fr client