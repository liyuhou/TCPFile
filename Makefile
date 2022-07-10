all:
	gcc server.c svrDeal.c -o server.out
	gcc client.c cltDeal.c -o client.out
clean:
	rm server.out
	rm client.out