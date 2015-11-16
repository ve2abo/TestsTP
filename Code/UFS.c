#include "UFS.h"
#include <stdio.h>
#include <string.h>

//Définition de constantes
#define MAX_PATH_SIZE 255



/*******************************************************************************
*					SECTION DES FONCTIONS UTILITAIRES						   *
*******************************************************************************/

// Prototypes des fonctions UTILITAIRES
UINT16 auxRechercheiNode(const char *pDirLocation, const char *DirCumulR, ino iNodeNumPrevDir);



/*******************************************************************************
	Fonction : ino auxRechercheiNode(const char *pDirLocation, const char 
	           *DirCumulR, ino iNodeNumPrevDir)

	Description : fonction récursive auxiliaire de recherche de l'iNode à 
	              à partir d'un chemin d'accès.

	pDirLocation : chaîne de caractères contenant le chemin d'accès du fichier
                   recherché.
    DirCumulR : chemin d'accès courant de la recherche.
    iNodeNumPrevDir : numéro de l'iNode.

    Retourne le numéro de l'iNode ou 0 si le chemin d'accès est invalide.
*******************************************************************************/
ino auxRechercheiNode(const char *pDirLocation, const char *DirCumulR, ino iNodeNumPrevDir) {

	//Recherche du iNode courant.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumPrevDir, iNodeData);
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
//		printf("Chemin d'acces compose : %s\n", sName);
		
		//Fin d'itération si chemin d'accès équivalents.
		if (strcmp(sName,pDirLocation) == 0) {
//Debug
//			printf("Chemin d'acces trouve : %s\n", sName);
			return pDE[i].iNode;
		}
		
		int iLongueur = strlen(sName);

//Debug
//		printf("Debug : %d %c\n", strncmp(sName,pDirLocation,iLongueur),pDirLocation[iLongueur]);
		//Entre dans nouvelle itération si chemin d'accès cumulatifs compatibles.	
//Debug: retrait du +1 dans pDirLocation[iLongueur+1]
		if (strncmp(sName,pDirLocation,iLongueur) == 0 && pDirLocation[iLongueur] == '/') {
		
			//Ajoute '/' au chemin d'accès.
			strcat(sName,"/");

//Debug
//		printf("Appel récursif avec sName= %s et iNode = %d\n", sName, pDE[i].iNode);
			//Appel et retourne le résultat de la fonction auxiliaire.
			return auxRechercheiNode(pDirLocation, sName, pDE[i].iNode);
		}		
	}

	//Aucun fichier ne correspond au chemin d'accès.
	return 0;
}


/*******************************************************************************
	Fonction : ino RechercheiNode(const char *pDirLocation)

	Description : trouve le numéro de iNode associé à un chemin d'accès.

	pDirLocation :	chaîne de caractères contenant le chemin d'accès du fichier
					recherché.

	Retourne le numéro de l'iNode ou 0 si le chemin d'accès est invalide.
*******************************************************************************/
ino RechercheiNode(const char *pDirLocation) {

	//Sortie si répertoire racine recherché.	
	if (strcmp(pDirLocation, "/") == 0) {
		return 1;
	}

//Debug : retrait de BASE_BLOCK_INODE + dans return auxRechercheiNode(pDirLocation, "/", BASE_BLOCK_INODE + ROOT_INODE);


	//Appel de la fonction auxiliaire.
	return auxRechercheiNode(pDirLocation, "/", ROOT_INODE);
}


/*******************************************************************************
	Fonction : ino FindFirstFreeiNode()

	Description : fonction de recherche du premier iNode libre à partir du
	              bitmap d'iNode libre.

    Retourne le numéro de l'iNode ou 0 si aucun iNode disponible.
*******************************************************************************/
ino FindFirstFreeiNode() {
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


/*******************************************************************************
	Fonction : ino FindFirstFreeBlock()

	Description : fonction de recherche du premier bloc libre à partir du
	              bitmap de bloc libre.

    Retourne le numéro de bloc ou 0 si aucun bloc disponible.
*******************************************************************************/
UINT16 FindFirstFreeBlock() {
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

/*******************************************************************************
	Fonction : int AddDirEntry(ino iNodeDestinationDir, 
							   const char *sNameNewEntry, ino iNodeNewEntry)

	Description : Ajoute une entrée à un répertoire.

	iNodeDestinationDir :	iNode du répertoire à modifier.
	sNameNewEntry :			nom du répertoire ou fichier à ajouter.
	iNodeNewEntry :			iNode du répertoire ou fichier à ajouter.

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
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

//debug
//	printf("Debug : %s.\n",sNameNewEntry);

	//Ajout du nouvel entré de répertoire à la fin 
	strcpy(pDirEntry[nextDirEntry].Filename,sNameNewEntry);
	pDirEntry[nextDirEntry].iNode = iNodeNewEntry;	

	//Écriture du bloc de donnée du répertoire
	WriteBlock(piNode->Block[0], DirEntryBlockData);

	//Ajustement de la valeur "size" du répertoire 
	piNode->iNodeStat.st_size +=  sizeof(DirEntry);
	
	//Écriture de l'iNode modifié
	WriteBlock(BASE_BLOCK_INODE + iNodeDestinationDir, iNodeData);
	return 1;
}

/*******************************************************************************
	Fonction : int RemoveDirEntry(ino iNodeDestinationDir, 
								  ino iNodeEntryToRemove)

	Description : Retire une entrée à un répertoire.

	iNodeDestinationDir :	iNode du répertoire à modifier.
	iNodeNewEntry :			iNode du répertoire ou fichier à retirer.

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int RemoveDirEntry(ino iNodeDestinationDir, ino iNodeEntryToRemove) {
//Debug
//	printf("no iNode du répertoire : %d.\n",iNodeDestinationDir);
	
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
//	printf("no iNode à retirer : %d.\n",iNodeEntryToRemove);

	//Recherche de l'entré de répertoire
	int i = 0;
	while  ( (i < nbDirEntry)  && (pDirEntry[i].iNode != iNodeEntryToRemove) ) {
//Debug
//		printf("iNode existant dans répertoire : %d.\n",pDirEntry[i].iNode);
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


int IncrementNLink(ino iNodeNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink++;
	WriteBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	return 1;
}

int DecrementNLink(ino iNodeNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink--;
	WriteBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
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

/*******************************************************************************
	Fonction : int CreateFile(UINT16 iMode, ino iNodeDestinationDir, 
							  char *sNewName)

	Description : Crée un nouveau fichier de type régulier ou répertoire.

	iMode :					Mode du fichier (S_IFREG ou S_IFDIR)
	iNodeDestinationDir :	iNode du répertoire à modifier.
	iNodeNewEntry :			iNode du répertoire ou fichier à retirer.

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int CreateFile(UINT16 iMode, ino iNodeDestinationDir, char *sNewName) {
	ino iNewiNode = FindFirstFreeiNode();		   //Recherche d'un iNode libre.

	//Est-ce qu'il y a un iNode libre?
	if (iNewiNode == 0) {
		//Plus de iNode libre.
		printf("Erreur ! Il n'y a plus de iNode libre.\n");
		return 0;
	}

	UINT16 iNewBlock = FindFirstFreeBlock();		//Recherche d'un bloc libre.
	
	//Est-ce qu'il y a un bloc libre?
	if (iNewBlock == 0) {
		//Plus de bloc libre.
		printf("Erreur ! Il n'y a plus de bloc de données libre.\n");
		return 0;
	}
	
	SeizeFreeiNode(iNewiNode);					 //Réservation du nouveau iNode.
	SeizeFreeBlock(iNewBlock);					  //Réservation du nouveau bloc.

	//Création de l'iNode
	char NewiNodeData[BLOCK_SIZE]="";
	iNodeEntry *pNewiNode = (iNodeEntry*)NewiNodeData;
	pNewiNode->Block[0] = iNewBlock;

	pNewiNode->iNodeStat.st_ino = iNewiNode;
	pNewiNode->iNodeStat.st_mode = iMode;
	pNewiNode->iNodeStat.st_nlink = 0;
	pNewiNode->iNodeStat.st_size = 0;
	pNewiNode->iNodeStat.st_blocks = 1;

	WriteBlock(BASE_BLOCK_INODE + iNewiNode, NewiNodeData);

	//Si le nouveau fichier est de type répertoire
	if (iMode == S_IFDIR) {
		//Ajout de l'entrée de répertoire '.'
		if (AddDirEntry(iNewiNode, ".", iNewiNode) == 0) {
			//Erreur lors de l'ajout de l'entrée de répertoire
			printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
			return 0;		
		}
		IncrementNLink(iNewiNode);

		//Ajout de l'entrée de répertoire '..'
		if (AddDirEntry(iNewiNode, "..", iNodeDestinationDir) == 0) {
			//Erreur lors de l'ajout de l'entrée de répertoire
			printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
			return 0;		
		}
		IncrementNLink(iNodeDestinationDir);		
	}

	//Ajout de l'entrée de répertoire dans le répertoire parent
	if (AddDirEntry(iNodeDestinationDir, sNewName, iNewiNode) == 0) {
		//Erreur lors de l'ajout de l'entrée de répertoire
		printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
		return 0;		
	}
	IncrementNLink(iNewiNode);

	return 1;	
}



/*******************************************************************************
*					SECTION DES FONCTIONS OBLIGATOIRES  					   *
*******************************************************************************/


int bd_hardlink(const char *pPathExistant, const char *pPathNouveauLien) {
	return 1;
}

/*******************************************************************************
	Fonction : bd_mv(const char *pFilename, const char *pFilenameDest)

	Description : fonction qui déplace un fichier.

    pFilename :		nom du fichier source (incluant chemin).
    pFilenameDest : Nom du fichier destination (incluant chemin).

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int bd_mv(const char *pFilename, const char *pFilenameDest) {
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
	if (sNewFileName == pFilenameDest) {
		sNewFileName++;
		strncpy(sDestPath, pFilenameDest, sNewFileName-pFilenameDest);
	} else {
		strncpy(sDestPath, pFilenameDest, sNewFileName-pFilenameDest);
		sNewFileName++;
	}

//Debug
//	printf("Debug: '%s' .\n", sNewFileName);
//	printf("Debug: '%s' .\n", sDestPath);

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

	return 1;
}

/*******************************************************************************
	Fonction : int bd_mkdir(const char *pDirName)

	Description : fonction qui ajoute un répertoire.

    pDirName : nom complet du nouveau répertoire (incluant chemin).

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int bd_mkdir(const char *pDirName) {
	//Recherche du iNode lié à pDirLocation.
	ino iNodeDir = RechercheiNode(pDirName);

	//Est-ce que pDirName existe ?
	if (iNodeDir != 0) { //iNodeDir ne sera pas à 0 si pDirName existe
		//Répertoire ou fichier du même nom déjà existant
		printf("Erreur ! Le nom '%s' existe déjà.\n", pDirName);
		return 0;
	}

	//Extraire le nom du nouveau répertoire et le chemin depuis pDirName
	char *sNewDirName;
	char sExistingPath[MAX_PATH_SIZE]="";

	sNewDirName = strrchr(pDirName, '/');
	if (sNewDirName == pDirName) {
		sNewDirName++;
		strncpy(sExistingPath, pDirName, sNewDirName-pDirName);
	} else {
		strncpy(sExistingPath, pDirName, sNewDirName-pDirName);
		sNewDirName++;
	}

	//Recherche du iNode lié à sExistingPath
	iNodeDir = RechercheiNode(sExistingPath);

	// Est-ce que le chemin de destination existe ?
	if  (iNodeDir == 0) { //iNodeDir sera pas à 0 si sExistingPath n'existe pas
		//Chemin d'accès inexistant ! 
		printf("Erreur ! Le chemin d\'acces '%s' n\'existe pas.\n", sExistingPath);
		return 0;
	}
	
	//Lecture de l'iNode du chemin
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeDir, iNodeData);
	iNodeEntry *piNode= (iNodeEntry*)iNodeData;

	// Est-ce que sExistingPath est bien un répertoire ?
	if (piNode->iNodeStat.st_mode!=S_IFDIR) { //On vérifie que l'on termine bien sur un répertoire
		//Chemin d'accès n'est pas un répertoire
		printf("Erreur ! Le chemin d\'acces '%s' n\'est pas un répertoire.\n", sExistingPath);
		return 0;
	}
	
	//Création du repertoire
	CreateFile(S_IFDIR, iNodeDir, sNewDirName);

	return 1;
}


/*******************************************************************************
	Fonction : int bd_create(const char *pFilename)

	Description : fonction qui ajoute un fichier à un répertoire.

    pDirLocation : chemin d'accès où le fichier sera créé.

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int bd_create(const char *pFilename) {
	//Vérification que le fichier n'existe pas déjà.
	if (RechercheiNode(pFilename) == 0) {
		char *sFinDirName;
		char sFileName[FILENAME_SIZE]="";
		char sDirPath[MAX_PATH_SIZE]="";

		//Obtenir chemin d'accès du répertoire de création du fichier.
		sFinDirName = strrchr(pFilename, '/');
		strncpy(sDirPath, pFilename, sFinDirName-pFilename);
		strncpy(sFileName, sFinDirName + 1, FILENAME_SIZE);

		//Si la création est à la racine, ajuster la variable sDirPath à "/".
		if (strlen(sDirPath) == 0) { strcpy(sDirPath, "/"); }
//Debug			
		printf("Dossier de création %s.\nNom fichier %s.\n", sDirPath, sFileName);

		//Trouver le iNode du répertoire de création du fichier.
		ino iNodeDir = RechercheiNode(sDirPath);

		//Vérifier si le répertoire de création existe (0 => n'existe pas).
		if  (iNodeDir != 0) {

			//Lecture des données du iNode du répertoire de création.
			char iNodeData[BLOCK_SIZE];
			ReadBlock(BASE_BLOCK_INODE + iNodeDir, iNodeData);
			iNodeEntry *piNodeDir= (iNodeEntry*)iNodeData;

			//Vérifier que le chemin d'accès de création est un répertoire.
			if (piNodeDir->iNodeStat.st_mode==S_IFDIR) {

				//Vérifier s'il reste assez d'espace dans le bloc de données.
				if (piNodeDir->iNodeStat.st_size < BLOCK_SIZE) {

					//Cette partie ne fonctionne pas, ls n'affiche rien.
					//Par contre, les blocs de données sont saisis.
					//Le numéro de bloc saisi est parfois erratique.

					//Création du iNode du nouveau fichier.
					char iNodeDataNewFile[BLOCK_SIZE]="";
					iNodeEntry *piNodeNewFile = (iNodeEntry*)iNodeDataNewFile;
					piNodeNewFile->iNodeStat.st_ino = FindFirstFreeiNode();
					piNodeNewFile->iNodeStat.st_mode = S_IFREG;
					piNodeNewFile->iNodeStat.st_nlink = 0;
					piNodeNewFile->iNodeStat.st_size = 0;
					piNodeNewFile->iNodeStat.st_blocks = 1;
					SeizeFreeiNode(piNodeNewFile->iNodeStat.st_ino);
					piNodeNewFile->Block[0] = FindFirstFreeBlock();
					SeizeFreeBlock(piNodeNewFile->Block[0]);
					WriteBlock(piNodeNewFile->iNodeStat.st_ino, iNodeDataNewFile);

					//Déterminer le nombre d'entrée dans le répertoire.
					int nextDirEntry = piNodeDir->iNodeStat.st_size / sizeof(DirEntry);

				 	//Leture du bloc de données du répertoire.
					char BlockDirEntryDir[BLOCK_SIZE];
					ReadBlock(piNodeDir->Block[0], BlockDirEntryDir);
					DirEntry *pDirEntryDir = (DirEntry *)BlockDirEntryDir;

					//Ajout de la nouvelle entrée de répertoire à la fin.
					strcpy(pDirEntryDir[nextDirEntry].Filename,sFileName);
					pDirEntryDir[nextDirEntry].iNode = piNodeNewFile->iNodeStat.st_ino;	

					//Écriture du bloc de donnée du répertoire
					WriteBlock(piNodeDir->Block[0], BlockDirEntryDir);

					//Appel à la fonction utilitaire
					printf("Création du fichier %s\n", pFilename);					

					return 1;
				}

				//Pas assez d'espace dans le bloc de données.
				printf("Erreur ! Espace insuffisant dans le bloc de données du répertoire %s.\n", sDirPath);
				return 0;
			}

			//Chemin d'accès n'est pas un répertoire
			printf("Erreur ! Le chemin d\'acces '%s' n\'est pas un répertoire.\n", sDirPath);
			return 0;
		}

		//Chemin d'accès inexistant ! 
		printf("Erreur ! Le chemin d\'acces '%s' n\'existe pas.\n", sDirPath);
		return 0;
	}

	//Répertoire ou fichier du même nom déjà existant
	printf("Erreur ! Le nom '%s' existe déjà.\n", pFilename);
	return 0;
}


/*******************************************************************************
	Fonction : int bd_ls(const char *pDirLocation)

	Description : fonction qui affiche le contenu détaillé d'un répertoire.

    pDirLocation : chemin d'accès du dossier dont on veut afficher le contenu.
*******************************************************************************/
int bd_ls(const char *pDirLocation) {
	//Recherche du iNode lié à pDirLocation.
	ino iNodeDir = RechercheiNode(pDirLocation);

	// Est-ce que pDirLocation existe ?
	if (iNodeDir == 0) { //iNodeDir sera à 0 si pDirLocation n'existe pas
		//Dossier introuvable.
		printf("Echec de la commande ls. Dossier %s introuvable.", pDirLocation);
		return 0;
	}

	//Lecture du iNode.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeDir, iNodeData);
	iNodeEntry *piNode= (iNodeEntry*)iNodeData;

	// Est-ce que pDirLocation est bien un répertoire ?
	if (piNode->iNodeStat.st_mode!=S_IFDIR) { //On vérifie que l'on termine bien sur un répertoire
		//Le nom fourni n'est pas un répertoire
		printf("Erreur ! '%s' n\'est pas un répertoire.\n", pDirLocation);
		return 0;		
	}

	//Déterminer le nombre d'entrée de répertoire (ou "DirEntry")
	int nbDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);
	  
 	//Leture du bloc de données liées au iNode de pDirLocation.
	char DirEntryBlockData[BLOCK_SIZE];
	ReadBlock(piNode->Block[0], DirEntryBlockData);
	DirEntry *pDirEntry = (DirEntry *)DirEntryBlockData;

	//Affichage de la commande suivi de pDirLocation
	printf("ls %s \n",pDirLocation);

	//Déclarations de varaibles nécessaire dans la boucle for
	int i;
	char sName[FILENAME_SIZE];
	char cType;
	ino iNode;
	UINT16 sSize;
	UINT16 iMode;
	UINT16 iNlink;

	//Affichage du contenu du répertoire cible.
	for(i=0; i<nbDirEntry; i++) {
		//Lecture du nom et du iNode par l'entrée de répertoire
		strncpy(sName, pDirEntry[i].Filename,FILENAME_SIZE);
		iNode = pDirEntry[i].iNode;

		//Lecture du iNode de l'entrée courante
		char iNodeDataCurDirEntry[BLOCK_SIZE];
		ReadBlock(BASE_BLOCK_INODE + iNode, iNodeDataCurDirEntry);
		iNodeEntry *piNodeCurDirEntry= (iNodeEntry*)iNodeDataCurDirEntry;
		
		//Lecture du 'size', du 'mode' et du nombre de 'nlink'.
		sSize = piNodeCurDirEntry->iNodeStat.st_size;
		iMode = piNodeCurDirEntry->iNodeStat.st_mode;
		iNlink = piNodeCurDirEntry->iNodeStat.st_nlink;

		//Déterminer cType à partir de iMode
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

		//Affichage d'une ligne d'entrée de répertoire
		printf("%c %14s size: %6d inode: %3d nlink: %3d\n", cType, sName, sSize, iNode, iNlink);
	}

	return 1;
}




int bd_rm(const char *pFilename) {
	printf("iNode : %i\n", RechercheiNode(pFilename));

	return 1;
}


/*******************************************************************************
	Fonction : int bd_FormatDisk()

	Description : fonction qui "formate" le fichier "DisqueVirtuel.dat"
	              (initialise tous les blocs de données à zéro, initialise le
	              bitmap des blocs de données, initialise le bitmap des iNodes,
	              crée le répertoire racine).

    Retourne le numéro de bloc ou 0 si aucun bloc disponible.
*******************************************************************************/
int bd_FormatDisk() {
	char Bitmap[BLOCK_SIZE]="";
	int i = 0;

	//Initialisation à zéro d'un bloc de données.
	for (i = 0; i < BLOCK_SIZE; i++) {
		Bitmap[i] = 0;
	}

	//Mise à zéro de tous les blocs de données.
	for (i = 0; i < BLOCK_SIZE; i++) {
		WriteBlock(i, Bitmap);
	}

	//Initialisation de la liste des blocs de données.
	for (i = 0; i < N_BLOCK_ON_DISK; i++) {
		if (i<=43)
			Bitmap[i] = 0;	// 0 = occupé.
		else
			Bitmap[i] = 1;	// 1 = libre.
	}

	//Écriture de la table des blocs libres.
	WriteBlock(FREE_BLOCK_BITMAP, Bitmap);

	//Initialisation de la liste des iNodes.
	for (i = 0; i < BLOCK_SIZE; i++) {
		if (i > 0 && i < 40)
			Bitmap[i] = 1;	// 1 = libre.
		else
			Bitmap[i] = 0;	// 0 = occupé.
	}

	//Écriture de la table des iNodes.
	WriteBlock(FREE_INODE_BITMAP, Bitmap);

	//Création du iNode du répertoire racine.
	char iNodeData[BLOCK_SIZE]="";
	iNodeEntry *piNode = (iNodeEntry*)iNodeData;
	piNode->iNodeStat.st_ino = FindFirstFreeiNode();
	piNode->iNodeStat.st_mode = S_IFDIR;
	piNode->iNodeStat.st_nlink = 0;
	piNode->iNodeStat.st_size = 0;
	piNode->iNodeStat.st_blocks = 1;
	SeizeFreeiNode(piNode->iNodeStat.st_ino);
	piNode->Block[0] = FindFirstFreeBlock();
	SeizeFreeBlock(piNode->Block[0]);
	WriteBlock(BASE_BLOCK_INODE + ROOT_INODE, iNodeData);

	//Ajout de l'entrée de répertoire '.'
	if (AddDirEntry(ROOT_INODE, ".", ROOT_INODE) == 0) {
		//Erreur lors de l'ajout de l'entrée de répertoire
		printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
		return 0;		
	}
	IncrementNLink(piNode->iNodeStat.st_ino);	
	
	//Ajout de l'entrée de répertoire '..'
	if (AddDirEntry(ROOT_INODE, "..", ROOT_INODE) == 0) {
		//Erreur lors de l'ajout de l'entrée de répertoire
		printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
		return 0;		
	}
	IncrementNLink(piNode->iNodeStat.st_ino);

	return 1;
}
