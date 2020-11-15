#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

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
    printf("current working directory: %s\n",getcwd(NULL,NULL));
    while((entry=readdir(dp))!=NULL){
    	lstat(entry->d_name,&statbuf);   
        if(S_ISDIR(statbuf.st_mode)){
            if(!strcmp(entry->d_name,".")||!strcmp(entry->d_name,".."))
               	 continue;
            //打印目录项的深度、目录名等信息
            printf("%d %s %lu\n",depth,entry->d_name,statbuf.st_atime);
            //递归调用printdir,打印子目录的信息,其中的depth+4;
            printdir(entry->d_name,depth+4);
        } 
	 	else{
	 		printf("%d %s %lu\n",depth,entry->d_name,statbuf.st_atime);
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
    printf("return to directory: %s\n",getcwd(NULL,NULL));

    closedir(dp);//关闭目录项; 
}

int main(int arc,char* arg[]){
    printdir(arg[1],0);
    return 0;
}
