#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>

struct req{
   int p;
   int t;
   int is_response;
}req;

char* pwd = "/home/filipgolubic/Nos/Lab1/Zad2/";

int updateclock(int t1, int t2){
      
   if(t1 > t2) return t1+1;
   
   else return t2 + 1;

}

int main(int argc, char** argv){
     
     int index = atoi(argv[0]);
     char*  rw= argv[1];
     char*  lr = argv[2];
     char* rr = argv[3];
     char* lw = argv[4];
     int k = 30;

     struct req req_a[20];
     int req_count = 0;
    
     int sticks[2];
     int rec_proc;

     //0 - left, 1 - right
     int readdescriptors[2];
     int writedescriptors[2];

      if(index == 0){

            if((writedescriptors[1] = open(rw, O_WRONLY)) == -1){
               
               printf("Process %d: error opening right write pipe: %s\n", index, rw);

           }

          

           if((readdescriptors[1] = open(rr, O_RDONLY )) == -1){
               
               printf("Process %d: error opening right read pipe: %s\n", index, rr);

           }

           if((readdescriptors[0] = open(lr, O_RDONLY)) == -1){
               
               printf("Process %d: error opening left read pipe: %s\n", index, lr);

           }
           

           if((writedescriptors[0] = open(lw, O_WRONLY)) == -1){
               
               printf("Process %d: error opening left write pipe: %s\n", index, lw);

           }
      }
      else{

         if((readdescriptors[0] = open(lr, O_RDONLY)) == -1){
               
               printf("Process %d: error opening left read pipe: %s\n", index, lr);

           }

         if((writedescriptors[0] = open(lw, O_WRONLY)) == -1){
               
               printf("Process %d: error opening left write pipe: %s\n", index, lw);

           }

         if((writedescriptors[1] = open(rw, O_WRONLY)) == -1){
               
               printf("Process %d: error opening right write pipe: %s\n", index, rw);

           }

          

           if((readdescriptors[1] = open(rr, O_RDONLY )) == -1){
               
               printf("Process %d: error opening right read pipe: %s\n", index, rr);

           }
      }
   

     int clock = 0;
          
     srand(time(0));

     int sleep_counter;

     for(int i = 0; i < 2; i++){
   
         if(fcntl(readdescriptors[i], F_SETFL, O_NONBLOCK) == -1){

               perror("Set nonblock.\n");      
   
         }

     }

     while(1){
         
         sleep_counter = rand()%10 + 1;
    
         printf("%*s", k*index, "");
         printf("Filozof %d misli.\n", index);

         //periodicki svake sekunde provjeravaj ima li zahtjeva i odgovaraj na njih
         while(1){

            sleep(1);

            struct req message;

            for(int i = 0; i < 2; i++){

                  int is_message = read(readdescriptors[i], &message, sizeof(struct req));

                  if(is_message > 0){

                        printf("%*s", k*index, "");
                        printf("Proces %d prima: (%d,%d,%d).\n", index, message.p, message.t, message.is_response);
                        clock = updateclock(clock, message.t);

                        message.p = index;
                        message.is_response = 1;
                        //samo odgovori na zahtjev
                         if(write(writedescriptors[i], &message, sizeof(message)) == -1 ){

                                 printf("Process %d: error writing to pipe %d\n", index, i);

                          }
                          
                          printf("%*s", k*index, "");
                          if(i == 1) rec_proc = (index + 1) % 5;
                          else rec_proc = ( index + 4 ) % 5;
                          printf("Proces %d ==> %d: (%d,%d,%d).\n", index,rec_proc, message.p, message.t, message.is_response);
                          sleep(1);

                  }

            }    

           //umanji razmisljanje za sekundu i ako je završilo izađi iz petlje
           sleep_counter--;

           if(sleep_counter == 0) break;

         } //kraj prve unutarnje while petlje


         //u ovom trenutku filozof želi jesti i šalje zahtjeve svom lijevom i desnom susjedu

         struct req request;
         request.p = index;
         request.t = clock;
         request.is_response = 0;

         for(int i = 0; i < 2; i++){

             if(write(writedescriptors[i], &request, sizeof(request)) == -1 ){

                   printf("Process %d: error writing to pipe %d\n", index, i);

             }
             
             printf("%*s", k*index, "");
             if(i == 1) rec_proc=(index+1)%5;
             else rec_proc = (index+4)%5;
             printf("Proces %d ==> %d: (%d,%d,%d).\n", index, rec_proc, request.p, request.t, request.is_response); 
             sleep(1);

          }//kraj slanja zahtjeva susjednim procesima

         //u ovom djelu prima zahtjeve susjednih procesa i odgovara sve dok nije dobio oba odgovora na svoje zahtjeve

         while(1){

            struct req message;

            for(int i = 0; i < 2 ; i++){

               int is_message = read(readdescriptors[i], &message, sizeof(struct req));

                if(is_message > 0){

                     printf("%*s", k*index, "");
                     printf("Proces %d prima: (%d,%d,%d).\n", index, message.p, message.t,message.is_response);
                     sleep(1);

                     clock = updateclock(clock, message.t);
                     
                     if((message.t == request.t) && message.is_response){

                           sticks[i] = 1;
                           break;

                     }      
                     else{

                           if(message.t < request.t || ( (message.t == request.t) && ( message.p < index)) ){
                              
                                    struct req my_response;

                                    my_response.t = message.t;
                                    my_response.p = index;
                                    my_response.is_response = 1;

                                    if(write(writedescriptors[i], &my_response, sizeof(my_response)) == -1 ){

                                          printf("Process %d: error writing to pipe %d\n", index, i);

                                    }
                                    
                                    printf("%*s", k*index, "");
                                    if(i == 1) rec_proc=(index+1)%5;
                                    else rec_proc = (index+4)%5;
                                    printf("Proces %d ==> %d: (%d,%d,%d).\n", index, rec_proc, request.p, request.t,request.is_response); 
                                    sleep(1);

                           }
                           else{
                        
                                 req_a[req_count] = message;
                                 req_count++;

                           }

                     } 
     
                }
            }

            if((sticks[0] == 1) && (sticks[1] == 1)){

                  break;

            }

         }

         

         //ulazak u KO: filozof ima oba stapica i krece jesti
         printf("%*s", k*index, "");
         printf("Filozof %d jede...\n", index);
         sleep(rand() % 10 + 5);
         printf("%*s", k*index, "");
         printf("Filozof %d završio.\n", index);

         sleep(1);

      
         //ocisti stapice
         sticks[0] = 0;
         sticks[1] = 0;

         int max = req_count;
         for(int j = 0; j < max; j++){
            
              //posalji odgovore iz reda zahtjeva odgovarajućim procesima
               
              struct req temp = req_a[j];
              int sent = temp.p;
              temp.p = index;
              temp.is_response = 1;

              if(sent == index){

               printf("ERROR: sent and recieved pid MUST be different!\n"); 

              }
              if( sent == ((index + 1) % 5) ){
                     
                   if(write(writedescriptors[1], &temp, sizeof(temp)) == -1 ){

                          printf("Process %d: error writing to pipe 1\n", index);

                    }
                    
               }
               else{
                  
                  if(write(writedescriptors[0], &temp, sizeof(temp)) == -1 ){

                        printf("Process %d: error writing to pipe 0\n", index);

                  }

               }
   
               req_count--;
               printf("%*s", k*index, "");      
               printf("Proces %d ==> %d: (%d,%d,%d).\n", index, sent, temp.p, temp.t,temp.is_response);                
               sleep(1);
            
         }

     }

   return 0;
}
