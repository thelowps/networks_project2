#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mhash.h>
#include <math.h>

// Helper function to call sendto and catch errors
int send_and_error (int sockfd, const void* buffer, size_t len, int flags,
		     const struct sockaddr* to_addr, socklen_t addrlen) {
  int s = sendto(sockfd, buffer, len, flags, to_addr, addrlen);
  if (s < 0) {
    perror("SERVER: ERROR IN SEND");
    exit(0);
  }
  return s;
}

// Helper function to call recvfrom and catch errors
int recv_and_error (int sockfd, void* buffer, size_t len, int flags,
		     struct sockaddr* to_addr, socklen_t* addrlen) {
  int n = recvfrom(sockfd, buffer, len, flags, to_addr, addrlen);
  if (n < 0) {
    perror("SERVER: ERROR IN RECEIVE");
    exit(0);
  } 
  return n;
}

// Helper function to compute the MD5 hash of a file
void compute_file_hash (FILE* fp, long file_size, unsigned char hash[16]) {
  MHASH td;
  td = mhash_init(MHASH_MD5);
  if (td == MHASH_FAILED) exit(1);
  
  // HASH FILE IN BLOCKS
  int file_block_size = 512; // how many bytes to hash with every block
  int file_block_amount = ceil(file_size/(float)file_block_size);
  char* file_block_data = malloc(file_block_size * sizeof(char));
  int bytes_read, i;
  for (i = 0; i < file_block_amount; ++i) {
    bytes_read = fread(file_block_data, 1, file_block_size, fp);
    mhash(td, file_block_data, bytes_read);
  }

  mhash_deinit(td, hash);

  fseek(fp, 0, SEEK_SET);
  free(file_block_data);
}

// Helper function to print out a hash
void print_hash (unsigned char* hash, size_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    printf("%.2x", hash[i]);
  }
}

// MAIN
int main(int argc, char** argv) {

  if (argc < 2) {
    printf("usage: %s <port>\n", argv[0]);
    return 0;
  }

  int listenfd, connfd, n;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t clilen;
  char mesg[1000];
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));
  bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
  
  listen(listenfd, 1); // backlog of 1?
  
  while (1) {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    printf("SERVER: Accepted a new client.\n");

    // RECEIVE SIZE OF FILENAME
    int16_t filename_size;
    recv_and_error(connfd, &filename_size, 2, 0, (struct sockaddr *)&cliaddr, &clilen);
    filename_size = ntohs(filename_size) + 1;
    //printf("Received filename size: %u\n", filename_size);
    
    // RECEIVE FILENAME
    char* filename = malloc((filename_size) * sizeof(char));
    memset(filename, 0, filename_size);
    recv_and_error(connfd, filename, filename_size, 0, (struct sockaddr *)&cliaddr, &clilen);
    //printf("Received filename: %s\n", filename);
    
    // OPEN FILE
    char* location = "/afs/nd.edu/coursefa.13/cse/cse30264.01/files/Project1/";
    char* full_filename = malloc(sizeof(char) * (strlen(location) + filename_size));
    memset(full_filename, 0, sizeof(char) * (strlen(location) + filename_size));
    strcpy(full_filename, location);
    strcat(full_filename, filename);
    FILE* fp = fopen(full_filename, "rb");
    if (fp == NULL) {
      // send 0, end connection
      int response = 0;
      send_and_error(connfd, &response, sizeof(response), 0, (struct sockaddr *)&cliaddr, clilen);
      close(connfd);
      continue;
    }
    
    // SEND FILE SIZE
    fseek(fp, 0, SEEK_END);
    int32_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int32_t filesize_to_send = htonl(filesize);
    printf("SERVER: Sending filesize: %d\n", filesize);
    send_and_error(connfd, &filesize_to_send, sizeof(filesize_to_send), 0, (struct sockaddr *)&cliaddr, clilen);
    
    // COMPUTE AND SEND MD5 HASH
    unsigned char hash[16];
    compute_file_hash(fp, filesize, hash);
    printf("SERVER: Sending hash: ");
    print_hash(hash, 16);
    printf("\n");
    send_and_error(connfd, hash, 16, 0, (struct sockaddr *)&cliaddr, clilen);

    // READ AND SEND FILE
    int file_block_size = 1024; // how many bytes to send with every block
    int file_block_amount = ceil(filesize/(float)file_block_size);
    char* file_block_data = malloc(file_block_size * sizeof(char));
    int bytes_read, i;
    for (i = 0; i < file_block_amount; ++i) {
      bytes_read = fread(file_block_data, 1, file_block_size, fp);
      send_and_error(connfd, file_block_data, bytes_read, 0, (struct sockaddr *)&cliaddr, clilen);
    }
    printf("SERVER: Transfer complete.\n");
    fclose(fp);
    
    close (connfd);
  }

  return 0;
}
