#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

struct sensor_data {
  char hostname [32];
  int host_num_sensors;
  int sensor_number;
  double data, acceptable_low, acceptable_high;
  char timestamp [32];
  int action;
};

#endif
