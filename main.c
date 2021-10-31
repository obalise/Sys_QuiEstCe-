#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <errno.h>

const char* filename = "eleve.txt";

void launch_regex();

void return_tableau(char tableau[30][80]);

void selection_aleatoire_perso(char tableau[30][80], char personnageselect[80]);

int main(int argc, char *argv[])
{

    char tableau[30][80];
    char personnageselect[80];
    return_tableau(tableau);
    
    
    for(int i = 0; i < 1; i++)
    {
        puts(tableau[i]);

    }
    
    selection_aleatoire_perso(tableau, personnageselect);
    //launch_regex();

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

    //chdir("../BaseDeDonne");

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
    }
    printf("Sortie du fichier\n");
}
