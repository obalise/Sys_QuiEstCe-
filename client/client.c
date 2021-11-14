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

#define NBR_CARACTERES 80
#define NBR_PERSONNAGES 20


void menu (char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void strcpy_pointeur(char *, char);
int launch_regex(char *, char *);
void affichagePersonnages(char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void arretCTRLC(){exit(0);};


int main(void){
	
    int descW,descR,nb;
    char prenom[50];
    char buf[NBR_CARACTERES];
    char tableau[NBR_PERSONNAGES ][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];

    chdir("../pipe"); //Pour le faire fonctionner sur les autres machines

	/* On demande le nom du client, qui est tu ?*/
	printf("Bonjour bienvenu de le jeu Qui est-ce ?\n");
	printf("Quel est votre Prenom?\n");
	scanf("%s", prenom);
	
	signal(SIGINT, arretCTRLC); // ingnore ctrl+c
	
	/* penser à ajouter main entre ""*/
    descW=open("main",O_WRONLY); // on ouvre le pipe main en ecriture
    write(descW,prenom,50); // on ecrit le nom du nouveau client
    close(descW); // on ferme le descripteur
    sleep(1);

    descR=open(prenom,O_RDONLY); // on ouvre le pipe main en lecture
	nb=read(descR,buf,NBR_CARACTERES);
	buf[nb]='\0';
	printf("Retour serveur: %s\n",buf);
	
	read(descR, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES);
		
	//Voyons voir avec ce for si le tableau s'est rempli correctement
	for(int i = 0; i <NBR_PERSONNAGES ; i++){
	        printf("%d -> ", i );
        	puts(tableau[i]); 
    	}
        	
	read(descR, personnageselect, sizeof(char)*NBR_CARACTERES);
    menu(personnageselect, tableau);
	//sleep(1);
	close(descR);
	write(descW,"_vainqueur_",12); // on ecrit de le pipe main si on a trouvé l'eleve mystere
	close(descW); // on ferme le descripteur
    printf("\nJ'vai m'balader !\n");
    exit (8);
}


void menu(char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES][NBR_CARACTERES]){

    int i = 0 ;
    char chaine_recherche[NBR_CARACTERES];
    char *p_chaine_recherche;
    int status = 0;
    bool recherche_personnage = false;

    do{
        //menu global
        printf("\n********** Bienvenu dans le menu du jeux QUI EST CE????? **********\n");
        printf ("|  0 | Quitter le programme\n");
        printf ("|  1 | Saisie caracteristique\n");
        printf ("|  2 | Proposition de l'eleve\n");
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
                            break;
						default:
							 printf ("Invalide !\n");
                            break;
                    }

                    p_chaine_recherche = chaine_recherche;
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
                recherche_personnage = true;
                printf("Quel est le nom du personnage mystere\n");
                scanf("%s", chaine_recherche);
                status = launch_regex(personnageselect,chaine_recherche);
                if ((status == 0) || (status == 1)){
                    if (status == 0){
                        printf("Personnage trouve%s\n", p_chaine_recherche);
                    }
                    if (status == 1){
                        printf("Personnage faux, Point de pénalité !!!!!\n");
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
        if (recherche_personnage == true){
            printf("Arret partie");
            // fonction retour pipe
        }
    }while (i != 0);
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
