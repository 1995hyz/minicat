#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
int fileCheck(char *path0,char *path1);
int checkdir(char *name,struct stat *oriFile,char *argv[],int bufSize,int dirAccessControl){
	DIR *dirp;
	struct dirent *de;
	if(!(dirp=opendir(name))){
		fprintf(stderr,"%s	TRYING OPEN DIRECTORY:%s\n",name,strerror(errno));
		return -1;
	}
	while(de=readdir(dirp)){
		int dirAccess=dirAccessControl;
		char subname[65536];
		if((strcmp(de->d_name,".")==0)||(strcmp(de->d_name,"..")==0)){
			continue;
		}
		if((bufSize+=sizeof(de->d_name))>65536){			//To prevent the pathname buffersize get overflow 
			fprintf(stderr,"Pathname Buffersize Overflow\n");
			return -2;
		}
		sprintf(subname,"%s/%s",name,de->d_name);
		int aFile;
		struct stat aFileStat;
		aFile=stat(subname,&aFileStat);
		if(aFile==1){
			fprintf(stderr,"%s	TRYING ACCESS STAT:%s\n",subname,strerror(errno));
			continue;
		}
		if(de->d_type==DT_DIR){
			if((unsigned long)aFileStat.st_mode%2==0){//If the mode number is even, it means "x"is 0;
				dirAccess=0;
			}
			int terminate=checkdir(subname,oriFile,argv,bufSize,dirAccess);//Recurse start
			if(terminate==-2){//This will terminate the recursive checkdir function and terminate the program
				return -2;
			}
		}
		else{
			if(de->d_type==DT_REG){
				int fileSize1;
				int fileSize0;
				if((oriFile->st_ino==aFileStat.st_ino)&&(aFileStat.st_dev==oriFile->st_dev)){
					char modeString[10];
					sprintf(modeString,"%lo",(unsigned long)aFileStat.st_mode);
					if((strcmp(&modeString[5],"3")>0)&&dirAccess){//The last digit of six digits number is greater than 3
						printf("%s	HARD LINK TO TARGET FILE	CAN BE READ BY OTHER\n",subname);
					}
					else{
						printf("%s	HARD LINK TO TARGET FILE	CANNOT BE READ BY OTHER\n",subname);
					}	
				}
				else{		
					if(aFileStat.st_size==oriFile->st_size){
						int fileFlag=fileCheck(argv[1],subname);
						if(fileFlag==1){
							int nlinkNumber=aFileStat.st_nlink;
							char modeStringD[10];
							sprintf(modeStringD,"%lo",(unsigned long)aFileStat.st_mode);
							if((strcmp(&modeStringD[5],"3")>0)&&dirAccess){
								printf("%s	DUPLICATE OF TARGET FILE(nlink=%d)	CAN BE READ BY OTHER\n",subname,nlinkNumber);
							}
							else{
								printf("%s	DUPLICATE OF TARGET FILE(nlink=%d)	CANNOT BE READ BY OTHER\n",subname,nlinkNumber);
							}
						}
						if(fileFlag==-2){
							return -2;
						}
					}
				}
			}
			else if(de->d_type==DT_LNK){
				if(oriFile->st_ino==aFileStat.st_ino){
					printf("%s	SYMLINK RESOLVES TO TARGET FILE\n",subname);
				}
				else if(((aFileStat.st_mode)&S_IFMT)==S_IFREG){
					if(aFileStat.st_size==oriFile->st_size){
						int fileFlagSym=fileCheck(argv[1],subname);	
						if(fileFlagSym==1){
							printf("%s	SYMLINK RESOLVES TO DUPLICATE FILE\n",subname);
						}
						if(fileFlagSym==-2){
							return -2;
						}
					}
				}
				else{
					printf("DEBUG:%s	LINKS TO SOMETHING NOT A FILE,SKIPPING\n",subname);
				}
			}	
		}
	}
	if(closedir(dirp)<0){
		fprintf(stderr,"%s	TRYING TO CLOSE DIRECTORY:%s",name,strerror(errno));
	}
	return 1;
}

int fileCheck(char *path0,char *path1){//This function compare two files that have the same size 
	int file0Des;
	int file1Des;
	int readF0;
	int readF1;
	char buf0[4096];
	char buf1[4096];
	char readControl=1;
	file0Des=open(path0,O_RDONLY);
	if(file0Des==-1){
		fprintf(stderr,"%s	OPEN TARGET FILE ERROR (Program terminated):%s\n",path0,strerror(errno));
		return -2;
	}
	file1Des=open(path1,O_RDONLY);
	if(file1Des==-1){
		fprintf(stderr,"%s	OPEN COMPARED FILE ERROR (The file will be skipped):%s\n",path1,strerror(errno));
		return -1;
	}
	while(readControl){
		readF0=read(file0Des,buf0,4096);
		readF1=read(file1Des,buf1,4096);
		if(readF0==-1){
			fprintf(stderr,"%s	READ TARGET FILE ERROR (Program terminated):%s\n",path0,strerror(errno));
			return -2;
		}
		if(readF1==-1){
			fprintf(stderr,"%s	READ COMPARED FILE ERROR (The file will be skipped):%s\n",path1,strerror(errno));
			return -1;
		}
		if(readF0==readF1){
			if(memcmp(buf0,buf1,readF0)!=0){
				readControl=0;
				return 0;
			}
			else if(readF0<4096){
				readControl=0;
			}
		}
		else{
			readControl=0;
			return 0;
		}
	}
	if(close(file1Des)){
		fprintf(stderr,"%s	FILE CLOSE ERROR:%s\n",path1,strerror(errno));
	}
	if(close(file0Des)){
		fprintf(stderr,"%s	FILE CLOSE ERROR:%s\n",path0,strerror(errno));
	}
	return 1;
}
void main(int argc,char *argv[]){
	struct stat originalFile;
	struct stat firstDir;
	int oriFileStu,firDir;
	oriFileStu=stat(argv[1],&originalFile);
	int dirAccessControl=1;
	if(oriFileStu==-1){
		fprintf(stderr,"%s	TARGET FILE PROBLEM:%s\n",argv[1],strerror(errno));
	}
	else{	firDir=stat(argv[2],&firstDir);
		if(firDir==-1){
			fprintf(stderr,"CANNOT ACCESS STAT AT STARTING POINT,\"READ BY OTHER\"INFORMATION MAY BE WRONG\n");
		}
		if(firstDir.st_mode%2==0){
			dirAccessControl=0;
		}
		checkdir(argv[2],&originalFile,argv,0,dirAccessControl);
	}
}

