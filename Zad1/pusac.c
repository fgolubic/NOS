#include<signal.h>
#include<stdlib.h>    
#include<string.h>                                                                                      
#include<unistd.h>
#include<stdio.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>

/* message structure */
struct message {
    long mtype;
    char mtext[100];
};

//redoslijed sastojaka u polju određuje koji proces ima koji sastojak prema procInd
//0 - šibice, 1 - duhan, 3 - papir
char* in[3] = {"šibice", "duhan", "papir" };


int* parse(char* list){
     int first, second; 
     int *res = malloc(2 * sizeof(int));
     char* par = strtok(list, " ,");

     first = atoi(par);
     
     second = atoi(strtok(NULL, " ,"));
   
     if(strtok(NULL, " ,") != NULL) exit(1);
     res[0] = first;
     res[1] = second;
     

     return res;

}

int main(int argc, char** argv){

   int procInd = atoi(*argv);
   int key = atoi(*(argv + 1));
   struct message message;

   /* connect to message queue */
    int msqid = msgget(key, IPC_CREAT | 0600);
   
    if (msqid == -1 ) {
        perror("msgget");

        return 1;
    }

    srand(time(0));
    while(1){
      
       //primi poruku o sastojcima
       if (msgrcv(msqid, (struct message *)&message, sizeof(message)-sizeof(long), procInd, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

       printf("Pušač %d primio: %s.\n", procInd - 1, message.mtext);

      sleep(1);

      int* rec = parse(message.mtext);

      if( ((procInd - 1) != rec[0] ) && ((procInd - 1) != rec[1] )){

          char temp[1];
          sprintf(temp, "%d", procInd);

          struct message allow;
          allow.mtype = 13; //msgtype 13 indicates premission granted
          strcpy(allow.mtext , "Zahtjev");
          //strcat(allow.mtext, temp);

          printf("Pušač %d šalje: %s.\n", procInd - 1, allow.mtext);

          //šalji poruku za ulazak u kritični odsječak
          if (msgsnd(msqid, &allow, sizeof(long) + (strlen(allow.mtext) * sizeof(char)) + 1, 0) == -1) {
                     perror("msgsnd");
                     return 1;
          }

           sleep(1);

          if(msgrcv(msqid, (struct message *) &allow, sizeof(allow) - sizeof(long),  14, 0) == -1){
               perror("msgrcv");
               return 1;
          }

          printf("Pušač %d prima: %s.\n", procInd - 1, allow.mtext);

          //kriticni odsjecak
          printf("Pušač %d mota...\n", procInd - 1 );
          sleep(rand()%3 + 2);
          printf("Pušač %d puši.\n", procInd - 1);
          sleep(rand()%4 + 2);

          struct message finished;
          finished.mtype = 15;
          strcpy(finished.mtext, "Gotov");

          printf("Pušač %d šalje: %s.\n", procInd - 1, finished.mtext);

          //šalji poruku za izlazak iz kritičnog odsječka
          if (msgsnd(msqid, &finished, sizeof(long) + (strlen(finished.mtext) * sizeof(char)) + 1, 0) == -1) {
                     perror("msgsnd");
                     return 1;
          }

         sleep(1);

      }

      
    }

   return 0;

}




