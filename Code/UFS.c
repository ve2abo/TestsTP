#include "UFS.h"
#include <stdio.h>
// Fonctions UTILITAIRE

int FindFirstFreeiNode(){
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	
	int iNodeNum;
	for (iNodeNum = 0; iNodeNum < 40; iNodeNum++) {
		if (iNodeFreeBitmap[iNodeNum] == 1) {  // libre == 1 ,  utilisé == 0

			//Devrait-on saisir le i-node immédiatement ?
			return iNodeNum;
		}
	}
	return -1;	//pas de i-node libre ?
}

int FindFirstFreeBlock(){
	char BlockFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);
	
	int BlockNum;
	for (BlockNum = 0; BlockNum < 40; BlockNum++) {
		if (BlockFreeBitmap[BlockNum] == 1) {  // libre == 1 ,  utilisé == 0
		
			//Devrait-on saisir le bloc immédiatement ?
			return BlockNum;
		}
	}
	return -1;	//pas de bloc libre ?
}

int IncrementNLink(UINT16 iNodeDiskBlockNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(iNodeDiskBlockNumber, BlockData);
	iNodeEntry*piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink++;
	WriteBlock(iNodeDiskBlockNumber, BlockData);
	return 1;
}

int DecrementNLink(UINT16 iNodeDiskBlockNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(iNodeDiskBlockNumber, BlockData);
	iNodeEntry*piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink--;
	WriteBlock(iNodeDiskBlockNumber, BlockData);
	return 1;
}

int SeizeFreeiNode(ino iNodeNum) {
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	iNodeFreeBitmap[iNodeNum] = 0;
	printf("IFTFS: Saisie i-node %d\n",iNodeNum);
	WriteBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	return 1;
}

int ReleaseFreeiNode(ino iNodeNum) {
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	iNodeFreeBitmap[iNodeNum] = 1;
	printf("IFTFS: Relache i-node %d\n",iNodeNum);
	WriteBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	return 1;
}

int SeizeFreeBlock(UINT16 BlockNum) {
	char BlockFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);
	BlockFreeBitmap[BlockNum] = 0;
	printf("IFTFS: Saisie bloc %d\n",BlockNum);
	WriteBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);
	return 1;
}

int ReleaseFreeBlock(UINT16 BlockNum) {
	char BlockFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);
	BlockFreeBitmap[BlockNum] = 1;
	printf("IFTFS: Relache bloc %d\n",BlockNum);
	WriteBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);
	return 1;
}

// Fonctions OBLIGATOIRE a implementer
int bd_hardlink(const char *pPathExistant, const char *pPathNouveauLien){
	return 1;
}

int bd_mv(const char *pFilename, const char *pFilenameDest){
	return 1;
}

int bd_mkdir(const char *pDirName){
	return 1;
}

int bd_create(const char *pFilename){
	return 1;
}

int bd_ls(const char *pDirLocation){
	return 1;
}

int bd_rm(const char *pFilename){
	return 1;
}

int bd_FormatDisk(){
	return 1;
}
