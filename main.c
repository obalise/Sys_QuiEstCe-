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
void launch_regex();
void return_tableau(char tableau[30][80]);
int serveur(char tableau[30][80], char personnageselect[80], int sec);
void selection_aleatoire_perso(char tableau[30][80], char personnageselect[80]);
void fin(int sig);

int main(int argc, char *argv[], char *arge[])
{
	char tableau[30][80];
    	char personnageselect[80];
	int sec = 0;

	sec = atoi(argv[1]);	

    	return_tableau(tableau);
 
    	selection_aleatoire_perso(tableau, personnageselect);

	serveur(tableau, personnageselect, sec);

 	return 0;
}

void launch_regex(){
    regex_t regex;
    int reti;
    char msgbuf[100];

/* Compile regular expression */
    reti = regcomp(&regex, "^a[[:alnum:]]", 0);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

/* Execute regular expression */
    reti = regexec(&regex, "abc", 0, NULL, 0);
    if (!reti) {
        puts("Match");
    }
    else if (reti == REG_NOMATCH) {
        puts("No match");
    }
    else {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(1);
    }

/* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
}

void selection_aleatoire_perso(char tableau[30][80], char personnageselect[80])
{
	srand(getpid());
	int numerolignealeatoire = (rand() % 20);
	printf("Le nombre aléatoire entre 1 et 19 est %d!\n", numerolignealeatoire);
	strcpy(personnageselect, tableau[numerolignealeatoire]);
	printf("Ligne choisie est : %s", personnageselect);
	
}

void return_tableau(char tableau[30][80]){
	
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
            printf("%s", ligne);
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

int serveur(char tableau[30][80], char personnageselect[80], int sec)
{
	int pid,pid2,descR,descW,nb,test;
	int status=1;
    	char buf[80], prenom[50];
	char main[30]= "./pipe/main";
	char chemin[9]= "./pipe/";
	char * myArgv[2];	

	alarm(sec);
    	signal(SIGALRM, fin);	 
	
    	unlink(main);
    	mkfifo(main,0666); // creation du pipe fifo
   	do
	{ 
    		descR=open(main,O_RDONLY); //ouverture du pipe
    		nb=read(descR,buf,80); // Ecoute sur le pipe main par bloc de 80 max

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
    			pid=fork();
			pid2=fork();	
    			if(pid == 0)
			{
        			//execv("ici la modularite", NULL);
				printf("on est dans le fils de %s\n",prenom);
				
				strcat(chemin,prenom);
	
    				unlink(chemin);
    				mkfifo(chemin,0755); // creation du pipe client
		
    				descW=open(chemin,O_WRONLY); //ouverture du pipe
    				write(descW,prenom,20); // Ecriture sur le pipe client
    				
    				write(descW, tableau, sizeof(char)*30*80);
    				
    				write(descW, personnageselect, sizeof(char)*80);
    				
				exit(0);
			}

		}
		/**************Fin gestion processus*********************/
	}while(status);

	if(pid2 == 0)
	{			
		printf("on est dans pid2\n");
		myArgv[0]="stats";
        	myArgv[1]="/home/isen/Sys_QuiEstCe-/pipe/main";
        	execv("stats", myArgv);
	}

    	wait(NULL); // on attende la fin du processus fils

    	close(descR); // fermeture du descripteur lecture
    
    	exit(0);
}

void fin(int sig)
{
	printf("Fin du jeu\n");
	exit(0);
}
