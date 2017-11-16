#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


void main(){
	size_t len=0;
	size_t len2=0;
	char *line=NULL;
	char *linetty=NULL;
	int n=0;
	int m=0;
	int lineCount=0;
	int redirect=0;
	FILE *fd;
	fd=fopen("/dev/tty","r");
	if(fd==NULL){
		perror("tty Error:");
	}
	while(1){
        errno=0;    
		n=getline(&line,&len,stdin);
		if(n<0){
            if(errno!=0){
		    	perror("Pager getline Error:");
            }
			break;
		}
		printf("%s",line);
		free(line);
		line=NULL;
		lineCount++;
		if(lineCount==23){
			printf("---Press RETURN for more---\n");
			while(1){
				m=getline(&linetty,&len2,fd);
				if(m<0){
					if(errno==0){
						exit(0);
					}
					else{
						fprintf(stderr,"getline from /dev/tty Error: %s\n",strerror(errno));
						exit(1);
					}
				}
				if(strncmp(linetty,"\n",1)==0){
					lineCount=0;
					free(linetty);
					linetty=NULL;
					break;
				}
				else if(strcmp(linetty,"q\n")==0){
					fprintf(stderr,"Page Terminated by q Command\n");
					exit(0);
				}
				else{
					fprintf(stderr,"Please Press Return\n");
					free(linetty);
					linetty=NULL;
				}
			}
		}
	}
	exit(0);	
}
