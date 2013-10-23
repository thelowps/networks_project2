#ifndef EZ_TCP_H
#define EZ_TCP_H

int ezconnect(int* sock, char* ip, int port);
int ezsend(int sock, unsigned char* data, int len);
int ezreceive(int sock, unsigned char* data, int len);
int ezlisten(int* sock, int port);
int ezaccept(int sock, int* conn);

#endif
