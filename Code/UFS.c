#include "UFS.h"
#include <stdio.h>
#include <string.h>

//Définition de constantes
#define MAX_PATH_SIZE 255



/******************************************************************************
*					SECTION DES FONCTIONS UTILITAIRES						  *
******************************************************************************/

// Prototypes des fonctions UTILITAIRES
UINT16 auxRechercheiNode(const char *pDirLocation, const char *DirCumulR, ino NumiNode);



/******************************************************************************
	Fonction : ino auxRechercheiNode(const char *pDirLocation, const char 
	           *DirCumulR, ino NumiNode)

	Description : fonction récursive auxiliaire de recherche de l'iNode à 
	              à partir d'un chemin d'accès.

	pDirLocation : chaîne de caractères contenant le chemin d'accès du fichier
                   recherché.
    DirCumulR : chemin d'accès courant de la recherche.
    NumiNode : numéro de l'iNode.

    Retourne le numéro de l'iNode ou 0 si le chemin d'accès est invalide.
******************************************************************************/
ino auxRechercheiNode(const char *pDirLocation, const char *DirCumulR, ino NumiNode){

	//Recherche au iNode courant.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(NumiNode, iNodeData);
	iNodeEntry *piNode = (iNodeEntry*) iNodeData;

 	//Leture du bloc de données du iNode courant.
	char DataBlockDirEntry[BLOCK_SIZE];
	ReadBlock(piNode->Block[0], DataBlockDirEntry);
	DirEntry *pDE = (DirEntry*) DataBlockDirEntry;

	int nbDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);

	int i;
	char sName[FILENAME_SIZE];
	ino iNode;

	//Parcourir le contenu du répertoire courant.
	for(i = 0; i < nbDirEntry; i++) {

		//Composition du chemin d'accès du fichier.
		strcpy(sName, DirCumulR);
		strncat(sName, pDE[i].Filename,FILENAME_SIZE);
//Debug
		printf("Chemin d'acces compose : %s\n", sName);
		
		//Fin d'itération si chemin d'accès équivalents.
		if (strcmp(sName,pDirLocation) == 0) {
//Debug
			printf("Chemin d'acces trouve : %s\n", sName);
			return pDE[i].iNode;
		}
		
		int iLongueur = strlen(sName);

		//Entre dans nouvelle itération si chemin d'accès cumulatifs compatibles.	
		if (strncmp(sName,pDirLocation,iLongueur) == 0 && pDirLocation[iLongueur+1] == '/') {
		
			//Ajoute '/' au chemin d'accès.
			strcat(sName,"/");

			//Appel et retourne le résultat de la fonction auxiliaire.
			return auxRechercheiNode(pDirLocation, sName, NumiNode);
			 //ca devrait pas être pDE[i].iNode au lieu de NumiNode
		}		
	}

	//Aucun fichier ne correspond au chemin d'accès.
	return 0;
}

/******************************************************************************
	Fonction : ino RechercheiNode(const char *pDirLocation)

	Description : trouve le numéro de iNode associé à un chemin d'accès.

	pDirLocation :	chaîne de caractères contenant le chemin d'accès du fichier
					recherché.

	Retourne le numéro de l'iNode ou 0 si le chemin d'accès est invalide.
******************************************************************************/
ino RechercheiNode(const char *pDirLocation){

	//Sortie si répertoire racine recherché.	
	if (strcmp(pDirLocation, "/") == 0) {
		return 1;
	}

	//Appel de la fonction auxiliaire.
	return auxRechercheiNode(pDirLocation, "/", BASE_BLOCK_INODE + ROOT_INODE);
}



/******************************************************************************
	Fonction : ino FindFirstFreeiNode()

	Description : fonction de recherche du premier iNode libre à partir du
	              bitmap d'iNode libre.

    Retourne le numéro de l'iNode ou 0 si aucun iNode disponible.
******************************************************************************/
ino FindFirstFreeiNode(){
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);

	int iNodeNum;
	for (iNodeNum = ROOT_INODE; iNodeNum < N_INODE_ON_DISK; iNodeNum++) {
		if (iNodeFreeBitmap[iNodeNum] == 1) {  // libre == 1 ,  utilisé == 0
			return iNodeNum;
		}
	}

	//Pas de iNode libre.
	printf("Erreur ! Aucun iNode libre.\n");
	return 0;
}



/******************************************************************************
	Fonction : ino FindFirstFreeBlock()

	Description : fonction de recherche du premier bloc libre à partir du
	              bitmap de bloc libre.

    Retourne le numéro de bloc ou 0 si aucun bloc disponible.
******************************************************************************/
UINT16 FindFirstFreeBlock(){
	char BlockFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_BLOCK_BITMAP, BlockFreeBitmap);

	int BlockNum;
	for (BlockNum = BASE_BLOCK_INODE + N_INODE_ON_DISK; BlockNum < N_BLOCK_ON_DISK; BlockNum++) {
		if (BlockFreeBitmap[BlockNum] == 1) {  // libre == 1 ,  utilisé == 0
			return BlockNum;
		}
	}
	
	//Pas de bloc libre.
	printf("Erreur ! Aucun bloc libre.\n");
	return 0;
}

int AddDirEntry(ino iNodeDestinationDir, const char *sNameNewEntry, ino iNodeNewEntry) {
	//Lecture de l'iNode
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeDestinationDir, iNodeData);
	iNodeEntry *piNode= (iNodeEntry*)iNodeData;

	//Déterminer le nombre d'entrée de répertoire (ou "DirEntry")
	int nextDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);

 	//Leture du bloc de données du répertoire.
	char DirEntryBlockData[BLOCK_SIZE];
	ReadBlock(piNode->Block[0], DirEntryBlockData);
	DirEntry *pDirEntry = (DirEntry *)DirEntryBlockData;

	//Ajout du nouvel entré de répertoire à la fin 
	strcpy(pDirEntry[nextDirEntry].Filename,sNameNewEntry);
	pDirEntry[nextDirEntry].iNode = iNodeNewEntry;	

	//Écriture du bloc de donnée du répertoire
	WriteBlock(piNode->Block[0], DirEntryBlockData);

	//Ajustement de la valeur "size" du répertoire et écriture de l'iNode modifié
	piNode->iNodeStat.st_size +=  sizeof(DirEntry);
	WriteBlock(BASE_BLOCK_INODE + iNodeDestinationDir, iNodeData);
	return 1;
}

int RemoveDirEntry(ino iNodeDestinationDir, ino iNodeEntryToRemove) {
//Debug
	printf("no iNode du répertoire : %d.\n",iNodeDestinationDir);
	
	//Lecture de l'iNode
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeDestinationDir, iNodeData);
	iNodeEntry *piNode= (iNodeEntry*)iNodeData;

	//Déterminer le nombre d'entrée de répertoire (ou "DirEntry")
	int nbDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);


 	//Leture du bloc de données du répertoire.
	char DirEntryBlockData[BLOCK_SIZE];
	ReadBlock(piNode->Block[0], DirEntryBlockData);
	DirEntry *pDirEntry = (DirEntry *)DirEntryBlockData;

//Debug
	printf("no iNode à retirer : %d.\n",iNodeEntryToRemove);

	//Recherche de l'entré de répertoire
	int i = 0;
	while  ( (i < nbDirEntry)  && (pDirEntry[i].iNode != iNodeEntryToRemove) ) {
		printf("iNode existant dans répertoire : %d.\n",pDirEntry[i].iNode);
		i++;
	}

	//Si on trouve l'entrée de répertoire.
	if (pDirEntry[i].iNode == iNodeEntryToRemove) {
		//Si l'entré de répertoire n'est pas la dernière ...
		if (i != nbDirEntry-1) { 
			// ... alors il faut copier la dernier à la place ...
			strcpy(pDirEntry[i].Filename,pDirEntry[nbDirEntry-1].Filename);
			pDirEntry[i].iNode = pDirEntry[nbDirEntry-1].iNode;	
		}
		
		// ... et vider les champ de la dernière entrée.
		strcpy(pDirEntry[nbDirEntry-1].Filename, "");
		pDirEntry[nbDirEntry-1].iNode = 0;
	} else { //Si on ne trouve pas l'entrée de répertoire.
		printf("Erreur ! Entrée de répertoire non-trouvé.\n");
		return 0;
	}

	//Écriture du bloc de donnée du répertoire
	WriteBlock(piNode->Block[0], DirEntryBlockData);

	//Ajustement de la valeur "size" du répertoire et écriture de l'iNode modifié
	piNode->iNodeStat.st_size -=  sizeof(DirEntry);
	WriteBlock(BASE_BLOCK_INODE + iNodeDestinationDir, iNodeData);
	return 1;
}


int IncrementNLink(UINT16 iNodeDiskBlockNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(iNodeDiskBlockNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink++;
	WriteBlock(iNodeDiskBlockNumber, BlockData);
	return 1;
}

int DecrementNLink(UINT16 iNodeDiskBlockNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(iNodeDiskBlockNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
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
	//Recherche du iNode lié à pFilename
	ino iNodeSource = RechercheiNode(pFilename);

	//Est-ce que la source existe ?
	if (iNodeSource == 0) { //iNodeSourceFile sera à 0 si pFilename n'existe pas
		//La source à déplacer n'existe pas
		printf("Erreur ! '%s' n\'existe pas.\n", pFilename);
		return 0;
	}

	//Recherche du iNode lié à pFilenameDest
	ino iNodeDest = RechercheiNode(pFilenameDest);

	// Est-ce que le nom de destination est libre ?
	if (iNodeDest != 0) { //iNodeDestFile ne sera pas à 0 si pFilenameDest existe
		//Le nom de destination existe déjà
		printf("Erreur ! Le nom '%s' existe déjà.\n", pFilenameDest);
		return 0;
	}

	//Lecture de l'iNode de la source
	char iNodeSrcData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeSource, iNodeSrcData);
	iNodeEntry *piNodeSource= (iNodeEntry*)iNodeSrcData;

	// Est-ce que la source est bien un fichier ?
	if (piNodeSource->iNodeStat.st_mode!=S_IFREG) { //On vérifie que l'on termine bien sur un fichier
		//Le nom fourni n'est pas un fichier
		printf("Erreur ! '%s' n\'est pas un fichier.\n", pFilename);
		return 0;		
	}

	//Extraire le nom du fichier et le chemin depuis pFilenameDest
	char *sNewFileName;
	char sDestPath[MAX_PATH_SIZE]="";

	sNewFileName = strrchr(pFilenameDest, '/');
	strncpy(sDestPath, pFilenameDest, sNewFileName-pFilenameDest);
	sNewFileName++;

	//Recherche du iNode lié à sDestPath
	iNodeDest = RechercheiNode(sDestPath);

	// Est-ce que le chemin de destination existe ?
	if (iNodeDest == 0) { //iNodeDest sera à 0 si sDestPath n'existe pas
		//Le chemin fourni n'existe pas
		printf("Erreur ! Le chemin '%s' n\'existe pas.\n", sDestPath);
		return 0;		
	}

	if (AddDirEntry(iNodeDest, sNewFileName, iNodeSource) == 0) {
		//Erreur lors de l'ajout de l'entrée de répertoire
		printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
		return 0;		
	}

	//Extraire le nom du fichier et le chemin depuis pFilename
	char *sOldFileName;
	char sSourcePath[MAX_PATH_SIZE]="";

	sOldFileName = strrchr(pFilename, '/');
	if (sOldFileName == pFilename) {sOldFileName++;}
	strncpy(sSourcePath, pFilename, sOldFileName-pFilename);

	//Recherche du iNode lié à sSourcePath
	ino iNodeDirSource = RechercheiNode(sSourcePath);

	if (RemoveDirEntry(iNodeDirSource, iNodeSource) == 0) {
		//Erreur lors du retrait de l'entrée de répertoire
		printf("Erreur lors du retrait de l'entrée de répertoire.\n");
		return 0;		
	}

	printf("Déplacement / renommage du fichier '%s' vers '%s'\n", pFilename, pFilenameDest);
	return 1;
}

int bd_mkdir(const char *pDirName){
	//Recherche du iNode lié à pDirLocation.
	ino iNodeDir = RechercheiNode(pDirName);

	if (iNodeDir == 0) { //iNodeDir sera à 0 si pDirName n'existe pas
		char *sNewDirName;
		char sExistingPath[MAX_PATH_SIZE]="";

		sNewDirName = strrchr(pDirName, '/');
		strncpy(sExistingPath, pDirName, sNewDirName-pDirName);
		sNewDirName++;

		iNodeDir = RechercheiNode(sExistingPath);

		if  (iNodeDir != 0) { //iNodeDir ne sera pas à 0 si sExistingPath existe
			char iNodeData[BLOCK_SIZE];
			ReadBlock(BASE_BLOCK_INODE + iNodeDir, iNodeData);
			iNodeEntry *piNode= (iNodeEntry*)iNodeData;

			if (piNode->iNodeStat.st_mode==S_IFDIR) { //On vérifie que l'on termine bien sur un répertoire
				printf("Création du répertoire %s\n",sNewDirName);

				//Appel à la fonction utilitaire
	

				return 1;
			}
			//Chemin d'accès n'est pas un répertoire
			printf("Erreur ! Le chemin d\'acces '%s' n\'est pas un répertoire.\n", sExistingPath);
			return 0;
		}
		//Chemin d'accès inexistant ! 
		printf("Erreur ! Le chemin d\'acces '%s' n\'existe pas.\n", sExistingPath);
		return 0;
	}
	//Répertoire ou fichier du même nom déjà existant
	printf("Erreur ! Le nom '%s' existe déjà.\n", pDirName);
	return 0;
}

int bd_create(const char *pFilename){
	return 1;
}

int bd_ls(const char *pDirLocation){
	
	//Recherche du iNode lié à pDirLocation.
	ino iNodeDirLocation = RechercheiNode(pDirLocation);

	if (iNodeDirLocation != 0) {
		//Lecture du iNode racine.
		char iNodeData[BLOCK_SIZE];
		ReadBlock(BASE_BLOCK_INODE + iNodeDirLocation, iNodeData);
		iNodeEntry *piNode= (iNodeEntry*)iNodeData;

		int nbDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);
	  
	 	//Leture du bloc de données liées à la racine.
		char DataBlockDirEntry[BLOCK_SIZE];
		ReadBlock(piNode->Block[0], DataBlockDirEntry);
		DirEntry *pDE = (DirEntry *)DataBlockDirEntry;

		printf("ls %s \n",pDirLocation);

		int i;
		char sName[FILENAME_SIZE];
		char cType;
		ino iNode;
		UINT16 sSize;
		UINT16 iMode;
		UINT16 iNlink;

		//Affichage du contenu du répertoire cible.
		for(i=0; i<nbDirEntry; i++) {
			strncpy(sName, pDE[i].Filename,FILENAME_SIZE);
			iNode = pDE[i].iNode;

			char iNodeBlockDataEntry[BLOCK_SIZE];
			ReadBlock(BASE_BLOCK_INODE + iNode, iNodeBlockDataEntry);
			iNodeEntry *piNodeEntry= (iNodeEntry*)iNodeBlockDataEntry;
			sSize = piNodeEntry->iNodeStat.st_size;
			iMode = piNodeEntry->iNodeStat.st_mode;
			iNlink = piNodeEntry->iNodeStat.st_nlink;

			switch (iMode) {
					case S_IFREG : 
						cType = '-';
						break;
					case S_IFDIR : 
						cType = 'd';
						break;
					default :
						cType = '?';
			}

			printf("%c %14s size: %6d inode: %3d nlink: %3d\n", cType, sName, sSize, iNode, iNlink);
		}

		return 1;
	}
	//As-t-on besoin d'un message d'erreur ??
	return 0;
}

int bd_rm(const char *pFilename){

	printf("iNode : %i\n", RechercheiNode(pFilename));

	return 1;
}

int bd_FormatDisk(){

	char Bitmap[BLOCK_SIZE]="";
	int i = 0;

	//Mise à zéro de tous les blocs
	
	
	


	//Initialisation de la liste des blocs de données.
	for (i = 0; i < BLOCK_SIZE; i++) {
		if (i<=43)
			Bitmap[i] = 0;
		else
			Bitmap[i] = 1;
	}

	//Écriture de la table des blocs libres.

	WriteBlock(FREE_BLOCK_BITMAP, Bitmap);


	//Initialisation de la liste des iNodes.

	for (i = 0; i < BLOCK_SIZE; i++) {
		if (i<40)
			Bitmap[i] = 1;	//1=libre
		else
			Bitmap[i] = 0;	//0=occupé
	}

	Bitmap[0] = 0;


	//Écriture de la table des iNodes.

	WriteBlock(FREE_INODE_BITMAP, Bitmap);

	//Création du iNode du répertoire racine.
	char iNodeData[BLOCK_SIZE]="";
	iNodeEntry *piNode = (iNodeEntry*)iNodeData;
	piNode->iNodeStat.st_ino = FindFirstFreeiNode();
	piNode->iNodeStat.st_mode = S_IFDIR;
	piNode->iNodeStat.st_nlink = 0;
	piNode->iNodeStat.st_size = 32;
	piNode->iNodeStat.st_blocks = 1;
	SeizeFreeiNode(piNode->iNodeStat.st_ino);
	piNode->Block[0] = FindFirstFreeBlock();
	SeizeFreeBlock(piNode->Block[0]);
	WriteBlock(BASE_BLOCK_INODE + ROOT_INODE, iNodeData);

	//Déclaration du bloc de données.	
	char BlockDirEntry[BLOCK_SIZE]="";
	DirEntry *pDirEntry = (DirEntry *)BlockDirEntry;

	//Création dirEntry '.'.
	strcpy(pDirEntry[0].Filename,".");
	pDirEntry[0].iNode = piNode->iNodeStat.st_ino;	
	IncrementNLink(piNode->Block[0]);	
	
	//Création dirEntry '..'.
	strcpy(pDirEntry[1].Filename,"..");
	pDirEntry[1].iNode = piNode->iNodeStat.st_ino;
	IncrementNLink(piNode->Block[0]);

	//Écriture du bloc de données.
	WriteBlock(piNode->Block[0], BlockDirEntry);


	return 1;

}
