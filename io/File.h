#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAGIC_NUMBER 0x53EF
#define DISK_SIZE NUM_BLOCKS*BLOCK_SIZE
#define NUM_BLOCKS 4096
#define BLOCK_SIZE 512
#define INODE_SIZE 32

typedef unsigned char   BYTE; 
typedef unsigned short  WORD;

//int NUM_INODES = 0;

typedef struct superBlock{
	u_int32_t magic_num;
	u_int32_t numberOfBlocks;
	u_int32_t numberOfInodes;
}superBlock;

typedef struct freeBlockVector{
	u_int8_t freeBlock[BLOCK_SIZE];
}freeBlockVector;

typedef struct Block{
	u_int8_t block[BLOCK_SIZE];
}Block;

typedef struct inode{
	u_int32_t sizeOfFile;
	u_int32_t flags;
	u_int16_t blocks[10];
	u_int16_t single_indirectBlock;
	u_int16_t double_indirectBlock;
}inode;

typedef struct entry{
	u_int8_t inodeMap;
	u_int8_t fileName[31];
}entry;

typedef struct directory{
	struct entry map[16];
}directory;
extern FILE *disk;
extern struct directory* root;
extern struct directory* cur;
extern char path[256];
extern u_int8_t *systemAddress;
extern u_int16_t blockNumForDir;
void reverseUsability(int Num);
u_int8_t *getblockAddr(u_int16_t block_num);
u_int8_t findFreeEntry(struct directory *dir);
u_int8_t findEntry(struct directory *dir, char* fileName);
int add_todir(char *fileName, struct directory *dir, u_int8_t inodeNum);
int createFile(char *fileName,u_int16_t size);
int checkUsability(int blockNum);
u_int32_t allocateBlock();
struct inode *getInodeAddr(u_int8_t inodeIndex);
inode* allocateInode();
u_int8_t getInodeNum(struct inode *addr);
void createInode(struct inode *Inode, u_int16_t block[],u_int16_t size,u_int32_t flag,int blockN);
void initRootDir();
void createDisk();
int writeFile(char* fileName,char* data);
void readFile(char* fileName, int len);
void createDir(char* dirName);
void changeDirectory(char* dirName);
