#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


void convert_case(char *str) {
    for (int i = 0; str[i]; i++) {
        if (islower(str[i])) {
            str[i] = toupper(str[i]);
        } else if (isupper(str[i])) {
            str[i] = tolower(str[i]);
        }
    }
}

int main() {
    
    int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  
        
        char *shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1) {
            perror("shmat failed in child");
            exit(1);
        }

        
        printf("Enter message: ");
        if (fgets(shmaddr, 4096, stdin) == NULL) {
            perror("fgets failed");
            exit(1);
        }
        shmaddr[strcspn(shmaddr, "\n")] = '\0';  
        if (shmdt(shmaddr) == -1) {
            perror("shmdt failed in child");
            exit(1);
        }
        sleep(1);
        shmaddr = shmat(shmid, NULL, SHM_RDONLY);
        if (shmaddr == (char *)-1) {
            perror("shmat failed in child");
            exit(1);
        }

        printf("Received message: %s\n", shmaddr);

        
        if (shmdt(shmaddr) == -1) {
            perror("shmdt failed in child");
            exit(1);
        }

        exit(0);
    } else {  
        
        char *shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1) {
            perror("shmat failed in parent");
            exit(1);
        }

        while (shmaddr[0] == '\0') {  
            usleep(100000);  
        }
        convert_case(shmaddr);
        if (shmdt(shmaddr) == -1) {
            perror("shmdt failed in parent");
            exit(1);
        }
        wait(NULL);
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(1);
        }

        printf("Shared memory terminated.\n");
    }

    return 0;
}