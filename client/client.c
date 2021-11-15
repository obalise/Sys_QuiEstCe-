#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <assert.h>

#define NBR_CARACTERES 80
#define NBR_PERSONNAGES 19

typedef struct messageClientServeur {
	int type_message;         //0 Initialisation et 1 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 1 (message final de fin de partie)
	pid_t pid;
    char identite_envoyeur [50];
} MessageClientServeur;


int menu (char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void strcpy_pointeur(char *, char);
int launch_regex(char *, char *);
void affichagePersonnages(char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void arretCTRLC(){exit(0);};
char** str_split(char* , const char);

int arretProgrammeGagnantTrouve = 0;


int main(void){
	
	signal(SIGINT, arretCTRLC); // ingnore ctrl+c
	signal(SIGUSR1, personnageTrouve);
	
	
    int descW, descR;
    char prenom[50];
    char tableau[NBR_PERSONNAGES ][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];

    chdir("../pipe"); //Pour le faire fonctionner sur les autres machines

	/* On demande le nom du client, qui est tu ?*/
	printf("Bonjour bienvenu de le jeu Qui est-ce ?\n");
	printf("Quel est votre Prenom?\n");
	scanf("%s", prenom);
	
	MessageClientServeur *messageInitialisation = malloc(sizeof(MessageClientServeur));
	messageInitialisation->type_message = 0;
	messageInitialisation->resultat = 0;
	messageInitialisation->pid = getpid();
	strcpy(messageInitialisation->identite_envoyeur, prenom);
	
	/* penser à ajouter main entre ""*/
    descW=open("main",O_WRONLY); // on ouvre le pipe main en ecriture
    write(descW, messageInitialisation, sizeof(MessageClientServeur));
    close(descW); // on ferme le descripteur
    sleep(1);
	
	descR=open(prenom,O_RDONLY); // on ouvre le pipe main en lecture
	read(descR, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES);
	//Voyons voir avec ce for si le tableau s'est rempli correctement
	for(int i = 0; i <NBR_PERSONNAGES ; i++){
	        printf("%d -> ", i );
        	puts(tableau[i]); 
    	}
	read(descR, personnageselect, sizeof(char)*NBR_CARACTERES);
	close(descR);
	
    int resultat = menu(personnageselect, tableau);
	
	if (resultat == 2){
		
		int descR;
		char gagnant[50];
		
		printf("\nQuelqu'un a trouvé le personnage ! \n");
		
		descR=open(prenom,O_RDONLY); // on ouvre le pipe main en lecture
		read(descR, gagnant, sizeof(char)*50);
		close(descR);
		printf("\n%s est le gagnant ! \n", gagnant);
		
		exit(0);
		
	}else if (resultat == 1){
		
		MessageClientServeur *messageFinal = malloc(sizeof(MessageClientServeur));
		messageFinal->type_message = 1;
		messageFinal->resultat = resultat;
		messageFinal->pid = getpid();
		strcpy(messageFinal->identite_envoyeur, prenom);	
		
		descW=open("main",O_WRONLY); 
		write(descW, messageFinal,sizeof(MessageClientServeur)); 
		close(descW); // on ferme le descripteur
		printf("\nJ'vai m'balader !\n");
		exit (8);
	}
}


int menu(char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES][NBR_CARACTERES]){

    int i = 0 ;
    char chaine_recherche[NBR_CARACTERES];
    int status = 0;
    char elu[NBR_PERSONNAGES];

    char index_token[] = "";
    strcpy(index_token,personnageselect);
    char** tokens;
    tokens = str_split(index_token,':');
    if (tokens) {
        strcpy(elu,*(tokens)+0);
        free(tokens);
    }
    else{
        printf("Program Error\n");
        exit(0);
    }

    do{
        //menu global
        printf("\n********** Bienvenue dans le menu du jeu QUI EST-CE ? **********\n");
        printf ("|  0 | Quitter le programme\n");
        printf ("|  1 | Saisie caracteristique\n");
        printf ("|  2 | Proposition du personnage mystere (penalite -> 3sec)\n");
        printf ("|  3 | Affiche tableau\n");

        scanf("%d",&i);
        switch(i)
        {
            case 0:
                exit(0);
                break;
            case 1:
                printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                affichagePersonnages(tableau);
                //menu des caracteristiques
                printf ("|  1 | couleur des yeux\n");
                printf ("|  2 | couleur des cheveux\n");
                printf ("|  3 | regularite à l'apéro\n");

                scanf("%d",&i);
                switch(i)
                {
                    case 0: break;
                    case 1:
                        //menu des yeux
                        printf ("|  1 | vert\n");
                        printf ("|  2 | lunettes\n");
                        printf ("|  3 | marron\n");
                        printf ("|  4 | bleu\n");
                        printf ("|  5 | noisette\n");

                        scanf("%d",&i);
                        switch(i)
                        {
                            case 0: break;
                            case 1:
                                strcpy(chaine_recherche,"vert");
                                break;
                            case 2:
                                strcpy(chaine_recherche,"lunettes");
                                break;
                            case 3:
                                strcpy(chaine_recherche,"marron");
                                break;
                            case 4:
                                strcpy(chaine_recherche,"bleu");
                                break;
                            case 5:
                                strcpy(chaine_recherche,"noisette");
                                break;
                            default:
                                printf ("Invalide !\n");
                                break;
                        }
                        break;
                    case 2:
                        //menu des cheveux
                        printf ("|  1 | chatain\n");
                        printf ("|  2 | grisonant\n");
                        printf ("|  3 | brun\n");
                        printf ("|  4 | blond\n");
                        printf ("|  5 | boucle\n");

                        scanf("%d",&i);
                        switch(i)
                        {
                            case 0:

                                break;
                            case 1:
                                strcpy(chaine_recherche,"chatain");
                                break;
                            case 2:
                                strcpy(chaine_recherche,"grisonant");
                                break;
                            case 3:
                                strcpy(chaine_recherche,"brun");
                                break;
                            case 4:
                                strcpy(chaine_recherche,"blond");
                                break;
                            case 5:
                                strcpy(chaine_recherche,"boucle");
                                break;
                            default:
                                printf ("Invalide\n");
                                break;
                        }
                        break;
                    case 3:
                        //menu de la regularite à l'apéro
                        printf ("|  1 | faible\n");
                        printf ("|  2 | moyen\n");
                        printf ("|  3 | fort\n");
                        printf ("|  4 | alcoolique\n");
                        printf ("|  5 | inexistant\n");

                    p_chaine_recherche = chaine_recherche;
                    status = launch_regex(personnageselect,chaine_recherche);
                    if ((status == 0) || (status == 1)) {
                        if (status == 0) {
                            printf("Votre personnage a la caractériqtique\n");
                        }
                        break;
                    default:
                        printf ("Invalide !\n");
                        break;
                }

                status = launch_regex(personnageselect,chaine_recherche);
                if ((status == 0) || (status == 1)) {
                    if (status == 0) {
                        printf("Votre personnage a la caractériqtique\n");
                    }
                    if (status == 1) {
                        printf("Votre personnage n'a pas la caractériqtique\n");
                    }
                }
                else{
                    printf("Program Error\n");
                    exit(0);
                }
                break;
            case 2:
                printf("Quel est le nom (MAJUSCULE) ou le prenom (minuscule) du personnage mystère\n");
                scanf("%s", chaine_recherche);
                status = launch_regex(elu,chaine_recherche);

                if ((status == 0) || (status == 1)){     //C'est surement le if le plus chelou que j'ai vu, faut le changer ou le prof se moquera de nous.
                    if (status == 0){
                        printf("Personnage trouve %s ! C'est gagné !\n", elu);
                        return 1;
                    }
                    if (status == 1){
                        sleep(3);
                    }
                }
                else{
                    printf("Program Error\n");
                    exit(0);
                }
                break;
            case 3:
                affichagePersonnages(tableau);
                break;
            default:
                break;
        }

    }while (i != 0 && arretProgrammeGagnantTrouve == 0 );
    printf("On sort du menu car quelqu'un d'autre a gagné\n");
    return 2; 
}

int launch_regex(char *elu, char *p_chaine_recherche){
    regex_t regex;
    int reti;
    char msgbuf[100];
    int status = 0;

/* Compile regular expression */
    reti = regcomp(&regex, p_chaine_recherche, 0);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

/* Execute regular expression */
    reti = regexec(&regex, elu, 0, NULL, 0);
    if (!reti) {
        puts("Match");
        status = 0;
    }
    else if (reti == REG_NOMATCH) {
        puts("No match");
        status = 1;
    }
    else {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        status = 2;
        exit(1);
    }

/* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
    return status;
}


void affichagePersonnages(char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]){

    printf("Liste des Personnages et de leurs caractéristiques: \n");
    for(int i = 0; i < NBR_PERSONNAGES ; i++)
    {
        printf("%d -> ", i );
        puts(tableau[i]);
    }

}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;
    result = malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
void personnageTrouve(){
	
	arretProgrammeGagnantTrouve = 1;
	printf("[CLIENT] SIGUSR1 reçu!!!!\n");
}

