/*
How to run the code:
  gcc -o PSWF PortScanWithFork.c
  ./PSWF
  Its possible to set the numbers of Forks that should be created
*/
#include <sys/ipc.h>
#include <sys/shm.h>
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
#define TRUE 1

int meusocket;
struct sockaddr_in host;
struct hostent *he;
struct servent *servico;

  void portScan(int initialPort, int finalPort) {
    printf("PID WORKING: %i \n", getpid());
    while (initialPort <= finalPort) {
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
      host.sin_port = htons(initialPort);
      host.sin_addr = *((struct in_addr *) he->h_addr);
      bzero(&(host.sin_zero), 8);
      if (connect(meusocket, (struct sockaddr *) &host, sizeof (host)) == -1) {
        close(meusocket);
      } else {
        servico = getservbyport(htons(initialPort), "tcp");
        printf("PORT: [%d] ABERTA\t SERVIÇO: [%s]\t PID: [%i]\n", initialPort, (servico == NULL) ? "Desconhecido" : servico-> s_name, getpid());
      }
      initialPort++;
      close(meusocket);
    }
  }

  int main(){
    pid_t childPid, wpid;
    int totalPorts = 65536, portToScanLimit, myPortToRead, shm_id, status = 0, forksQuantity;
    key_t key = ftok("shmfile",65);

    shm_id = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) { printf("shmget error\n"); }

    int *portToScan = (int*)shmat(shm_id, NULL, 0);
    *portToScan = 0;

    while(TRUE){
      printf("\nInforme a quantidade de processos que quer criar: \n");
      scanf("%i", &forksQuantity);
      if ((totalPorts % forksQuantity) == 0) {
        portToScanLimit = (totalPorts / forksQuantity);
        break;
      }
      printf("\n As 65536 portas precisam ser divisiveis por %i, sem restar nada. \n", forksQuantity);
    }

    // FATHER PROCESS
    for (int createForks = 0; createForks < forksQuantity; createForks++)
      // IF CHILD PROCESS THEN
      if ((childPid = fork()) == 0){
        myPortToRead = *portToScan;
        *portToScan+= portToScanLimit;
        break;
      }
    if (childPid == 0){
      // CHILD PROCESS
      printf("InitialPort: %i FinalPort: %i PID: %i \n", (myPortToRead + 1), (myPortToRead + portToScanLimit), getpid());
      portScan((myPortToRead + 1), (myPortToRead + portToScanLimit));
      exit(0);
    }
    else {
      // FATHER PROCESS
      while ((wpid = wait(&status)) > 0);
      printf("FinalPort: %i\n",*portToScan);
    }
  return 0;
  }
