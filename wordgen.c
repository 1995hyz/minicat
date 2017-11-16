#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void main(int argc,char *argv[]){
	int total=0;
	int digits=0;
	int endless=0;
	int randomChar=0;
	if((argc==1)||(strcmp(argv[1],"0")==0)){
		endless=1;
	}
	else{
		total=atoi(argv[1]);
	}
	while((total>0)||(endless)){
		digits=(int)((((float) rand())/RAND_MAX)*7+3);//The word are 3 to 10 chars
		int i=0;
		while(i<digits){
			randomChar=(int)((((float) rand())/RAND_MAX)*25+65);
			fprintf(stdout,"%c",randomChar);
			i++;
		}
		fprintf(stdout,"\n");
		if(!endless){
			total--;
		}
	}
	exit(0);
}
