Angélique Delevingne
L2 Informatique
19000955
Projet Pacman
Programmation Graphique – Algorithme et Structure de Donnée
Janvier 2021

    1. Objectif du projet
       
L’objectif de ce projet est de représenter le jeu Pacman. Le jeu Pacman est modélisé par un labyrinthe en 3D en vue isométrique. Le jeu consiste à déplacer Pacman à l’intérieur du labyrinthe afin de lui faire manger toutes les Pac-gommes qui s’y trouvent et cela en évitant d’être touché par les fantômes. Les super pac-gommes permettent au Pacman de manger les fantômes durant quelques secondes.

    2. Comment utiliser le programme
       

Le programme s’ouvre avec une page d’accueil qui explique les règles du jeu. Pour commencer, le jeu il suffit d’appuyer sur la touche d’espace. Le jeu s’affiche, on peut y voir le labyrinthe en vue isométrique avec le Pacman symbolisé par une sphère jaune. Les Pac-gommes sont en sphère verte. Les super Pac-gommes sont en sphère rouge. Les fantômes sont en sphère ovale violette. Pour déplacer le Pacman, il suffit d’appuyer sur les flèches du clavier, c’est à dire flèche du haut pour monter, du bas pour descendre, celle de droite pour se déplacer à droite et celle de gauche pour se déplacer à gauche.
Le pacman dispose de trois vies.
La couleur des fantômes change en vert s’ils détectent le pacman. Elle change en rouge lors qu’ils peuvent être mangés durant leurs parcours.

    3. Comment installer
       
Le programme a été développé en langage C sous Ubuntu. J’ai utilisé la libraire Opengl / Glut me permettant de représenter en trois dimensions le labyrinthe et les personnages.

Les commandes utilisés sont : 
    • sudo apt-get install freeglut3-dev
    • glxinfo | grep 'version'
    • sudo apt install mesa-utils
    • glxinfo | grep 'version'
    • sudo apt-get install libgl1-mesa-dev build-essential
    • gcc -o pacman main.c -lGL -lGLU -lglut
      
Le code source est disponible sous https://code.up8.edu/adelevingne/pacman


    4. Description de l’algorithme



    A) Les personnages
       
Le personnage joué par l’utilisateur est le pacman. Le pacman est représenté par une shère jaune. Le pacman est défini par une structure. Elle contient ses coordonnées, sa taille, son score, sa vie, son orientation, son pouvoir et le temps de son pouvoir. Le pacman est initialisé dans la fonction pacman_init. Elle permet de positionner le pacman à l’aide de ses multiples paramètres en début du jeu. 

Les fantômes sont des personnages représentés en violet. Les fantômes sont définis par une structure qui contient les coordonnées, la taille et leurs vies. Le fantôme est initialisé dans la fonction ghost_init.

    B) Le Labyrinthe

       Le labyrinthe est un tableau de 11x11, chaque case peu être soit : 
               -un sol (parallélipipède),
       	-un mur (cube), 
       	-un pac-gomme  (sphère),
       	-un super  pac-gomme (sphère) .
       Le labyrinthe est parcouru par les personnages. Un mur ne peut pas être traversé et ne peut pas être escaladé.

    C) Les déplacements du pacman

La fonction keyOperations lit une touche. Lorsqu’il s’agit d’une touche de directions, elle déplace  le pacman dans le labyrinthe si le déplacement est possible. 
 
    D) Les déplacements des fantômes

Chaque fantôme possède un parcours de recherche pré défini. Le fantôme changera de parcours s’il détecte un pacman dans sa vision horizontal ou vertical. Le fantôme changera de couleur et se dirigera vers le pacman là où il a été détecté. Dans le cas où le pacman disparaît de sa vision, le fantôme reprendra son parcours de recherche. 

    E) Fenêtre d’information de l’état du jeu 

Il existe 4 fenêtres possibles :
    • Lancement du jeu : cette fenêtre donne les informations de comment utiliser le jeu,
    • Jeu gagné : cette fenêtre indique que le niveau est terminé et va redémarrer un nouveau niveau,
    • Jeu perdu : cette fenêtre indique que le pacman a perdu toutes ces vies et va relancer une nouvelle partie,
    • Partie perdue : cette fenêtre indique que le pacman a perdu une vie durant la partie. La partie sera relancée avec une vie en moins. Le labyrinthe reste inchangé.

    F) Les visions du jeu

Il y a deux fenêtres : une 3D et une 2D.
La 2D est une projection orthogonale du jeu et la 3D est une projection en perspective.
L’affichage des personnages et du labyrinthe est représenté en 2D et en 3D.
Dans la vue 3D, j’ai placé mon angle de vision afin d’obtenir une vue isométrique.

    G) Algorithme général

L’algorithme est le suivant : 
	-Initialisation du jeu, des personnages, le labyrinthe et le niveau.
	-Dans une boucle infinie faire :
	Si le jeu est suspendu faire :
- Affichage de la fenêtre d’information de l’état du jeu
Si le jeu est démarré faire :
	Pour  l’écran 3D et 2D faire:
	-Affichage du labyrinthe ( le sol, les murs, les pac-gommes et les super pac-	gommes) 
	-Affichage du Pacman 
	-Affichage des fantômes
	-Lecture des touches et calcul du déplacement du pacman
	-Calcul du déplacement des fantômes
	-Temporisation avant de rafraîchir l’affichage

    4. Points à améliorer
       
Pour améliorer le programme, il aurait été bien d’avoir plusieurs labyrinthes et de taille plus grande. De plus, les fantômes devraient s’enfuir si le pacman a un super pouvoir. Donner une intelligence artificielle au fantôme dans la recherche du pacman.



    5. Remarque de travail 
       
J’ai été débordé par les projets en simultanéité ce qui m’a beaucoup fait stresser. Je pense que si les projets n’aurait pas été en simultanéité avec des dates de rendu moins proche, j’aurai pu mieux organiser mon projet. 
Pour mon projet de pacman, je me suis inspiré de : https://github.com/ekdud014/OSS_pacman
J’ai repris la notion des deux vues que j’ai trouvé intéressante et la modélisation des objets.
J’ai essayé de reprendre et de corriger la vue qu’il a utilisé en FPS mais sans succès. C’est pour cela que j’ai préféré faire une vue isométrique.
J’ai trouvé ce projet complexe dans le positionnement de la caméra. Pour mieux placer ma caméra, j’ai du coder une fonction qui me permettait de déplacer les angles de vue grâce à mes touches de clavier. Pour autant, j’ai trouvé enrichissant d’expérimenter la 3D et la 2D pour mon premier jeu. J’ai trouvé amusant de faire un jeu interactif avec un visuel. J’ai pris du plaisir à faire les fantômes intelligents pour manger le pacman.

