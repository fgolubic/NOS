#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h> 

typedef void (*sighandler_t)(int);

sighandler_t sigset(int sig, sighandler_t disp);

char* pipe_names[10] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };

void retreat(int status){

   
    /* unlink pipes */
         
    for(int i = 0; i < 10; i++){

      unlink(pipe_names[i]);

    }

   exit(0);

}


int main(void){

   for( int i = 0; i < 10; i++){
         
         if( mkfifo(pipe_names[i], S_IRUSR|S_IWUSR) == -1){

            perror("Creating pipeline\n");

         }
   }

   sigset(SIGINT, retreat);

   for(int j = 0; j < 5; j++){
      
      if( fork() ==  0 ){

         char temp[1];
         sprintf(temp, "%d", j);
         
         //predaj procesu (filozofu) njegov index, desni za pisanje, lijevi za citanje, desni za citanje, lijevi za pisanje
         execl("./philosophers.out", temp, pipe_names[j],  pipe_names[(j+4)%5], pipe_names[j + 5], pipe_names[(j+4)%5 + 5], NULL);
      }

   }

   for(int i = 0; i < 5; i++) wait(NULL);

   return 0;

}
