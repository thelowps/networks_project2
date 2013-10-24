all: therm thermd

thermd: thermd.o sensor.o eztcp.o
	gcc thermd.o eztcp.o sensor.o -o thermd

thermd.o: thermd.c
	gcc -c thermd.c

therm: therm.o sensor.o eztcp.o
	gcc therm.o sensor.o eztcp.o -o therm

therm.o: therm.c
	gcc -c therm.c

sensor.o: sensor.c
	gcc -c sensor.c	

eztcp.o: eztcp.c
	gcc -c eztcp.c

clean:
	rm -rf *.o therm thermd