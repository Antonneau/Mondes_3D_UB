# Mondes 3D Rapport
### TD2 : Maillage Triangulaire, Antonin AYOT

## I - Bilan

Le calcul d'intersection et le BVH sont fonctionnels.

### Calcul d'intersection

Cette 1ère partie nous demandait d'implémenter la méthode de Möller-Trumbore pour l'intersection de faces dans un maillage triangulaire. La formule utilisée dans la fonction `Mesh::intersectFace` est la suivante :

$$ \begin{bmatrix}t \\ u \\v \end{bmatrix} = \frac{1}{P \cdot E_1} \begin{bmatrix} Q\cdot E_2 \\ P \cdot T \\Q \cdot D \end{bmatrix} $$

Maintenant qu'on a nos coordonnées barycentrique $(u, v)$, il faut vérifier certaines conditions :
	- Il faut que $u$ soit compris entre $[0, 1]$,
	- De même pour $v$,
	- Il faut que la somme de $u$ et $v$ sont inférieur à 1 (sinon on est à l'extérieur du triangle),
	- Il faut que $t$ soit positif.

Si toutes ces conditions sont vérifiées, on est dans le repère barycentrique. On peut commencer par calculer la normale avec la normalisation de la formule $E_1 \cdot E_2$ . Il n'y a plus qu'à définir les attributs du `Hit`, étant donné qu'on a $t$, la normale, la forme (`this`) ainsi que les textures. Cela nous donne le résultat suivant :

![tw](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw.png?raw=true)

Nous pouvons améliorer le rendu en "lissant" le bord des triangles. Il suffit de changer la valeur de la normale par $u(A - C) + v(B - C) + C$ avec A, B et C correspondant aux faces du triangle intersecté. Voici le nouveau résultat (le 1er avec `tw503.obj` et le 2ème avec `tw.obj`).

![tw2](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw3.png?raw=true)
![tw2time](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw3_time.png?raw=true)


![tw3](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw2.png?raw=true)
![tw3time](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw2_time.png?raw=true)

### BVH

La 2ème partie consistait à implémenter une BVH. Pour cela, penchons nous sur le fichier `bvh.cpp`. Trois fonctions étaient à implémenter :

- `BVH::buildNode` qui se charge de la construction des noeuds en fonction de la boite englobante. Par récursion, elle attribue à l'arbre la valeur des noeuds et des feuilles.
La fonction commence d'abord par construire la boite du noeud `Eigen::AlignedBox3f box`, qui est la boite contenant les triangles du maillage allant de l'indice `start` jusqu'à `end`. On inclut dans la boite (par le biais de la fonction `box.extend()`) toutes les faces de chaque triangles compris dans l'intervalle d'indices. On attribue ensuite la boite au noeud, ainsi que le nombre de faces dans le noeud (champ `nb_faces`). On vérifie ensuite si le noeud actuel est une feuille. Si le niveau de récursion atteint la profondeur maximale ou si le nombre de faces est inférieur au nombre de faces désiré `targetCellSize`, le noeud est une feuille et on attribue son champ `first_face_id` par `start` et la valeur de `is_leaf` à `true`.
On cherche désormais l'axe de la boite la plus large pour effectuer la coupe. On attribue la valeur de l'axe dans la variable `axisToCut` ainsi que le milieu de la boite selon l'axe dans `axisValue` (étant donné qu'on coupe selon le milieu). Pour savoir quel dimension a la plus grande largeur, on compare les coordonnées de la boite `box.min` et `box.max`.
Une fois ces informations récupérées, on peut effectuer la coupe grâce à un appel à la fonction `split` avec les arguments `(start, end, axisToCut, axisValue)`.
On regarde si la valeur retournée par `split` n'est ni `start`, ni `end`. Si c'est le cas, la récursion est inutile : la coupe n'a pas été faite, le noeud est une feuille. On stoppe la récursion pour un gain de temps.
On s'est assuré que `node` est bien un noeud. On alloue donc ses fils avec `m_nodes.resize` et on attribue au noeud l'indice de son fils gauche `first_child_id`. On peut appeler la récursion sur les fils gauche et droit, en faisant attention à bien attribuer les valeurs `start` et `end` et en incrémentant la valeur de `level` de 1.
- `BVH::intersect` est l'initialisation de l'intersection dans la boite. Appelée dans `Mesh::intersect`, elle commence par appeler directement `BVH::intersectNode` sur la racine de l'arbre. Aucune condition de vérification n'est à faire, la seule possible étant déjà faite avant l'appel dans `Mesh::intersect`.
- `BVH::intersectNode` vérifie l'intersection sur chaque noeuds de l'arbre. On récupère le noeud à la position `nodeId` pour commencer à comparer. Si c'est une feuille (`node.is_leaf == true`) on regarde si il y a des intersections avec les faces comprises dans le noeud avec `Mesh::intersectFace`. Si il y a intersection, on compare la distance entre l'ancien `hit` et le nouveau `hit`. Si le nouveau est plus proche de la caméra, on écrase l'ancien `hit`.
Ceci est pour le cas de la feuille. Pour le noeud, il faut tester tous les cas possible pour la boite de gauche (fils gauche) et la boite de droite (fils droit).
-- Si aucune des deux boites n'est touchée, on arrête la fonction.
-- Si seule la boite de gauche est touchée, on appelle récursivement `BVH::intersectNode` avec le fils gauche seulement,
-- De même pour la boite de droite (appel avec le fils droit),
-- Si les deux boites sont touchées en même temps :
On appelle récursivement la fonction avec le fils de gauche. Si les boites s'entrelacent (condition `tMaxL > tMinR`), si le point d'intersection se trouve dans dans cet entrelacement ou si il n'y a pas eu d'intersection avec le fils de gauche, on effectue la récursion cette fois-ci avec le fils de droite.

Si une de ces conditions manquent ou est mal respectée, cela peut donner le type de bug suivant : 

![tw_intersect](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/tw_intersect.png?raw=true)

Cependant, si ces trois fonctions sont bien implémentées, nous obtenons un gain de temps non négligeable pour les rendus :

![killeroo](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/killeroo.png?raw=true)
![killeroo_time](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/killeroo_time.png?raw=true)

![killerooGlass](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/killerooGlass.png?raw=true)
![killerooGlass_time](https://github.com/Antonneau/Mondes_3D_UB/blob/INSTABLE/TD2/killerooGlass_time.png?raw=true)


## II . Points délicats

Le point m'ayant posé le plus problème est la partie sur le BVH en général, la visualisation de l'arbre et des conditions à respecter pour l'intersection dans les boites englobante n'étant pas aisé.
Mes phases de debugage étaient plus long sur l'implémentation de la fonction `BVH::buildNode`, dû aux crashs intervenus en plein milieu d'un rendu par exemple.
