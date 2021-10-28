**Propriétaire du document: Florent VILLENEUVE**

Avant de définir l'architecture il faut définir les fonctionnalités à implémenter et pour cela il faut étudier et comprendre le besoin client les mieux possible. Faisons la liste des fonctionnalités les plus basiques à prendre en compte.

# Expression des besoins / Cahier des charges:

Le but du projet et d'implémenter un "Qui est-ce ?" compétitif en ligne de commande qui permettra à la classe de fraterniser joyeusement durant les pauses.
Notre implémentation du jeu consistera à ce que chaque élève affronte l'ordinateur et devine le plus vite possible le personnage de celui-ci. Un classement sera effectué selon le temps mis par la classe et un gagnant pourra être désigné ainsi. L'interface étant purement textuel il faudra que les personnages utilisés soient connus de tous, nous prendront donc les élèves de la classe.

Le programme pourra éventuellement évoluer et proposer d'autres mode de jeu (utilisateur vs utilisateur par exemple).

## Exigences fonctionnelles

Pour le mode de jeu de base décrit plus haut:
- Un élève sera choisit aléatoirement parmi la base de données
- La liste des peronnages sera affiché à l'écran de l'utilisateur comme un vrai joueur devant son plateau de jeu
- Les élèves pourront poser des questions à l'ordinateur/au serveur. *Exemple: "Le personnage a t-il les yeux bleus ?"* Ces choix ne seront pas tapés à la main mais choisie à l'aide d'un menu
- Le serveur devra répondre par oui ou par non et mettre à jour la liste des personnages à l'écran en effaçant ceux ne correspondant pas à la description
- Ce cycle de question réponse se poursuit jusqu'à ce que le joueur trouve le personnage
- Le premier joueur qui gagne déclenche une explosion de saveur sous la forme d'un **STM32 qui fait bip bip**

## Exigences non-fonctionnelles

On priviligera la performance et l'économie des ressources.

```diff
-La sécurité du produit final à voir  -> Julien TERRIER notre amour de sys admin 

```

# Specifications fonctionnelles 



 


