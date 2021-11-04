#include<fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void menu ();

int main(void)
{
    int descW,descR,nb;
	char prenom[50];
	char prenomcpy[50];
	char buf[80];

	chdir("../pipe"); //Pour le faire fonctionner sur les autres machines

	/* On demande le nom du client, qui est tu ?*/
	printf("Bonjour bienvenu de le jeu Qui est-ce ?\n");
	printf("Quel est votre Prenom?\n");
	scanf("%s", prenom);
	
	//strcpy(prenomcpy,prenom); //artifice sinon on coupe les deux premier caractères du tableau ???

	/* penser à ajouter main entre ""*/
    	descW=open("main",O_WRONLY); // on ouvre le pipe main en ecriture
    	write(descW,prenom,20); // on ecrit le nom du nouveau client
    
	close(descW); // on ferme le descripteur
	sleep(1);

    	descR=open(prenom,O_RDONLY); // on ouvre le pipe main en ecriture
	nb=read(descR,buf,80);
	buf[nb]='\0';
	printf("Retour serveur: %s\n",buf);
	sleep(30);
	close(descR);
}

void menu()
{
    int i = 0 ;
    char chaine_recherche[20] = "";


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
            case 0: break;
            case 1:
                i = 0;

                do{
                    //menu des caracteristiques
                    printf ("|  1 | couleur des yeux\n");
                    printf ("|  2 | couleur des cheveux\n");
                    printf ("|  3 | regularite à l'apéro\n");

                    scanf("%d",&i);
                    switch(i)
                    {
                        case 0: break;
                        case 1:
                            i = 0 ;

                            do{
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
                                        break;
                                }
                                i =0;
                            }while (i != 0);
                            break;
                        case 2:
                            i = 0 ;

                            do{
                                //menu des cheveux
                                printf ("|  1 | chatain\n");
                                printf ("|  2 | grisonant\n");
                                printf ("|  3 | brun\n");
                                printf ("|  4 | blond\n");
                                printf ("|  5 | boucle\n");

                                scanf("%d",&i);
                                switch(i)
                                {
                                    case 0: break;
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
                                        break;
                                }
                                i =0;
                            }while (i != 0);
                            break;
                        case 3:
                            i = 0 ;

                            do{
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
                                        break;
                                }
                                i =0;
                            }while (i != 0);
                            break;
                        default:
                            break;
                    }
                }while (i != 0);

                break;
            case 2:
                printf("Quel est votre Prenom?\n");
                scanf("%s", chaine_recherche);
                break;
            case 3:
                break;
            default:
                break;
        }
    }while (i != 0);
}

