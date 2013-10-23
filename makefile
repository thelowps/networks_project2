all: therm thermd

thermd: thermd.o eztcp.o
	gcc thermd.o eztcp.o -o thermd

therm: therm.o sensor_reader.o eztcp.o
	gcc therm.o sensor_reader.o eztcp.o -o therm

therm.o: therm.c
	gcc -c therm.c

sensor_reader.o: sensor_reader.c
	gcc -c sensor_reader.c	

eztcp.o: eztcp.c
	gcc -c eztcp.c

clean:
	rm -rf *.o therm thermd