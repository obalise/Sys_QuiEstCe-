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
#include <termios.h>

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
void personnageTrouve();
int attenteTouche ( void );


int main(void){

    signal(SIGINT, arretCTRLC); 
    signal(SIGUSR1, personnageTrouve);


    int descW, descR;
    char prenom[50];
    char tableau[NBR_PERSONNAGES ][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];

    chdir("../pipe"); //Pour le faire fonctionner sur les autres machines

    /* On demande le nom du client, qui est tu ?*/
    printf("Bonjour et bienvenue dans le jeu \"Qui est-ce\" ?\n");
    printf("Quel est votre prenom ? (Exemples : Olivier, Arnaud...)\n");
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
    read(descR, personnageselect, sizeof(char)*NBR_CARACTERES);
    close(descR);

    int resultat = menu(personnageselect, tableau);


    if (resultat == 1){

        MessageClientServeur *messageFinal = malloc(sizeof(MessageClientServeur));
        messageFinal->type_message = 1;
        messageFinal->resultat = resultat;
        messageFinal->pid = getpid();
        strcpy(messageFinal->identite_envoyeur, prenom);

        descW=open("main",O_WRONLY);
        write(descW, messageFinal,sizeof(MessageClientServeur));
        close(descW); // on ferme le descripteur
        printf("\nFin du client de %s!\n", prenom);
        printf("Au revoir !\n");
        exit (1);
    }
}


int menu(char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES][NBR_CARACTERES]) {

    int i = 0;
    char chaine_recherche[NBR_CARACTERES];
    int status = 0;
    char elu[NBR_PERSONNAGES];

    char index_token[] = "";
    strcpy(index_token, personnageselect);
    char **tokens;
    tokens = str_split(index_token, ':');
    if (tokens) {
        strcpy(elu, *(tokens) + 0);
        free(tokens);
    } else {
        printf("Program Error\n");
        exit(0);
    }
    
    printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
    
    do {
        //menu global       
        printf("\n********** Bienvenue dans le menu du jeu QUI EST-CE ? **********\n");
        printf("|  0 | Quitter le programme\n");
        printf("|  1 | Saisie caracteristique\n");
        printf("|  2 | Proposition du personnage mystere (penalite -> 3sec)\n");
        printf("|  3 | Affiche tableau\n");

        scanf("%d", &i);
        switch (i) {
            case 0:
                exit(0);
                break;
            case 1:
                printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                //affichagePersonnages(tableau);
                //menu des caracteristiques
                printf("|  1 | Couleur des yeux\n");
                printf("|  2 | Couleur des cheveux\n");
                printf("|  3 | Regularité à l'apéro\n");

                scanf("%d", &i);
                switch (i) {
                    case 0:
                        break;
                    case 1:
                        //menu des yeux
                        printf("|  1 | Vert\n");
                        printf("|  2 | Lunettes\n");
                        printf("|  3 | Marron\n");
                        printf("|  4 | Bleu\n");
                        printf("|  5 | Noisette\n");

                        scanf("%d", &i);
                        switch (i) {
                            case 0:
                                break;
                            case 1:
                                strcpy(chaine_recherche, "vert");
                                break;
                            case 2:
                                strcpy(chaine_recherche, "lunettes");
                                break;
                            case 3:
                                strcpy(chaine_recherche, "marron");
                                break;
                            case 4:
                                strcpy(chaine_recherche, "bleu");
                                break;
                            case 5:
                                strcpy(chaine_recherche, "noisette");
                                break;
                            default:
                                printf("Invalide !\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
                        printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                        break;
                    case 2:
                        //menu des cheveux
                        printf("|  1 | Chatain\n");
                        printf("|  2 | Grisonant\n");
                        printf("|  3 | Brun\n");
                        printf("|  4 | Blond\n");
                        printf("|  5 | Boucle\n");

                        scanf("%d", &i);
                        switch (i) {
                            case 0:

                                break;
                            case 1:
                                strcpy(chaine_recherche, "chatain");
                                break;
                            case 2:
                                strcpy(chaine_recherche, "grisonant");
                                break;
                            case 3:
                                strcpy(chaine_recherche, "brun");
                                break;
                            case 4:
                                strcpy(chaine_recherche, "blond");
                                break;
                            case 5:
                                strcpy(chaine_recherche, "boucle");
                                break;
                            default:
                                printf("Invalide\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
						printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                        break;
                    case 3:
                        //menu de la regularite à l'apéro
                        printf("|  1 | Faible\n");
                        printf("|  2 | Moyen\n");
                        printf("|  3 | Fort\n");
                        printf("|  4 | Alcoolique\n");
                        printf("|  5 | Inexistant\n");

                        scanf("%d",&i);
                        switch(i)
                        {
                            case 0: break;
                            case 1:
                                strcpy(chaine_recherche,"faible");
                                break;
                            case 2:
                                strcpy(chaine_recherche,"moyen");
                                break;
                            case 3:
                                strcpy(chaine_recherche,"fort");
                                break;
                            case 4:
                                strcpy(chaine_recherche,"alcoolique");
                                break;
                            case 5:
                                strcpy(chaine_recherche,"inexistant");
                                break;
                            default:
                                printf ("Invalide !\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
						printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                        break;
                    default:
                        printf ("Invalide !\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
						printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                        break;
                }

                status = launch_regex(personnageselect, chaine_recherche);
                if ((status == 0) || (status == 1)) {
                    if (status == 0) {
                        printf("Votre personnage possède bien cette caractéristique\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
						printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                    }
                    if (status == 1) {
                        printf("Votre personnage n'a pas cette caractéristique\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
						printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                    }
                } else {
                    printf("Erreur comparaison Regex avec la caractéristique !\n");
                    printf("Appuyer sur une touche pour continuer.\n");
					attenteTouche();
					printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                    exit(0);
                }
                break;
            case 2:
				printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
				
                printf("Quel est le nom (MAJUSCULE) ou le prenom (minuscule) de l'élève mystère ?\n");
                scanf("%s", chaine_recherche);
                status = launch_regex(elu, chaine_recherche);

                if ((status == 0) || (status ==1)) {     //C'est surement le if le plus chelou que j'ai vu, faut le changer ou le prof se moquera de nous.
                    if (status == 0) {
                        printf("Personnage trouvé %s ! C'est gagné !\n", elu);
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
                        return 1;
                    }
                    if (status == 1) {
						printf("Pénalité ! Vous devez attendre 3 secondes avant de pouvoir reprendre la partie !\n");
                        sleep(3);
                        printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                    }
                } else {
                    printf("Erreur comparaison Regex avec l'élève mystère !\n");
                    printf("Appuyer sur une touche pour continuer.\n");
					attenteTouche();
					printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                    exit(0);
                }
                break;
            case 3:
				printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
                affichagePersonnages(tableau);
                break;
            default:
                break;
        }

    }
    while (i != 0);
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

char** str_split(char* a_str, const char a_delim) {
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
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
    result = malloc(sizeof(char *) * count);
    if (result) {
        size_t idx = 0;
        char *token = strtok(a_str, delim);
        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

void personnageTrouve() {
    printf("Un de vos adversaires vous a devancé et a trouvé l'élève caché !\nLa carte Olimex vous dévoilera de qui il s'agit !\n");
}


int attenteTouche ( void ) 
{
  int ch;
  struct termios oldt, newt;
  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}
