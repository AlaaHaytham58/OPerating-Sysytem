#include <sys/types.h> 
#include <sys/ipc.h>   
#include <sys/msg.h>   
#include <stdio.h>     
#include <string.h>    
#include <stdlib.h>    
#include <unistd.h> 
#include<signal.h>  
struct msgbuf {
    long mtype;       
    char mtext[256];   // Message data
};

int main(){
   /* key_t upkey=ftok("Desktop/upfile",65);
    key_t downkey=ftok("Desktop/downfile",66);
   
    if (upkey == -1) {
        perror("ftok upkey failed");
        exit(1);
    }
    if (downkey == -1) {
        perror("ftok downkey failed");
        exit(1);
    }
        */
    int upidq=msgget(1,IPC_CREAT|0666);
    int downidq=msgget(2,IPC_CREAT|0666);
    if (upidq == -1) {
        perror("msgget up queue failed");
        exit(1);
    }
    if (downidq == -1) {
        perror("msgget queue failed");
        exit(1);
    }
    struct msgbuf msg;
    
    pid_t my_pid = getpid();
    msg.mtype = my_pid % 10000;  
    while(1)
{   printf("Enter your message: ");
    if (fgets(msg.mtext, sizeof(msg.mtext), stdin) == NULL) {
        perror("Error reading input");
        return 1;
    }

    int send=msgsnd(upidq,&msg,sizeof(msg.mtext),!IPC_NOWAIT); //up message to be send
    if(send==-1)
    {
        perror("failend to send message to reciever ");
    }
    int recieve=msgrcv(downidq,&msg,sizeof(msg.mtext),msg.mtype,!IPC_NOWAIT);
    if (recieve== -1) {
        perror("msgrcv failed");
        exit(1);
    }
    printf("received Message: %s\n", msg.mtext);
}
}