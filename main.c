#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


const char* filename = "eleve.txt";

/*Prototype*/
void return_tableau(char tableau[20][80]);
int serveur(char tableau[20][80], char personnageselect[80]);
void selection_aleatoire_perso(char tableau[20][80], char personnageselect[80]);

int main(int argc, char *argv[])
{

    char tableau[20][80];
    char personnageselect[80];
    return_tableau(tableau);
    
    selection_aleatoire_perso(tableau, personnageselect);
    
    int status = 1;

	while(status)
	{
		status = serveur(tableau, personnageselect);

	}

    	return 0;
}

void selection_aleatoire_perso(char tableau[20][80], char personnageselect[80])
{
	srand(getpid());
	int numerolignealeatoire = (rand() % 20);
	printf("Le nombre aléatoire entre 1 et 19 est %d!\n", numerolignealeatoire);
	strcpy(personnageselect, tableau[numerolignealeatoire]);
	printf("Ligne choisie est : %s", personnageselect);
	
}

void return_tableau(char tableau[20][80]){
	
	errno = 0;
	
    FILE* in_file = NULL;

    chdir("./BaseDeDonne");

    in_file = fopen(filename, "r+");
    char ligne[80];
    int i = 0;

    if (in_file == NULL)
    {
		printf("Echec ouverture fichier !\n");
		printf("Errno: %d \n", errno);
		exit(0);
	}
	else{
        while(fgets(ligne, 80, in_file) != NULL) {
           // printf("%s", ligne);
            strcpy(tableau[i],ligne);
            i++;
            //puts(&tableau[i]);
        }
        // On l'écrit dans le fichier
        fclose(in_file);

	chdir("../.");
    }
    printf("Sortie du fichier\n");
}

int serveur(char tableau[20][80], char personnageselect[80])
{
	int pid,descR,descW,nb,test;
	int etudiant=0;
    char buf[80], prenom[50];
	char main[30]= "./pipe/main";
	char chemin[9]= "./pipe/";
	

    	unlink(main);
    	mkfifo(main,0666); // creation du pipe fifo
   	do
	{ 
    		descR=open(main,O_RDONLY); //ouverture du pipe
    		nb=read(descR,buf,80); // Ecoute sur le pipe main par bloc de 80 max
    		
		etudiant++;

    		/*post traitement de ce qu'on recoit dans le pipe à ajouter ici*/
    		buf[nb]='\0';
    		printf("Client: %s\n",buf);
    	
		strcpy(prenom, buf);

		test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier"));	

		if(test==0)
		{
	
			/****************Gestion processus***********************/
			/* on cree un processus des qu'il y a un nouveau client */
    			pid=fork();
	
    			if(pid == 0)
			{
        			//execv("ici la modularite", NULL);
				printf("on est dans le fils de %s\n",prenom);
				
				strcat(chemin,prenom);
	
    				unlink(chemin);
    				mkfifo(chemin,0755); // creation du pipe client
		
    				descW=open(chemin,O_WRONLY); //ouverture du pipe
    				write(descW,prenom,20); // Ecriture sur le pipe client
    				
    				write(descW, tableau, sizeof(char)*20*80);
    				
    				write(descW, personnageselect, sizeof(char)*80);
    				
				exit(0);
			}
    			wait(NULL); // on attende la fin du processus fils
		}
		/**************Fin gestion processus*********************/
	}while(etudiant!=4);
    	close(descR); // fermeture du descripteur lecture
    
    	exit(0);
}
