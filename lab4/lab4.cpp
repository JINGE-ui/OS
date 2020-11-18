#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctime>

void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir))==NULL){     
        printf("opendir error!\n");
        return;
    }
    if(chdir(dir)==-1){
    	printf("chdir error!\n");
    	return;
    }   
    printf("\ncurrent working directory: %s\n",getcwd(NULL,NULL));
    while((entry=readdir(dp))!=NULL){
    	lstat(entry->d_name,&statbuf);   
        if(S_ISDIR(statbuf.st_mode)){
        	char modestr[11];
        	strcpy(modestr,"----------");
        	 if (S_ISDIR(statbuf.st_mode))modestr[0] =  'd' ;
	         if (S_ISCHR(statbuf.st_mode))modestr[0] =  'c' ;
	         if (S_ISBLK(statbuf.st_mode))modestr[0] =  'b' ;
	 
	         if (statbuf.st_mode&S_IRUSR)modestr[1] =  'r' ;
	         if (statbuf.st_mode&S_IWUSR)modestr[2] =  'w' ;
	         if (statbuf.st_mode&S_IXUSR)modestr[3] =  'x' ;
	 
	         if (statbuf.st_mode&S_IRGRP)modestr[4] =  'r' ;
	         if (statbuf.st_mode&S_IWGRP)modestr[5] =  'w' ;
	         if (statbuf.st_mode&S_IXGRP)modestr[6] =  'x' ;
	 
	         if (statbuf.st_mode&S_IROTH)modestr[7] =  'r' ;
	         if (statbuf.st_mode&S_IWOTH)modestr[8] =  'w' ;
	         if (statbuf.st_mode&S_IXOTH)modestr[9] =  'x' ; 
            if(!strcmp(entry->d_name,".")||!strcmp(entry->d_name,".."))
               	 continue;
            //打印目录项的深度、目录名等信息
            printf("%s  filename:%s  last modefied time:%.12s\n",modestr,entry->d_name,4+ctime(&statbuf.st_atime));
            //递归调用printdir,打印子目录的信息,其中的depth+4;
            printdir(entry->d_name,depth+4);
        } 
	 	else{
	 		char modestr[11];
        	strcpy(modestr,"----------");
        	 if (S_ISDIR(statbuf.st_mode))modestr[0] =  'd' ;
	         if (S_ISCHR(statbuf.st_mode))modestr[0] =  'c' ;
	         if (S_ISBLK(statbuf.st_mode))modestr[0] =  'b' ;
	 
	         if (statbuf.st_mode&S_IRUSR)modestr[1] =  'r' ;
	         if (statbuf.st_mode&S_IWUSR)modestr[2] =  'w' ;
	         if (statbuf.st_mode&S_IXUSR)modestr[3] =  'x' ;
	 
	         if (statbuf.st_mode&S_IRGRP)modestr[4] =  'r' ;
	         if (statbuf.st_mode&S_IWGRP)modestr[5] =  'w' ;
	         if (statbuf.st_mode&S_IXGRP)modestr[6] =  'x' ;
	 
	         if (statbuf.st_mode&S_IROTH)modestr[7] =  'r' ;
	         if (statbuf.st_mode&S_IWOTH)modestr[8] =  'w' ;
	         if (statbuf.st_mode&S_IXOTH)modestr[9] =  'x' ; 
	 		printf("%s  filename:%s  last modefied time:%.12s\n",modestr,entry->d_name,4+ctime(&statbuf.st_atime));
        }
    }
    char a[100];
    strcpy(a,getcwd(NULL,NULL));
    int k=0;
    for(int i=0;i<strlen(a);i++){
    	if(a[i]=='/'){
    		k=i;
    	}
    }
    a[k]=0;
    if(chdir(a)==-1){
    	printf("chdir error!\n");
    	return;
    }   
    printf("\nreturn to directory: %s\n",getcwd(NULL,NULL));

    closedir(dp);//关闭目录项; 
}

int main(int arc,char* arg[]){
    printdir(arg[1],0);
    return 0;
}
