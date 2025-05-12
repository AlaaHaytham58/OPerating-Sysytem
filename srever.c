#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>


struct msgbuf {
    long mtype;       
    char mtext[256]; 
};

int msgid_up, msgid_down;

void cleanqueue(int signum) {
    msgctl(msgid_up, IPC_RMID, NULL);
    msgctl(msgid_down, IPC_RMID, NULL);
    printf("I terminate queues");
    exit(0);
}

void convert_case(char *text) {
    for (int i = 0; text[i]; i++) {
        if (islower(text[i])) {
            text[i] = toupper(text[i]);
        } else if (isupper(text[i])) {
            text[i] = tolower(text[i]);
        }
    }
}

int main() {
    signal(SIGINT, cleanqueue);

  /*  key_t upkey = ftok("upfile", 65);
    key_t downkey = ftok("downfile", 66);
    if (upkey == -1) {
        perror("ftok  upkey failed");
        exit(1);
    }
    if (downkey == -1) {
        perror("ftok downkey failed");
        exit(1);
    }*/
    msgid_up = msgget(1, 0666 | IPC_CREAT);
    msgid_down = msgget(2, 0666 | IPC_CREAT);
    if (msgid_up == -1) {
        perror("msgget up queue failed");
        exit(1);
    }
    if (msgid_down == -1) {
        perror("msgget down queue failed");
        exit(1);
    }
    struct msgbuf msg;
   
    while (1) {
        int recieve=msgrcv(msgid_up, &msg, sizeof(msg.mtext), 0, !IPC_NOWAIT);
        if (recieve== -1) {
            perror("msgrcv failed not recieved correctly");
            exit(1);
        }
        convert_case(msg.mtext);
        int send=msgsnd(msgid_down, &msg, sizeof(msg.mtext), !IPC_NOWAIT);
        if(send==-1)
        {
            perror("failend to send message to client ");
        }
    }

    return 0;
}