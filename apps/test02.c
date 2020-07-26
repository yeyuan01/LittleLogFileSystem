#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "File.h"
int main(int argc, char*arg[]){
	printf("Test02 start!\n");  
	disk = fopen("vdisk", "wb");
	printf("vdisk is opened\n");  
	systemAddress = (u_int8_t *)calloc(DISK_SIZE, sizeof(char));
	printf("systemAddress is calloced\n"); 
	struct Block *blk = (struct Block*)systemAddress;
	struct superBlock *sb = (struct superBlock*)blk;
	printf("superBlock allocated\n"); 
	sb->magic_num = MAGIC_NUMBER;
	sb->numberOfBlocks = NUM_BLOCKS;
	sb->numberOfInodes = 0;
	printf("superBlock initialized, Magic# is %d\n",MAGIC_NUMBER);
	blk += 1;

	struct freeBlockVector *fbv = (struct freeBlockVector*)blk;
	
	fbv->freeBlock[1] = 63;
	int i;
	for(i = 2; i < BLOCK_SIZE; i++){
		fbv->freeBlock[i] = 255;
	}
	printf("freeBlock initialized\n");
	fwrite(systemAddress,DISK_SIZE*sizeof(char),1,disk);
	printf("data written to vdisk\n");
	free(systemAddress);  
	printf("free address\n");
	return 0;
}
