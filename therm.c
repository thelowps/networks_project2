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
#include "sensor_data.h"
#include "sensor_reader.h"
#include "eztcp.h"

// quick and dirty
void gettimestamp(char* stamp, int len) {
  time_t rawtime;
  struct tm* timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (stamp, len, "%G %m/%d %H %M %S", timeinfo);
}

int main (int argc, char** argv) {
  
  // Setup filenames
  char* conf_filename = "/etc/t_client/client.conf";
  char* sensor_filename = "/dev/gotemp";
  char* sensor_filename2 = "/dev/gotemp2";

  /////////////////////////////////
  // READ THE CONFIGURATION FILE //
  /////////////////////////////////
  
  // Open the file
  FILE* conf_file = fopen("/etc/t_client/client.conf", "r");
  if (conf_file == NULL) {
    fprintf(stderr, "FATAL: Error opening configuration file.");
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

  printf("num_sensors = %d\nacceptable values: %.2f<-->%.2f : %.2f<-->%.2f\n", 
	 num_sensors, acceptable[0], acceptable[1], acceptable[2], acceptable[3]);


  //////////////////////////////
  // READ THE THERMAL SENSORS //
  //////////////////////////////
  float* sensor_data = (float*)malloc(sizeof(float) * num_sensors);
  read_sensor(sensor_filename, sensor_data);
  if (num_sensors>1) read_sensor(sensor_filename2, sensor_data+1);

  printf("sensor_data: %.2f : %.2f\n", sensor_data[0], sensor_data[1]);


  ////////////////////
  // SEND TO SERVER //
  ////////////////////
  struct sensor_data sdata;
  gethostname(sdata.hostname, 32);
  sdata.host_num_sensors = num_sensors;
  sdata.sensor_number = 0;
  sdata.data = sensor_data[0];
  sdata.acceptable_low = acceptable[0];
  sdata.acceptable_high = acceptable[1];
  gettimestamp(sdata.timestamp, 32);
  sdata.action = 0;

  
  int sock;
  ezconnect(&sock, "127.0.0.1", 9042);
  ezsend(sock, (unsigned char*)&sdata, sizeof(sdata));

  // Clean up!
  free(sensor_data);
  free(acceptable);

  return 0;
}
