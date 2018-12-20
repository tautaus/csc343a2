CFLAGS = -DPORT=\$(PORT) -Wall -g -std=c99
PORT=37229

hcq_server:hcq_server.o hcq.o
	gcc $(CFLAGS) -o hcq_server hcq_server.o hcq.o

hcq_server.o: hcq_server.c
	gcc $(CFLAGS) -c hcq_server.c

hcq.o: hcq.c hcq.h
	gcc $(CFLAGS) -c hcq.c

clean: 
	rm *.o
