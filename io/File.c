#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "File.h"

FILE *disk;
struct directory* root;
struct directory* cur;
char path[256] = {};
u_int8_t *systemAddress;
u_int16_t blockNumForDir = 2;
u_int8_t *getblockAddr(u_int16_t block_num){
	return systemAddress + block_num*BLOCK_SIZE;
}
u_int8_t findFreeEntry(struct directory *dir){
	int i;	
	for(i = 0;i < 16;i++){
		if(dir->map[i].inodeMap == 0){
			return i;
		}
	}
	return -1;
}
u_int8_t findEntry(struct directory *dir,char* fileName){
	int i;
	for(i=0; i < 16; i++){
		if(!strncmp(dir->map[i].fileName, fileName, strlen(fileName))){
			return i;
		}
	}
	return -1;
}
int addToDir(char *fileName, struct directory *dir, u_int8_t inodeNum){
    u_int8_t freeEntry = findFreeEntry(dir);
    if(freeEntry == -1){
        perror("entry is full");
        return -1;
    }
    dir->map[freeEntry].inodeMap = inodeNum;
    strncpy(dir->map[freeEntry].fileName, fileName, 32);

    return 0;
}
int createFile(char *fileName,u_int16_t size){
	if(strlen(fileName) > 30){
		perror("file name should be less than 31");
		return -1;	
	}
	struct inode *Inode = allocateInode();
	if(Inode == NULL){
		return -1;
	}
	int numberOfBlocks,i;
	if(size%512 == 0){
		numberOfBlocks = size/512;
	}else{
		numberOfBlocks = (size/512) + 1;
	}
	u_int16_t Blocks[numberOfBlocks];
	for(i = 0;i < numberOfBlocks;i++){
		Blocks[i] = allocateBlock();

	}
	createInode(Inode,Blocks,size,0,i);
	u_int8_t inodeNum = getInodeNum(Inode);
	addToDir(fileName,cur,inodeNum);
	return 0;
}
void createDir(char* dirName){
	blockNumForDir++;
	struct inode *Inode = allocateInode();
	u_int16_t block[1];
	block[0] = blockNumForDir;
	createInode(Inode,block,1,1,1);
	u_int8_t inodeNum = getInodeNum(Inode);
	addToDir(dirName,cur,inodeNum);
}
void changeDirectory(char* dirName){
	u_int8_t dirEntry = findEntry(cur,dirName);
	if(dirEntry == -1){
		perror("No such dir.");
		return;
	}
	u_int8_t dirInodeNum = cur->map[dirEntry].inodeMap;
	struct inode *Inode = getInodeAddr(dirInodeNum);
	struct directory *dir = (struct directory *)getblockAddr(Inode->blocks[0]);
	cur = dir;
}
/*
	the following writeFile can write in noncontinuous blocks but have some bugs
*/
/*
int writeFile(char* fileName,char* data){
	u_int8_t fileEntry = findEntry(cur,fileName);
	if(fileEntry == -1){
		perror("file not found");
		return -1;
	}
	u_int8_t nblock = strlen(data) / 512;
	u_int8_t nbyte = strlen(data) % 512;
	u_int8_t i,j;
	u_int8_t fileBlocksMove[nblock];

	u_int8_t inodeNum = cur->map[fileEntry].inodeMap;
	struct inode *Inode = getInodeAddr(inodeNum);
	u_int16_t size = strlen(data);
	//fileBlocksMove[] is the number of blocks need to move
	//
	//				ie. Inode->blocks[0] = 20; Inode->blocks[1] = 30;
	//				fileBlocksMove[0] = 0    
	//				fileBlocksMove[1] = Inode->blocks[1]-Inode->blocks[0](20) = 10
	//				when first get address,fblock = 20; 
	//				fblock+fileBlocksMove[1] = 30  which is the block# of the second block for the file
	//
	fileBlocksMove[0] = 0;
	for(i = 1;i < nblock; i++){
		fileBlocksMove[i] = Inode->blocks[i] - Inode->blocks[i-1];
		printf("***%d***",fileBlocksMove[i]);
	}
	struct Block *fBlock = (struct Block *)getblockAddr(Inode->blocks[0]);


	for(j = 0; j < nblock; j++){
		
		memcpy((void *)(fBlock+fileBlocksMove[j]), (void *)(data+j*512), 512);
	}
	memcpy((void *)(fBlock+fileBlocksMove[j]), (void *)(data+j*512), nbyte);
	return 0;
}*/
//only able to write in continuous blocks
int writeFile(char* fileName,char* data){
	u_int8_t fileEntry = findEntry(cur,fileName);
	if(fileEntry == -1){
		perror("file not found");
		return -1;
	}
	u_int8_t inodeNum = cur->map[fileEntry].inodeMap;
	struct inode *Inode = getInodeAddr(inodeNum);
	u_int16_t size = strlen(data);
	u_int16_t fileBlock = Inode->blocks[0];
	struct Block *fBlock = (struct Block *)getblockAddr(fileBlock);
//	printf("%d\n",fileBlock);
	u_int8_t i;
	u_int8_t nblock = strlen(data) / 512;
	u_int8_t nbyte = strlen(data) % 512;
	for(i=0; i<nblock; i++){
		memcpy((void *)(fBlock+i), (void *)(data+i*512), 512);
	}
	memcpy((void *)(fBlock+i), (void *)(data+i*512), nbyte);
	//writeBlock(disk,fileBlock,data,size);
	return 0;
}
void readFile(char* fileName, int len){
    u_int8_t fileEntry = findEntry(cur, fileName);
    if(fileEntry == -1){
        perror("file not found");
        return;
    }
    u_int8_t nodeNum = cur->map[fileEntry].inodeMap;
    struct inode *Inode = getInodeAddr(nodeNum);

    if(len > Inode->sizeOfFile*512){
        perror("length too long");
        return;
    }

    u_int16_t fileBlock = Inode->blocks[0];
    struct Block *fBlock = (struct Block *)getblockAddr(fileBlock);
	int i;
    for(i=0; i<len; ++i){
        printf("%c", *((char *)fBlock+i));
    }
    printf("\n");

}
//blockNum is the block # you want to check 
//1 means usable
//0 means not
int checkUsability(int blockNum){
	int byteNumInVB;
	int bitNumInByte = blockNum%8;
	if(bitNumInByte == 0){
	bitNumInByte = 8;
	byteNumInVB = (blockNum/8)-1;		
	}else{
	byteNumInVB = blockNum/8;	
	}

	struct freeBlockVector *fbv = (struct freeBlockVector*)getblockAddr(1);
	if(((0b00000001 << (8-bitNumInByte)) & fbv->freeBlock[byteNumInVB]) != 0){
		return 1;	
	}else {
		return 0;	
	}
}
u_int32_t allocateBlock(){
	int i;
	u_int32_t blockN = 0;
	struct freeBlockVector *fbv = (struct freeBlockVector*)getblockAddr(1);
	for(i = 0;i < NUM_BLOCKS;i++){
		if(checkUsability(i) == 1){
			u_int32_t blockN = i;
			reverseUsability(i);
			return blockN;	
		}
	}
}
void reverseUsability(int blockNum){
	int byteNumInVB;
	int bitNumInByte = blockNum%8;
	if(bitNumInByte == 0){
	bitNumInByte = 8;
	byteNumInVB = (blockNum/8)-1;		
	}else{
	byteNumInVB = blockNum/8;	
	}
	struct freeBlockVector *fbv = (struct freeBlockVector*)getblockAddr(1);
	int flag = 0b11111110;
	int i;	
	for(i = 0;i < 8 - bitNumInByte;i++){
		flag = (flag << 1) + 1;
	}
	if(checkUsability(blockNum) == 1){
	fbv->freeBlock[byteNumInVB] = fbv->freeBlock[byteNumInVB] & flag;
	} else {
	fbv->freeBlock[byteNumInVB] = fbv->freeBlock[byteNumInVB] | (0b000000001 << 8-bitNumInByte);
	}

}
struct inode *getInodeAddr(u_int8_t inodeIndex){
	struct Block *inodeMap = (struct Block *)getblockAddr(6);
	struct inode *inodeAddr = (struct inode *)(&inodeMap[inodeIndex/16]) + (inodeIndex%16);
	return inodeAddr;
}
inode* allocateInode(){
	
	struct superBlock *sb = (struct superBlock*)getblockAddr(0);
	struct Block *inodeMap = (struct Block *)getblockAddr(6);
	struct inode *newInode;
	if(sb->numberOfInodes == 4095){
		perror("inode full");
		return NULL;
	}
	
	newInode = getInodeAddr(sb->numberOfInodes);
	sb->numberOfInodes++;
	return newInode;
}
u_int8_t getInodeNum(struct inode *addr){
	struct Block *inodeMap = (struct Block *)getblockAddr(6);
	u_int16_t inode = addr - (struct inode *)inodeMap;
	return inode;

}
void createInode(struct inode *Inode, u_int16_t block[],u_int16_t size,u_int32_t flag,int blockN){

	u_int16_t i;
	for(i = 0;i < blockN;i++){
		Inode->blocks[i] = block[i];
	}
	Inode->sizeOfFile = size;
	Inode->flags = flag;
	Inode->double_indirectBlock = 0;
	Inode->single_indirectBlock = 0;

}

void initRootDir(){
	struct inode *Inode = allocateInode();
	
	root = (struct directory *)getblockAddr(2);
	cur = root;
	u_int16_t block[1];
	block[0] = blockNumForDir;
	createInode(Inode,block,1,1,1);
	path[0] = '\\';
	path[1] = '\0';
}
void createDisk(){
	disk = fopen("vdisk", "wb");
	systemAddress = (u_int8_t *)calloc(DISK_SIZE, sizeof(char));
	struct Block *blk = (struct Block*)systemAddress;
	struct superBlock *sb = (struct superBlock*)blk;
	sb->magic_num = MAGIC_NUMBER;
	sb->numberOfBlocks = NUM_BLOCKS;
	sb->numberOfInodes = 0;
	blk += 1;

	struct freeBlockVector *fbv = (struct freeBlockVector*)blk;
	fbv->freeBlock[1] = 63;
	int i;
	for(i = 2; i < BLOCK_SIZE; i++){
		fbv->freeBlock[i] = 255;
	}
	//if want to modify, use btiwise operation,(maybe, not sure)
	//ex. want to earse the 8th bits in a byte
	//fbv->freeBlock[4] = fbv->freeBlock[4] & 0xfe;	
}
// assumption of find whether certion block in vector block is 1 or 0
// suppose want to check block 1000
// 1000/8 = the byte in vector block
// then 
// 1000%8 = which bit in the byte
// then use 00000001 << i(here i = 8-1000%8, maybe) and then bitwise the 
// result with fbv->freeBlock[1000/8]

