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
#include "sensor.h"
#include "eztcp.h"

#define DEBUG

// Receives and unpacks data
int receive_sensor_data (int conn, struct sensor_data* sdata) {
  void* ser = malloc(1024);
  int ret = ezreceive(conn, ser, 1024);  
  deserializesdata(sdata, ser);
  return ret;
}

// Handles a single client
void handle_client (int conn) {
  int i;
  struct sensor_data sdata;
  for (i = 0; i < sdata.host_num_sensors; ++i) {
    if (receive_sensor_data(conn, &sdata) < 0) {
      // kill thread?
    }
    
#ifdef DEBUG 
    char debug[1024];
    sdatatostr(&sdata, debug, 1024);
    printf("Received sensor data: \n%s\n\n", debug);
#endif
    
  }

  close(conn);
}

int main(int argc, char** argv) {
  
  //////////////////////////
  // SETUP PORT LISTENING //
  //////////////////////////

  int sock, conn, port = 9779;
  if (ezlisten(&sock, port) < 0) { 
    exit(1); // eztcp already writes the error message to stderr
  }


  ///////////////
  // MAIN LOOP //
  ///////////////

  while (1) {
    conn = ezaccept(sock);
    handle_client(conn);
  }
  
  return 0;
}
