#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

struct packet {
  unsigned char measurements;
  unsigned char counter;
  int16_t measurement0;
  int16_t measurement1;
  int16_t measurement2; 
};

// Function to convert Celsius to Fahrenheit
float CtoF(float C){return (C*9.0/5.0)+32;}

int read_sensor(char* fileName, float* temperature) {
  float conversion=0.0078125;

  struct stat buf;
  struct packet temp;
  int fd;

  // Check if the file exists
  if(stat( fileName, &buf )) {
      if(mknod(fileName,S_IFCHR|S_IRUSR|S_IWUSR|S_IRGRP |S_IWGRP|S_IROTH|S_IWOTH,makedev(180,176))) {
	//fprintf(stderr,"Cannot creat device %s  need to be root",fileName);
	return 1;
      }
  }
  
  // If cannot open, check permissions on dev, and see if it is plugged in
  if((fd=open(fileName,O_RDONLY))==-1) {
    //fprintf(stderr,"Could not read %s\n",fileName);
    return 1;
  }
  
  // if cannot read, check is it plugged in
  if(read(fd,&temp,sizeof(temp))!=8) {
    //fprintf(stderr,"Error reading %s\n",fileName);
    return 1;
  }
  
  close(fd);

  // Success!
  *temperature = (CtoF(((float)temp.measurement0)*conversion));
  
  return 0;
}
