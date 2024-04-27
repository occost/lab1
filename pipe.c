#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int runCommand(char* curCMD, int inputFD, int* outputFD, int isLast){
    int procID;
    int fds[2];
    
	if (!isLast) { 
        if(pipe(fds)==-1){
            perror("pipe");
            return errno;
        }
    }
    procID=fork();
    
    if(procID==-1){
        perror("fork");
        return errno;
    }
	if(procID == 0){ // Child process
        if(inputFD != 0){
            if(dup2(inputFD, STDIN_FILENO)==-1){
                perror("dup2");
                return errno;
            }
        }
        if (!isLast) {
            if(dup2(fds[1], STDOUT_FILENO)==-1){
                perror("dup2");
                return errno;
            }
        }
		close(fds[0]);
        execlp(curCMD, curCMD, NULL);
        exit(1);
    } 
    else { // Parent 
        if (!isLast) {
            close(fds[1]); 
            *outputFD = fds[0]; 
        }
        if(inputFD != 0) {
            close(inputFD); 
        }
        wait(NULL); 
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int input = 0;
    int output;
    
	int isLast=0;
    for(int i = 1; i < argc; i++){
        if(i == argc-1)
			isLast=1;
			
        runCommand(argv[i], input, &output, isLast);
        if (!isLast) { 
            input = output;
        }
    }

    return 0;
}
