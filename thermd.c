/** 
 *  thermd.c
 *  Written by Andi Cescolini and David Lopes
 *  Computer Networks, 2013
 *  University of Notre Dame
 *
 *  A simple multithreaded TCP server that accepts data from clients and
 *  organizes the data in files.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sensor_data.h"
#include "eztcp.h"

int main(int argc, char** argv) {

  if (argc < 2) {
    printf("usage: %s <port>\n", argv[0]);
    return 0;
  }
  
  int sock, conn;
  ezlisten(&sock, atoi(argv[1]));
  ezaccept(sock, &conn);
  
  struct sensor_data sdata;
  ezreceive(conn, (unsigned char*)&sdata, sizeof(sdata));

  printf("Received data from: %s\nSensor number: %d, with temperature: %lf\n", 
	 sdata.hostname, sdata.sensor_number, sdata.data);

  close(conn);
  return 0;
}
