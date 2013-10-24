#ifndef EZ_TCP_H
#define EZ_TCP_H

void ezsetprinterror(int p);
int ezconnect(int* sock, char* ip, int port);
int ezsend(int sock, void* data, int len);
int ezreceive(int sock, void* data, int len);
int ezlisten(int* sock, int port);
int ezaccept(int sock);

#endif
