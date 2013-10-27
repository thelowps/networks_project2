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


//push testing

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sensor.h"
#include "eztcp.h"

// #define DEBUG

// Receives and unpacks data
int receive_sensor_data (int conn, struct sensor_data* sdata) {
  void* ser = malloc(1024);
  int ser_size;
  int err = ezreceive(conn, &ser_size, sizeof(int));
  if (err <= 0) {    // ERROR RECEIVING DATA
    exit(1);
  }
  ezreceive(conn, ser, ser_size);
  deserializesdata(sdata, ser);
  return err;
}

// Write two sensor datas to a file
// TODO : this function needs to be thread safe, I think.
//        maybe we need to lock the file? not sure
int write_data_to_file (struct sensor_data* sdata1, struct sensor_data* sdata2) {
  char full_filename[30] = "/var/log/therm/temp_logs/";
  char filename[30] = "g18_";
  snprintf(filename+4, 5, "%s", sdata1->timestamp);
  filename[8] = '_';
  snprintf(filename+9, 3, "%s", sdata1->timestamp+5);
  filename[11] = '_';
  snprintf(filename+12, 33, "%s", sdata1->hostname);
  
  strcat(full_filename, filename);
#ifdef DEBUG 
  printf("Server: writing to file %s\n", full_filename);
#endif
  
  FILE* file = fopen(full_filename, "a");
  if (file == NULL) {
    return -1;
  }
  
  char toprint [50];
  sprintf(toprint, "%s %.2lf %.2lf\n", sdata1->timestamp, sdata1->data, sdata2->data);
#ifdef DEBUG
  printf("Printing to file: %s\n", toprint);
#endif
  fprintf(file, toprint);
  fclose(file);
  return 0;
}

// Handles a single client
void handle_client (int conn) {
  int i = 0;
  struct sensor_data sdata[2];

  do {
    if (receive_sensor_data(conn, sdata+i) <= 0) {
      // TODO : error in receive -- kill thread?
#ifdef DEBUG
      fprintf(stderr, "FATAL ERROR RECEIVING DATA\n");
      fprintf(stderr, "EXITING\n");
#endif
      exit(1);
    }
    
#ifdef DEBUG 
    char debug[1024];
    sdatatostr(sdata+i, debug, 1024);
    printf("Received sensor data: \n%s\n\n", debug);
#endif


  // Send status packet
    if (sdata[i].action == 1) {
      int status = sdata[i].data > sdata[i].acceptable_high;
      ezsend(conn, &status, sizeof(int));
    }
    
    ++i;
  } while (i < sdata[0].host_num_sensors);
  
  // Write data to file
  if (sdata[0].action == 0) {
    write_data_to_file(sdata, sdata+1);
  }
  
  close(conn);
  exit(1);
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
  
  int status;
  while (1) {
    conn = ezaccept(sock);
    waitpid(-1, &status, WNOHANG);
    int pid = fork();
    if (pid == 0) {
      handle_client(conn);
    } else if (pid > 0) {
      close(conn);
    } else if (pid < 0) {
      close(conn);
      printf("Error on fork()\n");
      exit(1);
    }
  }
  
  return 0;
}
