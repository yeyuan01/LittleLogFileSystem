#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "File.h"
int main(int argc, char*arg[]){
	printf("Test start!\n");  
	createDisk();
	printf("Disk vdisk created!\n");
	initRootDir();
	printf("root directory initalizated!\n");
	createFile("File1",1024);
	printf("File1 created!\n");
	writeFile("File1","I'm File1! I'm in root!");
	printf("Writed into File1!\nRead from File1: ");
	readFile("File1",23);

	
	createDir("Dir2");
	printf("Subdir Dir2 created!\n");
	changeDirectory("Dir2");
	printf("Change Directory, Current directory now is Dir2\n");
	
	createFile("File2",1024);
	printf("File2 created!\n");
	writeFile("File2","I'm File2!! I'm in Dir2!");
	printf("Writed into File2!\nRead from File2: ");
	readFile("File2",24);
	
	createDir("Dir3");
	printf("Subdir Dir3 created!\n");
	changeDirectory("Dir3");
	printf("Change Directory, Current directory now is Dir3\n");
	createFile("File3",25);
	printf("File1 created!\n");
	writeFile("File3","I'm File3!!! I'm in Dir3!");
	printf("Writed into File3!\nRead from File3: ");
	readFile("File3",25);
	
	fwrite(systemAddress,DISK_SIZE*sizeof(char),1,disk);
	free(systemAddress);    
	return 0;
}
