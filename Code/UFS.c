#include "UFS.h"
#include <stdio.h>
#include <string.h>

//Définition de constantes
#define MAX_PATH_SIZE 255


/*******************************************************************************
*					SECTION DES FONCTIONS UTILITAIRES						   *
*******************************************************************************/


/*******************************************************************************
	Fonction : ino auxRechercheiNode(const char *pDirLocation, const char 
			   *DirCumulR, ino iNodeNumPrevDir)

	Description : fonction récursive auxiliaire de recherche de l'iNode d'un 
	              fichier à partir de son chemin d'accès.

	pDirLocation : chaîne de caractères contenant le chemin d'accès du fichier
                   recherché.
    DirCumulR : chemin d'accès courant de la recherche.
    iNodeNumPrevDir : numéro de l'iNode du répertoire de recherche courant.

    Retourne le numéro de l'iNode ou 0 si le chemin d'accès est invalide.
*******************************************************************************/
ino auxRechercheiNode(const char *pDirLocation, const char *DirCumulR, ino iNodeNumPrevDir) {
	//Lecture des informations du iNode courant.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumPrevDir, iNodeData);
	iNodeEntry *piNode = (iNodeEntry*) iNodeData;

 	//Leture du bloc de données associé au iNode courant.
	char DataBlockDirEntry[BLOCK_SIZE];
	ReadBlock(piNode->Block[0], DataBlockDirEntry);
	DirEntry *pDE = (DirEntry*) DataBlockDirEntry;

	//Calcul du nombre d'entrées de répertoire du iNode courant.
	int nbDirEntry = piNode->iNodeStat.st_size / sizeof(DirEntry);

	int i;
	char sName[FILENAME_SIZE];
	ino iNode;

	//Parcourir les entrées de répertoire du iNode courant.
	for(i = 0; i < nbDirEntry; i++) {

		//Composition du chemin d'accès du fichier.
		strcpy(sName, DirCumulR);
		strncat(sName, pDE[i].Filename,FILENAME_SIZE);
		
		//Fin d'itération si chemin d'accès équivalents.
		if (strcmp(sName,pDirLocation) == 0) {
			return pDE[i].iNode;
		}
		
		int iLongueur = strlen(sName);

		//Entre dans une nouvelle itération si les chemins d'accès cumulatifs sont compatibles.	
		if (strncmp(sName,pDirLocation,iLongueur) == 0 && pDirLocation[iLongueur] == '/') {
		
			//Ajoute '/' au chemin d'accès.
			strcat(sName,"/");

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
	//Lecture du bitmap des iNodes libres.
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);

	int iNodeNum;

	//Parcours du bitmap jusqu'à ce qu'un iNode soit trouvé.	
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

	//Recherche de l'entré de répertoire
	int i = 0;
	while  ( (i < nbDirEntry)  && (pDirEntry[i].iNode != iNodeEntryToRemove) ) {
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


/*******************************************************************************
	Fonction : int IncrementNLink(ino iNodeNumber)

	Description : Ajoute un lien à un fichier.

	iNodeNumber : iNode du fichier dont on doit incrémenter les liens.

    Retourne 1 si réussi.
*******************************************************************************/
int IncrementNLink(ino iNodeNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink++;
	WriteBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	return 1;
}


/*******************************************************************************
	Fonction : int DecrementNLink(ino iNodeNumber)

	Description : Retire un lien à un fichier.

	iNodeNumber : iNode du fichier dont on doit retirer un lien.

    Retourne 1 si réussi.
*******************************************************************************/
int DecrementNLink(ino iNodeNumber) {
	char BlockData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	iNodeEntry *piNode= (iNodeEntry*)BlockData;
	piNode->iNodeStat.st_nlink--;
	WriteBlock(BASE_BLOCK_INODE + iNodeNumber, BlockData);
	return 1;
}


/*******************************************************************************
	Fonction : int SeizeFreeiNode(ino iNodeNum)

	Description : Assigne un iNode à un fichier.

	iNodeNum : iNode à assigner au fichier.

    Retourne 1 si réussi.
*******************************************************************************/
int SeizeFreeiNode(ino iNodeNum) {
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	iNodeFreeBitmap[iNodeNum] = 0;
	printf("IFTFS: Saisie i-node %d\n",iNodeNum);
	WriteBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	return 1;
}


/*******************************************************************************
	Fonction : int ReleaseFreeiNode(ino iNodeNum)

	Description : Retire un iNode assigné à un fichier.

	iNodeNum : iNode à libérer.

    Retourne 1 si réussi.
*******************************************************************************/
int ReleaseFreeiNode(ino iNodeNum) {
	char iNodeFreeBitmap[BLOCK_SIZE];
	ReadBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	iNodeFreeBitmap[iNodeNum] = 1;
	printf("IFTFS: Relache i-node %d\n",iNodeNum);
	WriteBlock(FREE_INODE_BITMAP, iNodeFreeBitmap);
	return 1;
}


/*******************************************************************************
	Fonction : int SeizeFreeBlock(UINT16 BlockNum)

	Description : Retire un bloc de données assigné à un fichier.

	iNodeNum : Bloc de donnéesiNode à libérer.

    Retourne 1 si réussi.
*******************************************************************************/
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
	UINT16 iNewBlock = 0;

	//Est-ce qu'il y a un iNode libre?
	if (iNewiNode == 0) {
		//Plus de iNode libre.
		printf("Erreur ! Il n'y a plus de iNode libre.\n");
		return 0;
	}
	SeizeFreeiNode(iNewiNode);					 //Réservation du nouveau iNode.
	
	//Si on crée un répertoire
	if (iMode == S_IFDIR) {
		iNewBlock = FindFirstFreeBlock();			//Recherche d'un bloc libre.

		//Est-ce qu'il y a un bloc libre?
		if (iNewBlock == 0) {
			//Plus de bloc libre.
			printf("Erreur ! Il n'y a plus de bloc de données libre.\n");
			return 0;
		}
		SeizeFreeBlock(iNewBlock);				  //Réservation du nouveau bloc.
	}
	

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
		//Si ce n'est pas le répertoire racine que l'on crée
		if (strcmp(sNewName,"/") !=0 ) {
			if (AddDirEntry(iNewiNode, "..", iNodeDestinationDir) == 0) {
				//Erreur lors de l'ajout de l'entrée de répertoire
				printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
				return 0;		
			}
			IncrementNLink(iNodeDestinationDir);
		} else {
			if (AddDirEntry(iNewiNode, "..", iNewiNode) == 0) {
				//Erreur lors de l'ajout de l'entrée de répertoire
				printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
				return 0;		
			}
			IncrementNLink(iNewiNode);			
		}
	}

	//Si ce n'est pas le répertoire racine que l'on crée
	if (strcmp(sNewName,"/") !=0 ) {
		//Ajout de l'entrée de répertoire dans le répertoire parent
		if (AddDirEntry(iNodeDestinationDir, sNewName, iNewiNode) == 0) {
			//Erreur lors de l'ajout de l'entrée de répertoire
			printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
			return 0;		
		}
		IncrementNLink(iNewiNode);		
	}

	return 1;	
}



/*******************************************************************************
*					SECTION DES FONCTIONS OBLIGATOIRES  					   *
*******************************************************************************/


/*******************************************************************************
	Fonction : int bd_hardlink(const char *pPathExistant, const char 
	           				   *pPathNouveauLien)

	Description : fonction qui crée un lien additionnel vers un fichier existant.

    pPathExistant :		nom du fichier source (incluant chemin).
    pPathNouveauLien : Nom du fichier destination (incluant chemin).

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int bd_hardlink(const char *pPathExistant, const char *pPathNouveauLien) {

	//Recherche du iNode lié à pPathExistant.
	ino iNodePathExistant = RechercheiNode(pPathExistant);

	//Est-ce que le fichier pPathExistant existe ?
	if (iNodePathExistant == 0) { //iNodeSourceFile sera à 0 si pPathExistant n'existe pas.
		//Le fichier pPathExistant n'existe pas.
		printf("Erreur ! '%s' n\'existe pas.\n", pPathExistant);
		return 0;
	}

	//Lecture de l'iNode de pPathExistant
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodePathExistant, iNodeData);
	iNodeEntry *piNodePathExistant= (iNodeEntry*)iNodeData;

	// Est-ce que pPathExistant est bien un fichier ?
	if (piNodePathExistant->iNodeStat.st_mode!=S_IFREG) { //On vérifie que l'on termine bien sur un fichier.
		//Le nom fourni n'est pas un fichier.
		printf("Erreur ! '%s' n\'est pas un fichier.\n", pPathExistant);
		return 0;		
	}

	//Recherche du iNode lié à pPathNouveauLien.
	ino iNodePathNouveauLien = RechercheiNode(pPathNouveauLien);

	// Est-ce que le nom du nouveau lien est libre ?
	if (iNodePathNouveauLien != 0) { //iNodePathNouveauLien ne sera pas à 0 si pPathNouveauLien existe
		//Le nom de destination existe déjà
		printf("Erreur ! Le nom '%s' existe déjà.\n", pPathNouveauLien);
		return 0;
	}

	//Extraire le nom du fichier et le chemin depuis pPathNouveauLien
	char *sNewFileName;
	char sNouvPath[MAX_PATH_SIZE]="";

	sNewFileName = strrchr(pPathNouveauLien, '/');

	//Si sNewFileName ne contient aucun '/', alors le chemin est invalide
	if (sNewFileName==NULL) {
		printf("Erreur ! '%s' n\'est pas un chemin valide.\n", pPathNouveauLien);
		return 0;				
	}

	if (sNewFileName == pPathNouveauLien) {
		sNewFileName++;
		strncpy(sNouvPath, pPathNouveauLien, sNewFileName-pPathNouveauLien);
	} else {
		strncpy(sNouvPath, pPathNouveauLien, sNewFileName-pPathNouveauLien);
		sNewFileName++;
	}

	//Recherche du iNode lié à sNouvPath
	iNodePathNouveauLien = RechercheiNode(sNouvPath);

	// Est-ce que le chemin de sNouvPath existe ?
	if (iNodePathNouveauLien == 0) { //iNodePathNouveauLien sera à 0 si sNouvPath n'existe pas.
		//Le chemin d'accès du nouveau lien n'existe pas
		printf("Erreur ! Le chemin '%s' n\'existe pas.\n", sNouvPath);
		return 0;		
	}

	//Ajout de l'entrée de répertoire du nouveau lien.
	if (AddDirEntry(iNodePathNouveauLien, sNewFileName, iNodePathExistant) == 0) {
		//Erreur lors de l'ajout de l'entrée de répertoire
		printf("Erreur lors de l'ajout de l'entrée de répertoire.\n");
		return 0;		
	}

	//Incrémenter le nombre de lien du iNode du fichier.
	IncrementNLink(iNodePathExistant);

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

	//Si sNewFileName ne contient aucun '/', alors le chemin est invalide
	if (sNewFileName==NULL) {
		printf("Erreur ! '%s' n\'est pas un chemin valide.\n", pFilenameDest);
		return 0;				
	}

	if (sNewFileName == pFilenameDest) {
		sNewFileName++;
		strncpy(sDestPath, pFilenameDest, sNewFileName-pFilenameDest);
	} else {
		strncpy(sDestPath, pFilenameDest, sNewFileName-pFilenameDest);
		sNewFileName++;
	}

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

	//Si sNewFileName ne contient aucun '/', alors le chemin est invalide
	if (sNewDirName==NULL) {
		printf("Erreur ! '%s' n\'est pas un chemin valide.\n", pDirName);
		return 0;				
	}

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

    pFilename : chemin d'accès du fichier à créer.

    Retourne 1 si réussi, retourne 0 si échec.
*******************************************************************************/
int bd_create(const char *pFilename) {

	//Vérification que le nom de fichier n'existe pas déjà.
	if (RechercheiNode(pFilename) != 0) {
		//Répertoire ou fichier du même nom déjà existant
		printf("Erreur ! Le nom '%s' existe déjà.\n", pFilename);
		return 0;
	}

	//Obtenir la position du dernier '/' dans le chemin d'accès du fichier.
	char *sFinDirName;
	sFinDirName = strrchr(pFilename, '/');

	//Si sFinDirName ne contient aucun '/', alors le chemin est invalide
	if (sFinDirName==NULL) {
		printf("Erreur ! '%s' n\'est pas un chemin valide.\n", pFilename);
		return 0;				
	}

	//Obtenir le nom du fichier à créer à partir du chemin d'accès.
	char sFileName[FILENAME_SIZE]="";
	strncpy(sFileName, sFinDirName + 1, FILENAME_SIZE);

	//Obtenir chemin d'accès du répertoire de création du fichier.
	char sDirPath[MAX_PATH_SIZE]="";
	strncpy(sDirPath, pFilename, sFinDirName-pFilename);

	//Si la création est à la racine, ajuster la variable sDirPath à "/".
	if (strlen(sDirPath) == 0) { strcpy(sDirPath, "/"); }

	//Trouver le iNode du répertoire de création du fichier.
	ino iNodeDir = RechercheiNode(sDirPath);

	//Vérifier si le répertoire de création existe (0 => n'existe pas).
	if  (iNodeDir == 0) {
		//Chemin d'accès inexistant ! 
		printf("Erreur ! Le chemin d\'acces '%s' n\'existe pas.\n", sDirPath);
		return 0;
	}

	//Lecture des données du iNode du répertoire de création.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeDir, iNodeData);
	iNodeEntry *piNodeDir= (iNodeEntry*)iNodeData;

	//Vérifier que le chemin d'accès de création est un répertoire.
	if (piNodeDir->iNodeStat.st_mode != S_IFDIR) {
		//Chemin d'accès n'est pas un répertoire
		printf("Erreur ! Le chemin d\'acces '%s' n\'est pas un répertoire.\n", sDirPath);
		return 0;
	}

	//Vérifier s'il reste assez d'espace dans le bloc de données.
	if (piNodeDir->iNodeStat.st_size >= BLOCK_SIZE) {
		//Pas assez d'espace dans le bloc de données.
		printf("Erreur ! Espace insuffisant dans le bloc de données du répertoire '%s'.\n", sDirPath);
		return 0;
	}

	//Création du fichier.
	CreateFile(S_IFREG, iNodeDir, sFileName);

	return 1;		
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
		printf("Erreur ! Dossier '%s' introuvable.\n", pDirLocation);
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
	printf("%s \n",pDirLocation);

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
		printf(" %c %14s size: %6d inode: %3d nlink: %3d\n", cType, sName, sSize, iNode, iNlink);
	}

	return 1;
}



/*******************************************************************************
	Fonction : int bd_rm(const char *pFilename)

	Description : fonction qui efface un fichier ou un répartoire.

	pFilename : chemin d'accès du fichier à créer.

    Retourne 1 si l'opération réussi, 0 sinon.
*******************************************************************************/
int bd_rm(const char *pFilename) {
	
	//Déclaration des iNodes.
	ino iNodeFile = RechercheiNode(pFilename);
	ino iNodeParent;

	//Vérifier que le fichier ou répertoire existe.
	if (iNodeFile == 0) { 
		//Dossier introuvable.
		printf("Erreur ! Fichier ou dossier '%s' introuvable.\n", pFilename);
		return 0;
	}

	//Lecture du iNode.
	char iNodeData[BLOCK_SIZE];
	ReadBlock(BASE_BLOCK_INODE + iNodeFile, iNodeData);
	iNodeEntry *piNodeFile = (iNodeEntry*)iNodeData;

	//Est-ce que pFilename est un répertoire ?
	if (piNodeFile->iNodeStat.st_mode == S_IFDIR) {
		
		//Déterminer le nombre d'entrée de répertoire (ou "DirEntry")
		int nbDirEntry = piNodeFile->iNodeStat.st_size / sizeof(DirEntry);

		//Vérifier si le nombre de nlinks est supérieur à deux (dossier vide).
		if (nbDirEntry > 2) {
			//Le dossier n'est pas vide. Suppression impossible.
			printf("Erreur ! Le répertoire '%s' n'est pas vide.\n", pFilename);
			return 0;		
		}		

		
		if (nbDirEntry == 2) {
		 	//Leture du bloc de données liées au iNode de pFilename.
			char DirEntryBlockData[BLOCK_SIZE];
			ReadBlock(piNodeFile->Block[0], DirEntryBlockData);
			DirEntry *pDirEntry = (DirEntry *)DirEntryBlockData;

			//Vérifier que le répertoire contient seulement '.' et '..'.
			if (strcmp(".", pDirEntry[0].Filename) && (strcmp("..", pDirEntry[1].Filename))) {
				//Le répertoire contient autre chose que '.' et '..'.
				printf("Erreur ! Le répertoire '%s' contient autre chose que '.' et '..'.\n", pFilename);
				return 0;
			}

			//Récupérer le iNode du répertoire parent.
			iNodeParent = pDirEntry[1].iNode;

			//Décrémenter le nombre de liens du fichier et du répertoire parent.
			DecrementNLink(iNodeFile); // Une fois pour le dir "."
			DecrementNLink(iNodeParent); // Pour le dir ".."

			//Libérer le bloc de données du répertoire à supprimer.
			ReleaseFreeBlock(piNodeFile->Block[0]);
		}		
	} else { //Le filename est un fichier régulier.
		//Extraire le nom du fichier et le chemin depuis pFilename
		char *sNewFileName;
		char sPathParent[MAX_PATH_SIZE]="";

		sNewFileName = strrchr(pFilename, '/');
		if (sNewFileName == pFilename) {
			sNewFileName++;
			strncpy(sPathParent, pFilename, sNewFileName-pFilename);
		} else {
			strncpy(sPathParent, pFilename, sNewFileName-pFilename);
			sNewFileName++;
		}

		//Recherche du iNode lié à sPathParent
		iNodeParent = RechercheiNode(sPathParent);
	}
	
	//Retrait de l'entrée de répertoire du parent
	if (RemoveDirEntry(iNodeParent, iNodeFile) == 0) {
		//Erreur lors du retrait de l'entrée de répertoire
		printf("Erreur lors du retrait de l'entrée de répertoire.\n");
		return 0;		
	}

	//Décrémenter le nombre de lien du iNode du fichier à supprimer.
	DecrementNLink(iNodeFile); // Une fois pour l'entrée de répertoire du parent

	//Suppression du iNode pour un répertoire vide ou un fichier sans lien.
	if (((piNodeFile->iNodeStat.st_mode == S_IFREG) && (piNodeFile->iNodeStat.st_nlink == 1)) 
		|| (piNodeFile->iNodeStat.st_mode == S_IFDIR)) { 
		ReleaseFreeiNode(iNodeFile);
	}

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

	CreateFile(S_IFDIR, 0, "/");
	
	return 1;
}