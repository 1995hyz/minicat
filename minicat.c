#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

void argumentTestor(int argc,char *argv[],int index);
void fileCopy(int argc,char *argv[],int outstart,int instart,int totalByte);
int fd,control,fd1,fd2,closeReturn1,closeReturn2;
int bufferSize=8192;							//default buffer size

int main(int argc, char *argv[]){
	if(strcmp(argv[1],"-b")==0){
		if(argc==2){printf("Argument Format Error:Expected [-b int #>0]\n");}
		if(argc>2){
			bufferSize=atoi(argv[2]);
			if(bufferSize<=0){printf("Argument Format Error:Expected [-b int #>0]\n");}
									//if there is a "number" following b 
			else{	control=3;
				argumentTestor(argc,argv,control);	//if the argument has b	
			}
		}
	}
	else{	control=1;
		argumentTestor(argc,argv,control);			//if the argument doesn't have b 
	}
	return 1;
}

void argumentTestor(int argc,char *argv[],int index){
	if((argc>index)&&(strcmp(argv[index],"-o")==0)){
		if((fd=open(argv[index+1],O_RDONLY))<0){		//if the output cannot be open,stop,report error
			printf("Attemptted Open File:%s\n",argv[index+1]);
			perror("Open OUTPUT File Error");
		}
		else{   if(argc>index+2){fileCopy(argc,argv,index+1,index+2,bufferSize);} //the argument has output file and it is accessible
			else{fileCopy(argc,argv,index+1,0,bufferSize);}	//the argument doesn't have input file but have output file
		}		
	}
	else{   if((argc==1)||(strcmp(argv[1],"-b")==0)){fileCopy(argc,argv,0,0,bufferSize);}//the argument doesn't have output and input file
		else{fileCopy(argc,argv,0,index,bufferSize);}		//the argument doesn't have output file but have input file
	}
}

void fileCopy(int argc,char *argv[],int outstart,int instart,int totalByte){
	int totalRead=0;
	int totalWrite=0;	
	if(outstart>0){
		fd2=open(argv[outstart],O_CREAT|O_WRONLY|O_TRUNC,o666);	//open output file,set file descriptor 
	}
	else{	fd2=1;							//open stdout	
	}	
	for(int i=instart;i<argc;i++){	
		int readReturn;
		int writeReturn;
		if((strcmp(argv[i],"-")==0)||(instart==0)){		//open stdin if the argument is "-"
			fd1=0;
			if(instart==0){i=argc;};
		}
		else{	fd1=open(argv[i],O_RDONLY<0);		//open input file
			if(fd1<0){
				printf("Attemptted Open File:%s\n",argv[i]);
				perror("Open INPUT File Error");
				continue;
			}
		}
		while(1){					//write until read() returns 0
			char buf[totalByte]; 
			readReturn=read(fd1,buf,totalByte);
			if(readReturn<0){				//Generating read error
				if(fd1==0){printf("Attemptted Read from Stdin\n");}
				else{printf("Attemptted Read File:%s\n",argv[i]);}
				perror("Read Error");
				printf("Total %d bytes have been read\nTotal %d bytes have been writen\n",totalRead,totalWrite);
				break;
			}
			if(readReturn>=0){totalRead+=readReturn;}	
			writeReturn=write(fd2,buf,readReturn);
			if(writeReturn<0){				//Generating write error	
				if(fd2==0){printf("Attemptted Write to Stdout\n");}
				else{printf("Attemptted Write File:%s\n",argv[outstart]);}
				perror("Write Error");
				printf("Total %d bytes have been read\nTotal %d bytes have been writen\n",totalRead,totalWrite);
				break;
			}
			if(writeReturn>=0){
				totalWrite+=writeReturn;
				char *buf2;
				while(writeReturn<readReturn){		//if write function fails writing all buffer, try write again
					if(writeReturn<0){
						printf("Attemptted Read:%s\n",argv[i]);
						perror("Write Error");
						printf("Total %d bytes have been read\nTotal %d bytes have been writen\n",totalRead,totalWrite);
						exit(EXIT_FAILURE);
					}
					buf2=&buf[readReturn-writeReturn-1];
					write(fd2,buf2,readReturn-writeReturn);
				}
			}
			if(readReturn==0){break;}
		}
		if((fd1!=0)||(i==argc-1)){
			closeReturn1=close(fd1);
			if(closeReturn1==-1){				//Generating close error
				printf("Attemptted Close Input:%s\n",argv[i]);
				perror("Close Error");
			}
		}
	}
	closeReturn2=close(fd2);
	if(closeReturn2==-1){						//Generating close error
		if(fd2!=1){printf("Attemptted Close Output:%s\n",argv[outstart]);}
		else{printf("Attemptted Close Stdout");}
		perror("Close Error");
	}
}

