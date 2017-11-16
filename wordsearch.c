#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int wordListSize=4096;
char **wordList;
int match=0;
void handler();

void main(int argc,char *argv[]){
	if(argc==0){
		fprintf(stderr,"Need Word List\n");
		exit(1);
	}
	else{	FILE *fd;
		size_t len=0;
		size_t len2=0;
		int totalLine=0;
		int n=0;
		int m=0;
		//int match=0;
		int i=0;
		char a=0;
		signal(SIGPIPE,handler);
		wordList=(char**) malloc(wordListSize*sizeof(char*)+1); 
		char *line=NULL;
		fd=fopen("./words.txt","r");
		if(fd==NULL){
			fprintf(stderr,"Open File Error, File:%s, Error:%s\n",argv[1],strerror(errno));
			exit(1);
		}
		n=getline(&line,&len,fd);
		if(n<0){
			fprintf(stderr,"Read File Error, File:%s, Error:%s\n",argv[1],strerror(errno));
			exit(1);
		}
		else{	
			while(n>0){
				totalLine++;
				wordList[i]=(char*) malloc(n*sizeof(char)+1);
				strcpy(wordList[i],line);
				i++;
				if(i>wordListSize){
					wordListSize+=4096;
					wordList=(char**) realloc(wordList,1+wordListSize*sizeof(char*));
				}
				free(line);
				line=NULL;
				n=getline(&line,&len,fd);
			}
			perror("");
		}
		fclose(fd);
		free(line);
		line=NULL;
		int lineWords=16;
		unsigned long totalChar=0;
		char lineStdin[4096];
		while(1){
			char *log=NULL;
			m=getline(&log,&len2,stdin);
			int checkFlag=0;
			char bufStdin[9]; 
			if(m==-1){
				break;
			}
			for(int j=0;j<totalLine;j++){
				if(strlen(wordList[j])==strlen(log)){
					if((strcmp(wordList[j],log))==0){
						match++;
						fprintf(stdout,"%s",wordList[j]);
					}
				}
			}
			free(log);
			log=NULL;
		}
		fprintf(stderr,"Matched %d Word(s)\n",match);
	}
	exit(0);
}

void handler(){
	fprintf(stderr,"Matched %d Word(s)\n",match);
	exit(13);
}
	
