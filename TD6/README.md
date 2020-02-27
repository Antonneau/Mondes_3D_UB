# Mondes 3D Rapport
### TD6 : Textures, Antonin AYOT

## I - Bilan

Tout le TD a été implémenté et est fonctionnel.

Deux fragment shaders sont à disposition :
- "simple.frag", qui n'affiche qu'une seule texture sur l'objet correspondant,
- "earth.frag", qui affiche la Terre et ses multiples textures. Pour afficher les textures correctement, du code est à décommenter dans la fonction `Viewer::init()` et dans `Viewer::drawScene()`. De plus, dans le chargement des shaders dans `Viewer::loadShaders()`, il faut charger le bon fragment shader.

Voici la configuration des touches pour le filtrage des textures :

- Magnification :
    - N : Filtrage plus proche voisin (`GL_NEAREST`)
    - L : Filtrage linéaire (`GL_LINEAR`)
- Minification : 
    - PAGE_UP : Filtrage plus proche voisin (`GL_NEAREST`)
    - PAGE_DOWN : Filtrage linéaire (`GL_LINEAR`)
    - Haut : Interpolation des pixels plus proche voisin avec interpolation des niveaux plus proche voisin (`GL_NEAREST_MIPMAP_NEAREST`)
    - Gauche : Interpolation des pixels plus proche voisin avec interpolation des niveaux linéaire (`GL_NEAREST_MIPMAP_LINEAR`)
    - Bas : Interpolation des pixels linéaire avec interpolation des niveaux plus proche voisin (`GL_LINEAR_MIPMAP_NEAREST`)
    - Droite : Interpolation des pixels linéaire avec interpolation des niveaux linéaire (`GL_LINEAR_MIPMAP_LINEAR`)

### Introduction

L'affichage des coordonnées de textures se fait par un simple passage de ceux-ci en tant que variable "in". Un passage des cordonnées de textures à la couleur du modèle donne le résultat suivant : 

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/tex_coord.png?raw=true)

### Placage de textures

Le chargement de textures se fait dans la fonction `Viewer::init()` par le biais de la fonction `SOIL_load_OGL_texture` de la bibliothèque SOIL2. Cette fonction renvoie l'indice de la texture à passer dans le `sampler2D` du fragment shader. Une fois la texture chargée, on la passe dans le fragment shader par un appel à la fonction `glUniform1i()` étant donné que c'est un indice (il faut bien évidemment activer la texture avant et dire au shader que c'est une texture 2D). 

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/earth_texture.png?raw=true)

Pour rajouter de la lumière, il suffit de rajouter un `cos(theta)` et le multiplier par la couleur de la texture à la même manière qu'un Blinn-Phong.

Ensuite, il a fallu plaquer une autre texture pour rajouter une atmosphère à la Terre. De la même manière que précedemment, on charge une autre texture et on la donne en tant que `uniform sampler2D tex2D_2`. Pour un affichage mixé, on utilise la fonction OpenGL `mix()` dans le fragment shader avec les deux textures en argument et en interpolant avec la texture des nuages.

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/cloud_texture.png?raw=true)

La dernière étape et de rajouter dans la partie non-éclairée de la Terre une texture de nuit. On la charge, puis on la mixe dans le fragment shader avec les nuages. Pour un rendu plus "réaliste", on interpole avec la texture des nuages multiplié par theta (pour l'affichage dans la partie nuit) moins 0.1 pour que les nuages soient sombre.

Un dernier appel à la fonction `mix()` est à faire, pour la partie de nuit et la partie de jour. On interpole le tout avec theta pour que la partie de jour s'affiche dans la partie éclairée et la partie de nuit dans la partie sombre.

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/earth.gif?raw=true)

### Filtrage

La suite de ce TD était d'implémenter un filtrage pour les textures, afin qu'une mipmap soit utilisé si besoin est. 

Dans la classe `Viewer`, deux nouveaux attribus on été rajoutés pour le filtrage :
- _magFilter : filtre de magnification. Pour éviter l'effet d'escalier pour les rendus proche,
- _minFilter : filtre de minification. Pour éviter l'aliasing sur les distances lointaines.

Le sampler est d'abord généré dans la fonction `Viewer::init()`. Ensuite, dans `Viewer::drawScene()`, on définit les paramètres de minification et de magnification du sampler et on le définit pour chaque textures.

Voici des images qui comparent le filtre plus proche voisin (`GL_NEAREST`) avec le filtre linéaire (`GL_LINEAR`) :

- cow.obj

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/cow_nearest.png?raw=true)
![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/cow_linear.png?raw=true)

- plane.obj

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/plane_nearest.png?raw=true)
![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/plane_linear.png?raw=true)

- earth.obj

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/earth_nearest.png?raw=true)
![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/earth_linear.png?raw=true)

La dernière opération à effectuer était pour remarquer la différence entre l'interpolation des niveaux de mipmap. Pour cela, le code à disposition sur le site permet de génerer une texture qui change la couleur de l'objet selon le niveau de mipmap. On remarque donc que pour un filtre plus proche voisin, les fragments prennent directement la couleur du niveau de la mipmap correspondante. En revanche pour l'interpolation linéaire, le fragment cherchera à combiner les couleurs des différents niveaux. En voici un exemple avec le modèle cow.obj :

- Interpolation plus proche voisin :

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/mipmap_nearest.png?raw=true)

- Interpolation linéaire :

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD6/imgs/mipmap_linear.png?raw=true)

## II . Points délicats

Aucun point délicat n'a été noté durant ce TD. Tout s'est fait assez aisement.