# Mondes 3D Rapport
### TD1 : Raytracing, Antonin AYOT

## I - Bilan
Tout a été implémenté et est fonctionnel. Un petit problème persiste cependant pour la réfraction de la lumière (expliqué à la fin de cette partie).

### Rayons primaires

Le début du TD nous demandait d'implémenter la fonction `Scene::Intersect`, qui récupère les informations de la forme intersecté si un rayon touche la forme en question. L'algorithme consiste en une boucle sur le nombre d'objets dans la scène. Si il y a intersection entre le rayon et la forme de la boucle, on compare la distance entre la caméra et le point intersecté. Cette comparaison est importante pour les objets ce faisant traverser plusieurs fois (les sphères par exemple). On récupère le point le plus proche de la caméra. Si l'objet se situe derrière, on ne récupère pas les informations.
Il nous a fallu ensuite faire cette même méthode `intersect` pour les classes filles `Sphere` et `Plane`. Pour la sphère, il s'agit d'une équation du second degré, de la forme : $ax² + bx + c$ avec $a = ||d||²$, $b = 2<d.(o-c)>$ et $c = ||o-c||² - r²$. Une fois l'équation obtenu, on calcule son discriminant : Si delta < 0, l'objet est derrière la caméra et donc il n'est pas nécessaire de récupérer quoi que ce soit. Si delta = 0, le rayon est tangent à la sphère, on a juste à récupérer la racine et à la mettre dans la variable `Hit`. En revanche si delta > 0, le rayon traverse la sphère. Il faut calculer les deux racines et récupérer celle la plus petite étant positive. Ces racines correspondent à l'attribut `t` de la classe `hit`, qui est le temps mit au rayon pour toucher la forme.
Concernant les plans, il s'agit d'une simple opération : $t = <(c-o).n> / <d.n>$. Si t est positif, on récupère les données correspondantes.
Néanmoins, toutes ces implémentations ne sont pas utile sans l'affichage qui se fait via la fonction `render` du fichier `main.cpp`. Pour l'affichage, on effectue deux boucles imbriqués l'une dans l'autre (une pour les lignes, l'autre pour les colonnes). On trace le rayon du pixel correspondant, qui prend comme vecteur : $pixDir = camF + 2(x/camWidth - 0,5)*camX + 2(y/camHeight - 0,5)*camY$. On récupère la couleur du pixel grâce à la méthode `Li`, puis on affiche la couleur du pixel à son bon emplacement.
Pour finir, il nous faut récupérer la couleur de la forme (la méthode `Li`). Il s'agit juste d'une condition `if` : si le rayon touche une forme, on récupère la couleur diffuse de celle-ci. Sinon on récupère la couleur du fond.

Cela nous donne les résultats suivants :

![troisSpheres](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/troisspheres.png?raw=true)
![petanque](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/petanque.png?raw=true)

### Eclairage local

Pour effectuer un calcul d'éclairage local, il nous faut récupérer les normales des formes, et les mettre dans l'attribut `normal` de la classe `Hit`.

Pour un plan, on a déjà un attribut `m_normal` en entrée. Il suffit simplement de le mettre dans la classe `Hit`.
Pour une sphère, il faut calculer la distance qui sépare le centre de celle-ci avec le point d'intersection, avec la formule $normale = (o + dt) - c$. Une fois normalisé, on l'attribut à la classe `Hit`. Ces calculs se font dans les fichiers des classes respectives.
Ensuite, l'affichage ci-contre se fait en donnant à la couleur renvoyé par la méthode `Li` les coordonnées de la normale (rouge = x, vert = y, bleu = z). Il a fallu créer un fichier pour l'occasion, nommé `normals.cpp` (intégrateur `normals`). 

Cela nous donne le résultat suivant :


![troisspheres1](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/troisspheres1.png?raw=true)

Ces normales vont nous servir au calcul de l'éclairage local. On calcule le BRDF de la méthode de Phong dans la méthode `Material::brdf`. Deux attributs et une méthode vont nous servir à calculer ce modèle : `m_specularColor` pour la couleur spéculaire, `m_exponent` pour la brillance de l'objet et `Material::diffuseColor(uv)` pour récupérer la couleur diffuse.

Une fois cette méthode implémenté, il faut définir l'intensité lumineuse et la direction de la lumière pour le cas d'une source ponctuelle (la source unidirectionnelle étant déjà implémentée). La direction est juste la différence de la position de la source avec le point incident. L'intensité se fait par le calcul $Vx = I / ||d||$` avec d la direction de la lumière et I l'intensité de la source.

Un nouvel intégrateur a dû être ajouté pour tester le modèle de Phong : il s'agit de l'intégrateur `direct` (fichier `direct.cpp`). Cette fois-ci, on doit calculer la couleur en fonction de l'éclairage, on doit donc parcourir la liste des lumière de la scène. Le pixel sera une somme de tous les résultats des BRDF selon les lumières de la scène.

Voici le résultat sur la scène `troisSpheres.scn` avec l'intégrateur `direct` :

![phongreal](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/phongreal.png?raw=true)

### Rayons secondaires

Le calcul des ombres portées se fait après le calcul du BRDF de Phong. Il s'agit de tracer un rayon qui démarre au point d'intersection de la lumière avec l'objet en question. Si un autre objet est intersecté, il sera caché par la lumière et donc par conséquent le pixel aura une couleur noire (étant donné qu'il ne s'agit pas d'un éclairage global).

![ombres](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/ombres.png?raw=true)


Un nouvel intégrateur a été ajouté : l'intégrateur `whitted`, qui rajoute les effets de réflexion et de réfraction des rayons.
Ici, il s'agit du cas de la réflexion. Il faut récupérer le nombre maximal de rebond depuis le fichier `.scn` grâce à la méthode `getInteger` (de multiples variants sont disponibles), qu'on attribut à une variable de classe. Grâce à l'attribut `recursionLevel` de la classe `Ray`, on peut faire une comparaison entre celui-ci et la variable de classe `maxRecursion`. Tant que le rayon est réfléchi, on trace le nouveau rayon réfléchi, puis on appelle récursivement la fonction `Li` avec ce nouveau rayon. On multiplie ce résultat par la réflexivité du matériau (attribut de classe) et par le produit scalaire de la direction du rayon par la normale de la forme.

![reflect1](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/reflect1.png?raw=true)
![petanquereflect](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/petanquereflect.png?raw=true)

### Bonus

Il a fallu modifier le fichier `ray.h` pour ajouter un nouvel attribut : il s'agit du vecteur 2D `uv` qui contient la UV map de l'objet (la texture donc). A chaque forme, il y a une méthode pour récupérer cette UV Map.
Forme Sphérique : la UV Map aura comme valeur pour $u = 0,5 + atan(x/y) / 2\pi$ et $v = 0,5 - asin(z/r) / \pi$, avec x, y et z les coordonnées de la normale de la sphère et r son rayon.
Forme Planaire : les valeurs seront $u = o + dx$ et $v = o + dy$.
Une fois ces UV Maps récupérées, on les place dans l'attribut `uv` de la classe `Hit`.

Les résultats sont les suivants :

![texture](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/texture.png?raw=true)

![texture1](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/texture1.png?raw=true)

![texture2](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/texture2.png?raw=true)

![texture4](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/texture4.png?raw=true)

Concernant la réfraction, l'ajout de nouveaux attributs et méthodes dans la classe `Material` étaient nécessaire au calcul de la formule de `Snell-Descartes`. Il y a aussi les méthodes permettant de récupérer les attributs depuis le fichier scène dans le constructeur de `Phong`.

Le calcul de la réfraction se fait dans la condition `if(recursionLevel < maxRecursion)`, que j'ai dû changer pour l'occasion. Si le matériau de l'objet intersecté réfracte, on calcule la formule de Snell-Descartes, qui change si  $\cos\theta1 \lt 0$, la formule changera en conséquence (on prendra l'opposé de la normale et on permutera $n_A$ et $n_B$). On calcule le rayon réfracté, puis on rappelle la fonction `Li` récursivement en multipliant le résultat cette fois-ci par la transmissibilité du matériau.

![refract](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/refract.png?raw=true)

A ce stade là, un problème persiste : même si le matériau est "transparent", l'ombre reste pleine. Pour corriger cela, on ajoute un autre coefficient lors du calcul de l'ombre portée, c'est le coefficient `alpha`, qui est le produit de $0.5$ par la moyenne des coordonnées de la transmissibilité.

![refract1](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/refract1.png?raw=true)

Et enfin, pour obtenir une sphère transparente, il suffit de modifier les valeurs de `etaA` et `etaB` afin qu'ils aient les même valeurs. Voici le résultat :

![refract2](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/refract2.png?raw=true)

Un problème persiste, l'effet "bulle" (image ci-dessus) ne réfléchit pas correctement par rapport au résultat du TD. Les bords ne sont pas corrects.

## II . Points délicats

L'un des points les plus délicats de ce TD est la visualisations de vecteurs et le calcul de formules, notamment au niveau de la normalisation. Étant peu familier avec les mathématiques, trouver la formule du second degré permettant d'avoir le point d'intersection de la sphère par exemple était compliqué. De plus, la normalisation des vecteurs était nécessaire sur presque chacun des calculs, chose que j'oubliais souvent. De même avec certaines variables, dont le signe des valeurs importait (il m'a fallu quelques jours pour comprendre qu'il fallait donner l'opposé du vecteur de direction de la lumière à la fonction BRDF pour un bon fonctionnement).

La récursion pour la réflexion de la lumière m'a aussi posé problème, surtout pour la visualisation du nouveau rayon à calculer. Un des bugs illustré ci-dessous montre un problème de choix de variables obtenu lors d'une session de TD : 

![reflectffail](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/reflectffail.png?raw=true)

L'implémentation du bonus de la réfraction était délicat, non seulement pour la formule à implémenter, mais aussi en matière de programmation, notamment lors de l'ajout de méthode dans la classe `Phong` qui nécessitait de donner des primitives à la classe `Material` le mot clé `virtual` pour appeler les méthodes. Le bug ci-dessous est un bug qui intervient lorsque la normale n'est pas inversée si $\cos\theta1 \lt 0$:

![deuxSpheres](https://github.com/Antonneau/Mondes_3D_UB/blob/master/TD1/imgs/deuxSpheres.png?raw=true)
