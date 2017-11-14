//#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

void main(int argc,char *argv[]){
	int pipefd[2];
	pid_t pidWordgen;
	pid_t pidWordsearch;
	if(pipe(pipefd)==-1){
		fprintf(stderr,"Pipe Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	pidWordgen=fork();
	if(pidWordgen==-1){
		fprintf(stderr,"Fork Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(pidWordgen==0){	//Child Process
		close(pipefd[0]);	//Wordgen closes read pipe
		dup2(pipefd[1],1);
		close(pipefd[1]);
		if(argc==1){
			char argvW[128]={"/home/yingzhi/Desktop/wordgen\0"};
			char *argvWordgen=&argvW[0];
			if((execvp("/home/yingzhi/Desktop/wordgen",&argvWordgen))==-1){
				fprintf(stderr,"Exec wordgen Error: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		if(argc==2){
			char *argvWordgen[3];
			char argvW[128];
			strcpy(argvW,"/home/yingzhi/Desktop/wordgen\0");
			argvWordgen[0]=&argvW[0];
			argvWordgen[1]=argv[1];
			argvWordgen[2]=NULL;
			if((execvp("./wordgen",argvWordgen))==-1){
				fprintf(stderr,"Exec wordgen Error: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}
	else{	
		pidWordsearch=fork();
		if(pidWordsearch==-1){
			fprintf(stderr,"Fork Error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
	        if(pidWordsearch==0){
			close(pipefd[1]);       //Wordsearch closes write pipe
			dup2(pipefd[0],0);
			close(pipefd[0]);
			char *argvWordsearch[2];
			char argvWs[128];
			strcpy(argvWs,"/home/yingzhi/Desktop/wordsearch\0");
			argvWordsearch[0]=&argvWs[0];
			argvWordsearch[1]=NULL;
			if((execvp("./wordsearch",argvWordsearch))==-1){
				fprintf(stderr,"Exec wordsearch Error: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else{	
			fprintf(stderr,"pidWordgen:%d, pidWordsearch:%d\n",pidWordgen,pidWordsearch);
			pid_t waitReturn=0;
			int status;
			int status2;
			close(pipefd[1]);
			close(pipefd[0]);
			while((waitReturn=wait(&status))>0){
				if((waitReturn=wait(&status))==-1){
					fprintf(stderr,"Wait Error: %s\n",strerror(errno));
					exit(EXIT_FAILURE);
				}
				if(WIFEXITED(status)){
					fprintf(stderr,"Terminated Process:%d\nNormal Terminated, Exit Code :%d\n",waitReturn,WEXITSTATUS(status));
				}
				else if(WIFSIGNALED(status)){
					if(WCOREDUMP(status)){
						fprintf(stderr,"Terminated Process:%d\nCore Dump\n",waitReturn);
					}
					else{
						fprintf(stderr,"Terminated Process:%d\nTerminated By Signal:%d\n",waitReturn,WTERMSIG(status));
					}
				}	
			}
		}
	}
}
