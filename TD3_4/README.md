# Mondes 3D Rapport
### TD4/5 : Pipeline Graphique, Antonin AYOT

Le calcul d'éclairage, de la camera et des normales ont été implémentées et sont fonctionnels. Par défaut, le programme se lance avec un système solaire.

L'affichage en fils de fer se fait par la touche `W`.

## I - Bilan

### Passage à la 3D

Après quelques manipulations de matrices avec les fonctions `Eigen::Translation3f` et `Eigen::AngleAxis3f`, une caméra a dû être implémentée afin d'avoir un meilleur affichage dans le repère de la scène.

Il a fallu implémenter la méthode `Camera::LookAt` en effectuant les calculs vus en TD et mettre la matrice résultante dans la variable `_mViewMatrix` de la classe `Camera`. Cependant celà ne suffit pas pour un affichage convenable, il faut de plus faire une opération dans le shader : multiplier la position `vtx_position` par la matrice de l'objet `mat_obj` et la matrice de la camera obtenue `mat_cam`. Le tout multiplié par la matrice de perspective `mat_persp` et placé dans la variable `gl_position` donne un premier résultat suivant :

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD3_4/imgs/lemming_persp.png?raw=true)

La suite de ce TD était de créer un petit système solaire, composé d'un soleil, de la Terre et de la Lune.
Il s'agit d'une série d'appel à `Eigen::Translation3f` et à `Eigen::AngleAxis3f`. Pour la Terre et la Lune, on effectue une 1ère rotation pour tourner autour de l'axe du soleil. On translate ensuite la planète/satellite pour qu'elle puisse orbiter. Afin que la Terre puisse être penchée d'un certain angle, on soustrait à l'axe de rotation l'angle de l'orbite (pour que la Terre puisse être penchée dans le même sens) avant de la pencher de 23 degrès selon l'axe Z et la faire tourner selon l'axe Y. Concernant la Lune, on la translate deux fois pour qu'elle puisse tourner en même temps que la Terre autour du Soleil et qu'elle puisse orbiter autour de la Terre. (le résultat suivant s'est fait après implémentation de l'éclairage)

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD3_4/imgs/solar_system.png?raw=true)

### Calcul de l'éclairage et des normales

Cette partie nous demandait d'implémenter une fonction Blinn-Phong dans les shaders afin d'avoir de la lumière ambiante. Le calcul se fait dans le Fragment Shader pour un meilleur résultat. Tout d'abord, dans le Vertex Shader, on passe le vecteur de vue, de la normale, de la lumière ainsi que de la couleur diffuse (dans ce Shader car on possède toutes les matrices nécessaires pour les calculs).

On peut ensuite faire le calcul dans le Fragment Shader, en précisant la couleur spéculaire et l'exposant (qui est la taille de la tâche spéculaire). Dans la fonction de Blinn-Phong, on calcule le terme diffus `theta` et le terme spéculaire `beta` qu'on multiplie respectivement par la couleur diffuse et la couleur spéculaire. Le résultat obtenu est ensuite multipliée par une intensité I qui est l'intensité de la lumière (optionnel cependant).

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD3_4/imgs/monkey_blinn.png?raw=true)

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD3_4/imgs/monkey_blinn2.png?raw=true)

Cependant ces calculs ne suffisent pas pour les fichiers .off par exemple, qui nécessite un calcul de normales pour chaque sommets du maillage. Pour celà, l'implémentation de la fonction `Mesh::computeNormals()` se fait comme suit :

- On met toutes les normales des sommets à 0
- On effectue une boucle sur les faces. Dans la liste des faces, chaque cellule contient trois indices qui sont les indices des trois sommets qui composent la face. On construit deux vecteurs qui nous permettent de calculer le vecteur normal de la face, qu'on ajoute à chaque normale des sommets.
- On normalise les normales de chaque sommets à l'aide de la fonction `Eigen::normalize()`.

Quelques opérations supplémentaires étaient à faire pour de meilleurs résultats :
- Multiplier le vecteur lumière par la matrice de vue afin d'avoir une lumière fixe sur le plan de la scène et non sur le plan de la camera,
- Normaliser les normales pour que les objets translatés n'aient pas d'éclairage trop forte.

Voici un résultat avec le fichier `lemming.off` :

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD3_4/imgs/lemming_normal.png?raw=true)

## II . Points délicats

La difficulté de ce TD était notamment dû au fait qu'on manquait d'images de rendus pour comparer nos résultats avec ceux des chargés de TD. Ce qui fait que je ne suis pas sûr de mes résultats, qui peuvent avoir du sens mais qui ne peuvent pas forcément avoir lieu d'être.

Par exemple, même après avoir mit la lumière dans le repère monde, la tâche spéculaire peut bouger selon l'emplacement de la caméra. Ce que je trouve étrange, et je ne sais pas si c'est le résultat attendu, alors que le rendu semble être correct.