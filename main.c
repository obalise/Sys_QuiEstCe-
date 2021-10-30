#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>


const char* filename = "eleve.txt";


/*Prototype*/
void launch_regex();
void return_tableau(char *tableau);
int serveur(void);



int main(int argc, char *argv[])
{

	char tableau[30][80];
	int status=1;

    	return_tableau(*tableau);

	while(status)
	{
		status = serveur();
	
    		launch_regex();
	}

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

void return_tableau(char *tableau){
    FILE* in_file = NULL;

    chdir("../BaseDeDonne");

    in_file = fopen(filename, "r+");
    char ligne[80];
    int i = 0;

    if (in_file != NULL)
    {
        while(fgets(ligne, 80, in_file) != NULL) {
            printf("%s", ligne);
            strcpy(&tableau[i],ligne);
            i++;
        }
        // On l'écrit dans le fichier
        fclose(in_file);
    }

}

int serveur(void)
{
	int pid,descR,descW,nb,len,test;
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
				exit(0);
			}
    			wait(NULL); // on attende la fin du processus fils
		}
		/**************Fin gestion processus*********************/
	}while(etudiant!=4);
    	close(descR); // fermeture du descripteur lecture
    
    	exit(0);
}
