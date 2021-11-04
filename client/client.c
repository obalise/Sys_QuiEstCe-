#include<fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


int main(void)
{
    int descW,descR,nb;
	char prenom[50];
	//char prenomcpy[50];
	char buf[80];
	char tableau[30][80];
	char personnageselect[80];

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
	nb=read(descR,buf,20);
	buf[nb]='\0';
	printf("Retour serveur: %s\n",buf);
	
	
	read(descR, tableau, sizeof(char)*30*80);
	
	//Voyons voir avec ce for si le tableau s'est rempli correctement
	    for(int i = 0; i < 30; i++)
    {
        printf("%d -> ", i );
        puts(tableau[i]); 
    }
	
	read(descR, personnageselect, sizeof(char)*80);
	
	printf("Personnage selectionné -> ");
    puts(personnageselect); 
	
	sleep(30);
	close(descR);
}

