#ifndef SENSOR_H
#define SENSOR_H

// Encompasses data about one sensor
struct sensor_data {
  char hostname [32];
  int host_num_sensors;
  int sensor_number;
  double data, acceptable_low, acceptable_high;
  char timestamp [32];
  int action;
};

// Takes a sensor_data struct and fills in the given string up to len
// with a nice debug formatted version of the data
void sdatatostr (struct sensor_data* sdata, char* dest, int len);

// Takes the data from sdata and packs it into a format that can be sent over the network.
// Returns the size of the serialized data.
// NOTE: dest should point to an ample number of bytes of available allocated memory just to be sure
int serializesdata (struct sensor_data* sdata, void* dest);

// Reads data in the serialized format and places it into the given structure
void deserializesdata (struct sensor_data* sdata, void* data);

// Reads the thermal sensor located at the given file, stores the result in temp.
// Returns 0 on success, 1 on failure
int read_sensor(char* name, float* temp);

#endif
