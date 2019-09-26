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

#define _XOPEN_SOURCE 500

/* message structure */
struct message {
    long mtype;
    char mtext[8192];
};

char* in[3] = {"šibice", "duhan", "papir" };

int msqid;
typedef void (*sighandler_t)(int);

sighandler_t sigset(int sig, sighandler_t disp);

void retreat(int status){

   
    /* destroy message queue */
         
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
             perror("msgctl");
             exit(1);
    }

   exit(0);

}

int main(void)
{
    int key = ftok(".", 'A');
    int i;
    pid_t pid;
    int first, second;

    //initialize rand
    srand(time(0));

    /* create  message queue */
    msqid = msgget(key, IPC_CREAT | 0600);
   
    if (msqid == -1 ) {
        perror("msgget");

        return EXIT_FAILURE;
    }
    
    sigset(SIGINT, retreat);

	printf("0 - šibice, 1 - duhan, 2 - papir\n");
	printf("---------------------------------\n\n");

    /* fork a child processes */

   for(i = 0; i < 3; i++){
      
      pid = fork();
      if( pid == 0){
         char temp[3];
         char temp2[50];
         sprintf(temp, "%d", i+1);
         sprintf(temp2, "%d", key);
         execl("./pusac.out", temp, temp2, NULL);
      }

   }

   while(1) {
        /* parent */
        
        /* Put two random ingredients on table*/
        first  = rand() % 3;
        while((second = rand() % 3) == first){};

        char sfirst[1];
        char ssecond[1];

        sprintf(sfirst, "%d", first);
        sprintf(ssecond, "%d", second);

        struct message message;
        //strcpy(message.mtext, sfirst);
        memcpy(message.mtext, sfirst, 2);
        strcat(message.mtext, ", ");
        strcat(message.mtext, ssecond);

        printf("Trgovac stavlja sastojke na stol: %s.\n", message.mtext);
        sleep(1);

        for(i = 0; i < 3; i++){
              //msgtype indicates index of created process
              message.mtype = i+1;
              if (msgsnd(msqid, (struct message*) &message, sizeof(long) + (strlen(message.mtext) * sizeof(char)) + 1, 0) == -1) {

                    perror("msgsnd");
                    return EXIT_FAILURE;
              }
        }
      
         

        //msgtype 13 indicates process request
        if (msgrcv(msqid, (struct message *)&message, sizeof(message)-sizeof(long), 13, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Trgovac prima: %s.\n", message.mtext);

         sleep(1);

         if(strcmp("Zahtjev", message.mtext) == 0){
            
            struct message allow;
            allow.mtype = 14; //msgtype 14 indicates premission response
            strcpy(allow.mtext , "Trgovac daje sastojke ");
            strcat(allow.mtext, sfirst);
            strcat(allow.mtext, ", ");
            strcat(allow.mtext, ssecond);

            printf("%s.\n", allow.mtext);
            sleep(1);

            if (msgsnd(msqid, &allow, sizeof(long) + (strlen(allow.mtext) * sizeof(char)) + 1, 0) == -1) {
                     perror("msgsnd");
                     return EXIT_FAILURE;
             }
            
         }

         else{
            perror("Unknown message rcv!\n");
            printf("Trgovac prima: %s.\n", message.mtext);
            exit(2);
         }

         

         //msgtype 15 indicates that proccess finished its critical section
         if (msgrcv(msqid, (struct message *)&message, sizeof(message)-sizeof(long), 15, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

         printf("Trgovac prima: %s.\n", message.mtext);
         
         sleep(1);
      }

    return EXIT_SUCCESS;
}
