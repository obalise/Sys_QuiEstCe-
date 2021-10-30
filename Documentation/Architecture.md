**Propriétaire du document: Florent VILLENEUVE**

Avant de définir l'architecture il faut définir les fonctionnalités à implémenter et pour cela il faut étudier et comprendre le besoin client le mieux possible. Faisons la liste des fonctionnalités les plus basiques à prendre en compte.

# Expression des besoins / Cahier des charges:

Le but du projet et d'implémenter un "Qui est-ce ?" compétitif en ligne de commande qui permettra à la classe de fraterniser joyeusement durant les pauses.
Notre implémentation du jeu consistera à ce que chaque élève affronte l'ordinateur et devine le plus vite possible le personnage de celui-ci. Un classement sera effectué selon le temps mis par la classe et un gagnant pourra être désigné ainsi. L'interface étant purement textuel il faudra que les personnages utilisés soient connus de tous, nous prendrons donc les élèves de la classe.

Le programme pourra éventuellement évoluer et proposer d'autres modes de jeu (utilisateur vs utilisateur par exemple).

## Les attentes générales

Pour le mode de jeu de base décrit plus haut:
- Un élève sera choisi aléatoirement parmi la base de données
- La liste des personnages sera affiché à l'écran de l'utilisateur comme un vrai joueur devant son plateau de jeu
- Les élèves pourront poser des questions à l'ordinateur/au serveur. *Exemple: "Le personnage a-t-il les yeux bleus ?"* Ces choix ne seront pas tapés à la main mais choisie à l'aide d'un menu
- Le serveur devra répondre par oui ou par non et mettre à jour la liste des personnages à l'écran en effaçant ceux ne correspondant pas à la description
- Ce cycle de question réponse se poursuit jusqu'à ce que le joueur trouve le personnage
- Le premier joueur qui gagne déclenche une explosion de saveur sous la forme d'un **STM32 qui fait bip bip**

## Exigences non-fonctionnelles

On privilégiera la performance et l'économie des ressources. La vitesse d'execution n'est pas un critére primordial de notre application, il est simplement attendu que le joueur n'ait pas à attendre plus de quelques secondes les réponses du serveurs.

```diff
-La sécurité du produit final à voir, je sais pas si y'a besoin avec SSH  -> Julien TERRIER notre amour de sys admin 
```

# Spécifications fonctionnelles 


## Fonctionnalités obligatoires

- Les utilisateurs peuvent à partir d'un terminal se connecter à un serveur avec un identifiant et un mot de passe
- Mode de jeu "un seul joueur"
- Mode de jeu "multijoueur" avec toute la classe
```diff 
-asynchrone ou pas, les joueurs attendent t-il dans un lobby que la partie se lance pour tout le monde ? Comme pour le Kahoot ? Ou alors c'est en différé
```
- Système d'élimination des personnages du "Qui est-ce ?"
- Avoir un classement en mode "multijoueur" 
- 


## Fonctionnalités secondaires

- Lors du jeu, pouvoir choisir un personnage parmi ceux restant, si faux cela viendrait avec une pénalité de points (ou de temps plutôt)
- Mode de jeu Vs entre deux joueurs, donc avec la possibilité de choisir son propre personnage
- Pouvoir modifier la base de données
- Pouvoir changer la base de données pour avoir d'autres personnages avec d'autres caractéristiques. *Exemple: perso de Street fighter*
- Avoir une GUI 
```diff
Elle devra passer par SSH, il faudra surement demander alors à l'utilisateur de télecharger un logiciel comme PuTTY
```
- Ajout de statistiques



# Conception Préliminaire

## Choix de l'architecture

Nous travaillerons avec une architecture [Serveur-Client](https://fr.wikipedia.org/wiki/Client-serveur). 

![image](https://user-images.githubusercontent.com/57157757/139318258-5e23cab2-f699-42d8-bf76-bb7acc798835.png)

La communication entre Serveur et Client se fera à l'aide de "pipes". En effet, ces deux entités seront toutes deux sur la même machine (un Raspeberry pie) et nous n'aurons pas besoin de nous reposer sur l'utilisation de sockets qui est plus complexe. 

Afin que les utilisateurs puissent accèder à distance à la partie Client, nous nous servirons du protocole de communication sécurisée [SSH](https://fr.wikipedia.org/wiki/Secure_Shell). 

## Découpage du travail en module 

Cela permettra aux développeurs de travailler en parallèles.


# Conception Détaillée

## Mise en forme de la base de données

La base de données sera un unique fichier texte 


 


