#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

int pipefd[3]={0,1,'\0'};
int pipefd2[3]={3,4,'\0'};

void main(int argc,char *argv[]){
	//int pipefd[2];
	pid_t pidWordgen;
	pid_t pidWordsearch;
	pid_t pidPager;
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
			char *argvWordgen[2];
			argvWordgen[0]=&argvW[0];
			argvWordgen[1]=NULL;
			if((execvp("/home/yingzhi/Desktop/wordgen",argvWordgen))==-1){
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
		if(pipe(pipefd2)==-1){
			fprintf(stderr,"Pipe Error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
		pidWordsearch=fork();
		if(pidWordsearch==-1){
			fprintf(stderr,"Fork Wordsearch Error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
	        if(pidWordsearch==0){
			close(pipefd[1]);       //Wordsearch closes write pipe linking to wordgen
			close(pipefd2[0]);	//Wordsearch closes read pipe linking to pager
			dup2(pipefd[0],0);
			dup2(pipefd2[1],1);
			close(pipefd[0]);
			close(pipefd2[1]);
			char *argvWordsearch[2];
			char argvWs[128];
			strcpy(argvWs,"/home/yingzhi/Desktop/wordsearch\0");
			argvWordsearch[0]=&argvWs[0];
			argvWordsearch[1]=NULL;
			if((execvp("./wordsearch\0",argvWordsearch))==-1){
				fprintf(stderr,"Exec wordsearch Error: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		else{	
			pidPager=fork();
			if(pidPager==-1){
				fprintf(stderr,"Fork Pager Error: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			if(pidPager==0){
				close(pipefd[0]);
				close(pipefd[1]);
				close(pipefd2[1]);	//Pager closes write pipe linking to Wordsearch
				dup2(pipefd2[0],0);
				close(pipefd2[0]);
				char *argvPager[2];
				char argvPa[128];
				strcpy(argvPa,"/home/yingzhi/Desktop/pager\0");
				argvPager[0]=&argvPa[0];
				argvPager[1]=NULL;
				if((execvp("/home/yingzhi/Desktop/pager\0",argvPager))==-1){
					fprintf(stderr,"Exec pager Error: %s\n",strerror(errno));
				}
			}
			else{
				fprintf(stderr,"pidWordgen:%d, pidWordsearch:%d\n",pidWordgen,pidWordsearch);
				pid_t waitReturn=0;
				int status;
				int status2;
				close(pipefd[1]);
				close(pipefd[0]);
				close(pipefd2[1]);
				close(pipefd2[0]);
				while((waitReturn=wait(&status))>0){
					if(waitReturn==-1){
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
}
