all: thermd

thermd: thermd.o sensor.o eztcp.o
	gcc thermd.o sensor.o eztcp.o -o thermd

thermd.o: thermd.c
	gcc -c thermd.c

sensor.o: sensor.c
	gcc -c sensor.c	

eztcp.o: eztcp.c
	gcc -c eztcp.c

clean:
	rm -rf *.o therm thermd