/** 
 *  therm.c
 *  Written by Andi Cescolini and David Lopes
 *  Computer Networks, 2013
 *  University of Notre Dame
 *
 *  A simple client file that reads data from thermal sensors, packs them into
 *  an appropriate structure, and sends the files to a client.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "sensor.h"
#include "eztcp.h"

// #define DEBUG

// quick and dirty
void gettimestamp(char* stamp, int len) {
  time_t rawtime;
  struct tm* timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (stamp, len, "%G %m %d %H %M", timeinfo);
}

// Appends a timestamp and newline and writes to /var/log/therm/error/g18_error_log
void write_to_error_log (const char* error) {
  char* msg = malloc(sizeof(char) * (strlen(error) + 37));
  gettimestamp(msg, 32);
  strcat(msg, " -- ");
  strcat(msg, error);
  strcat(msg, "\n");
  FILE* errfile = fopen("/var/log/therm/error/g18_error_log", "a");
  fprintf(errfile, msg);
  fclose(errfile);
}

int main (int argc, char** argv) {
  
  // Setup filenames
  char* conf_filename = "/etc/t_client/client.conf";
  char* sensor_filename = "/dev/gotemp";
  char* sensor_filename2 = "/dev/gotemp2";

  if (argc < 2){ //if an IP address isn't supplied
    printf("Please specific a server IP address\n");
    exit(1);
  }
  

  /////////////////////////////////
  // READ THE CONFIGURATION FILE //
  /////////////////////////////////
  
  // Open the file
  FILE* conf_file = fopen("/etc/t_client/client.conf", "r");
  if (conf_file == NULL) {
#ifdef DEBUG
    fprintf(stderr, "FATAL: Error opening configuration file.");
#endif
    write_to_error_log("Error opening configuration file.");
    return 1;
  }
  
  // Read for the number of sensors
  int num_sensors;
  fscanf(conf_file, "%d", &num_sensors);
  if (num_sensors == 0) {
    // No sensors on this host! Simply exit.
    printf("No sensors on this host.");
    return 0;
  }
  
  // Read for acceptable values
  float* acceptable = (float*)malloc(sizeof(float) * num_sensors * 2);
  int i;
  for (i = 0; i < num_sensors; ++i) {
    fscanf(conf_file, "%f %f", acceptable+i*2, acceptable+i*2+1);
  }
  
#ifdef DEBUG
  printf("num_sensors = %d\nacceptable values 1: %.2f, %.2f\n", num_sensors, acceptable[0], acceptable[1]);
  if (num_sensors>1) printf("acceptable values 2: %.2f, %.2f\n\n", acceptable[2], acceptable[3]);
#endif


  //////////////////////////////
  // READ THE THERMAL SENSORS //
  //////////////////////////////
  float* sensor_temp = (float*)malloc(sizeof(float) * num_sensors);
  int error = 0;
  error += read_sensor(sensor_filename, sensor_temp);
  if (num_sensors>1) error += read_sensor(sensor_filename2, sensor_temp+1);
  if (error) {
#ifdef DEBUG
    fprintf(stderr, "Error reading sensors.");
#endif
    write_to_error_log("Error reading sensors.");
    exit(1);
  }

#ifdef DEBUG
  printf("sensor_temp 1: %.2f\n", sensor_temp[0]);
  if (num_sensors>1) printf("sensor_temp 2: %.2f\n", sensor_temp[1]);
  printf("\n");
#endif


  ////////////////////
  // SEND TO SERVER //
  ////////////////////
  
  // Set eztcp to not print to stderr if we are not in debug mode
#ifndef DEBUG
  ezsetprinterror(0);
#endif

char * host; 
host = argv[1];

  // Connect to our server
  int sock;
  if (ezconnect(&sock, host, 9779) < 0) {
    write_to_error_log("Error connecting to server.");
    exit(1);
  }

  // Fill in structs and send each one to the server
  struct sensor_data* sdata = (struct sensor_data*)malloc(sizeof(struct sensor_data) * num_sensors);
  for (i = 0; i < num_sensors; ++i) { // Kinda silly to put this in a loop but it condenses the code
    gethostname(sdata[i].hostname, 32);
    sdata[i].host_num_sensors = num_sensors;
    sdata[i].sensor_number = i;
    sdata[i].acceptable_low = acceptable[i*2];
    sdata[i].acceptable_high = acceptable[i*2+1];
    sdata[i].data = sdata[i].acceptable_high-1 + i*2;//sensor_temp[i];
    gettimestamp(sdata[i].timestamp, 32);
    sdata[i].action = 1;

#ifdef DEBUG
    char debug [1024];
    sdatatostr(sdata+i, debug, 1024);
    printf("Sending packet:\n%s\n\n", debug);
#endif
    void* ser = malloc(1024);
    int err, ser_size = serializesdata(sdata+i, ser);
    err = ezsend(sock, &ser_size, sizeof(int));
    err = ezsend(sock, ser, ser_size);
    if (err <= 0) {
#ifdef DEBUG
      fprintf(stderr, "Error sending data to server.\n");
#endif
      write_to_error_log("Error sending data to server.");
    }

    if (sdata[i].action == 1) {
      // We need to receive a status packet from the server!
      int status;
      if (err = ezreceive(sock, &status, sizeof(int)) <= 0) {
	write_to_error_log("Error receiving status packet.");
      }
      
      if (status == 0) {
	printf("STATUS: OKAY!\n");
      } else if (status == 1) {
	printf("STATUS: OVERTEMP\n");
      }

    }

    free(ser);
  }
  
  // Clean up!
  free(sdata);
  free(sensor_temp);
  free(acceptable);

  return 0;
}
