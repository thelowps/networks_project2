#ifndef SENSOR_READER_H
#define SENSOR_READER_H

// Reads the thermal sensor located at the given file, stores the result in temp.
// Returns 0 on success, 1 on failure
int read_sensor(char* name, float* temp);

#endif
