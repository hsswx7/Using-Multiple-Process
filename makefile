all: slave master
slave: slave.c 
	gcc -g -o slave slave.c
master: master.c 
	gcc -g -o master master.c
make clean: 
	rm -rf *.o
	rm -rf *.out
