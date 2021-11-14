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
//#include <check.h>

#define NBR_CARACTERES 80
#define NBR_PERSONNAGES 20

/*
START_TEST (test_name)
{
  		//unit test code 
}
END_TEST*/

const char* filename = "eleve.txt";

/*Prototype*/
void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]);
void launch_regex();
int serveur(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES], int sec);
void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES]);
void fin(int sig);

int main(int argc, char *argv[], char *arge[])
{
	int sec = 0;
    char tableau[NBR_PERSONNAGES][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];
    return_tableau(tableau);
	sec = atoi(argv[1]);	

    return_tableau(tableau);
    
    printf("Liste des Personnages et de leurs caractéristiques: \n");
	 for(int i = 0; i < NBR_PERSONNAGES ; i++)
    {
        printf("%d -> ", i );
        puts(tableau[i]);
    }
    
    selection_aleatoire_perso(tableau, personnageselect);
    	
    /*
	while(status)
	{
		status = serveur(tableau, personnageselect);

	}*/

	serveur(tableau, personnageselect, sec);

 	return 0;
}

void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES])
{
	srand(getpid());
	int numerolignealeatoire = (rand() % NBR_PERSONNAGES);
	printf("Le nombre aléatoire entre 1 et 19 est %d!\n", numerolignealeatoire);
	strcpy(personnageselect, tableau[numerolignealeatoire]);
	printf("Ligne choisie est : %s", personnageselect);
	
}

void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]){
	
	errno = 0;
	
    FILE* in_file = NULL;

    chdir("./BaseDeDonne");

    in_file = fopen(filename, "r+");
    char ligne[NBR_CARACTERES];
    int i = 0;

    if (in_file == NULL)
    {
		printf("Echec ouverture fichier !\n");
		printf("Errno: %d \n", errno);
		exit(0);
	}
	else{
        while(fgets(ligne, NBR_CARACTERES , in_file) != NULL) {
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

int serveur(char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ], int sec)
{
	int pid,pid2,pid3,descR,descW,nb,test;
	int status=1;
    char buf[NBR_CARACTERES ], prenom[50];
	char main[NBR_PERSONNAGES]= "./pipe/main";
	char chemin[9]= "./pipe/";
	char * myArgv[3];	

	alarm(sec);
    signal(SIGALRM, fin);	 
	
    unlink(main);
    mkfifo(main,0666); // creation du pipe fifo

	
	pid=fork();	
	if(pid == 0)
	{
		myArgv[0]="home/isen/Sys_QuiEstCe-/stats";
        myArgv[1]="/home/isen/Sys_QuiEstCe-/pipe/main";
		myArgv[2]=NULL;
        execv("/home/isen/Sys_QuiEstCe-/stats", myArgv);
	}
    	wait(NULL); // on attende la fin du processus fils

   	do
	{ 
    		descR=open(main,O_RDONLY); //ouverture du pipe
    		nb=read(descR,buf,50 ); // Ecoute sur le pipe main par bloc de 80 max
		
    		/*post traitement de ce qu'on recoit dans le pipe à ajouter ici*/
    		buf[nb]='\0';
    		printf("Client: %s\n",buf);
    		
		if (nb == 12)
		{
			status = 0; //on sort du do-while retour vainqueur 
		}
		
		strcpy(prenom, buf);

		test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier"));	

		if(test==0)
		{
			/****************Gestion processus***********************/
			/* on cree un processus des qu'il y a le premier client */
    			pid2=fork();
    			if(pid2 == 0)
			{
				printf("on est dans le fils de %s\n",prenom);
				
				strcat(chemin,prenom);
	
    				unlink(chemin);
    				mkfifo(chemin,0755); // creation du pipe client
		
    				descW=open(chemin,O_WRONLY); //ouverture du pipe
    				
    				//write(descW,prenom,20); // Ecriture sur le pipe client
    				
    				write(descW, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES );
    				
    				write(descW, personnageselect, sizeof(char)*NBR_CARACTERES);
    				
				exit(0);
			}
		}
		/**************Fin gestion processus*********************/
	}while(status);


    	wait(NULL); // on attende la fin du processus fils

	close(descW); // Fermeture du descritpeur d'ecriture
    close(descR); // fermeture du descripteur lecture
   
	pid3=fork();	
	if(pid3 == 0)
	{
		myArgv[0]="home/isen/Sys_QuiEstCe-/socket";
        myArgv[1]="le gagnant";
		myArgv[2]=NULL;
        execv("/home/isen/Sys_QuiEstCe-/socket", myArgv);
	}

    	wait(NULL); // on attende la fin du processus fils

 
    	exit(0);
}

void fin(int sig)
{
	printf("Fin du jeu\n");
	exit(0);
}
