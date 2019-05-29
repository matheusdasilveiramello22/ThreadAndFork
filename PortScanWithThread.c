/*
How to run the code:
  Should create 20 threads on code.
  gcc -o PSWT PortScanWithThread.c
  ./PSWT
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>

// Let us create a global variable to change it in threads
int g = 0;
int porta_inicial = 1, porta_final = 65536, porta = 1, i, meusocket;
struct sockaddr_in host;
struct hostent *he;
struct servent *servico;

// The function to be executed by all threads
void *functionScan(void *vargp) {
  // Store the value argument passed to this thread
  int *myid = (int *)vargp;

  // Print the argument, static and global variables
  while (porta < porta_final) {
    he = gethostbyname("127.0.0.1");
    if (he == NULL) {
      printf("Erro: Host Desconhecido!!\n");
      exit(-1);
    }
    meusocket = socket(AF_INET, SOCK_STREAM, 0);
    if (meusocket < 0) {
      perror("Socket");
      exit(1);
    }
    host.sin_family = he->h_addrtype;
    host.sin_port = htons(porta);
    host.sin_addr = *((struct in_addr *) he->h_addr);
    bzero(&(host.sin_zero), 8);
    if (connect(meusocket, (struct sockaddr *) &host, sizeof (host)) == -1) {
      close(meusocket);
    } else {
      servico = getservbyport(htons(porta), "tcp");
      printf("A porta %d aberta\tO Servico e [%s]\n", porta, (servico == NULL) ? "Desconhecido" : servico-> s_name);
    }
    porta++;
    close(meusocket);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int contador;
  pthread_t tid;

  for (contador = 0; contador < 20; contador++)
    pthread_create(&tid, NULL, functionScan, (void *)&tid);

  pthread_exit(NULL);
  return 0;
}
