
#include "bvh.h"
#include "mesh.h"
#include <iostream>

void BVH::build(const Mesh* pMesh, int targetCellSize, int maxDepth)
{
    // store a pointer to the mesh
    m_pMesh = pMesh;
    // allocate the root node
    m_nodes.resize(1);

    if(m_pMesh->nbFaces() <= targetCellSize) { // only one node
        m_nodes[0].box = pMesh->AABB();
        m_nodes[0].first_face_id = 0;
        m_nodes[0].is_leaf = true;
        m_nodes[0].nb_faces = m_pMesh->nbFaces();
        m_faces.resize(m_pMesh->nbFaces());
        for(int i=0; i<m_pMesh->nbFaces(); ++i)
        {
            m_faces[i] = i;
        }
    }else{
        // reserve space for other nodes to avoid multiple memory reallocations
        m_nodes.reserve( std::min<int>(2<<maxDepth, std::log(m_pMesh->nbFaces()/targetCellSize) ) );

        // compute centroids and initialize the face list
        m_centroids.resize(m_pMesh->nbFaces());
        m_faces.resize(m_pMesh->nbFaces());
        for(int i=0; i<m_pMesh->nbFaces(); ++i)
        {
            m_centroids[i] = (m_pMesh->vertexOfFace(i, 0).position + m_pMesh->vertexOfFace(i, 1).position + m_pMesh->vertexOfFace(i, 2).position)/3.f;
            m_faces[i] = i;
        }

        // recursively build the BVH, starting from the root node and the entire list of faces
        buildNode(0, 0, m_pMesh->nbFaces(), 0, targetCellSize, maxDepth);
    }
}

bool BVH::intersect(const Ray& ray, Hit& hit) const
{
    bool val = intersectNode(0, ray, hit);
    return val;
}

bool BVH::intersectNode(int nodeId, const Ray& ray, Hit& hit) const
{
    Node node = m_nodes[nodeId];

    bool ret = false;
    // Dans le cas d'une feuille, on teste l'interection sur chacunes des faces
    if (node.is_leaf){
        Hit hitTmp;
        int start = node.first_face_id;
        for(int i = start; i < start + node.nb_faces; i++){
            if (m_pMesh->intersectFace(ray, hitTmp, m_faces[i])){
                if (hitTmp.t() < hit.t() && hitTmp.t() > 0){
                    hit.setT(hitTmp.t());
                    hit.setNormal(hitTmp.normal());
                    hit.setShape(hitTmp.shape());
                    hit.setTexcoord(hitTmp.texcoord());
                    ret = true;
                }
            }
        }
    // Dans le cas d'un noeud, tester l'intersection entre les fils
    } else {
        // Tester si il y a intersection entre les boites des fils
        float tMinL, tMaxL, tMinR, tMaxR;
        Normal3f nL, nR;
        bool testL = ::intersect(ray, m_nodes[node.first_child_id].box, tMinL, tMaxL, nL);
        bool testR = ::intersect(ray, m_nodes[node.first_child_id+1].box, tMinR, tMaxR, nR);

        // Si aucune des boites n'est touchée
        if (!testL && !testR){
            return false;
        // Si la boite gauche est touchée
        } else if (testL && !testR){
            ret = intersectNode(node.first_child_id, ray, hit);
        // Si la boite droite est touchée
        } else if (!testL && testR){
            ret = intersectNode(node.first_child_id+1, ray, hit);
        // Si les deux boites sont touchées
        } else {
            ret = intersectNode(node.first_child_id, ray, hit);
            // Si les boites sont entrelacées, que le point d'intersection est dans l'ntrelacement ou la boite gauche est vide
            if(tMaxL > tMinR || hit.t() > tMinR || !ret){
                ret = intersectNode(node.first_child_id +1, ray, hit);
            }
        }
    }

    return ret;
}

/** Sorts the faces with respect to their centroid along the dimension \a dim and spliting value \a split_value.
  * \returns the middle index
  */
int BVH::split(int start, int end, int dim, float split_value)
{
    int l(start), r(end-1);
    while(l<r)
    {
        // find the first on the left
        while(l<end && m_centroids[l](dim) < split_value) ++l;
        while(r>=start && m_centroids[r](dim) >= split_value) --r;
        if(l>r) break;
        std::swap(m_centroids[l], m_centroids[r]);
        std::swap(m_faces[l], m_faces[r]);
        ++l;
        --r;
    }
    return m_centroids[l][dim]<split_value ? l+1 : l;
}

void BVH::buildNode(int nodeId, int start, int end, int level, int targetCellSize, int maxDepth)
{

    Node& node = m_nodes[nodeId];

    // étape 1 : calculer la boite englobante des faces indexées de m_faces[start] à m_faces[end]

    Eigen::AlignedBox3f box;
    for(int i = start; i < end; i++){
        for(int face = 0; face <= 2; face ++){
            box.extend(m_pMesh->vertexOfFace(m_faces[i], face).position);
        }
    }

    node.box = box;
    node.nb_faces = end - start;
    // étape 2 : déterminer si il s'agit d'une feuille (appliquer les critères d'arrêts)
    // Si c'est une feuille, finaliser le noeud et quitter la fonction
    if(level >= maxDepth || end - start < targetCellSize){
        node.first_face_id = start;
        node.is_leaf = true;
        return;
    }
    // Si c'est un noeud interne :
    // étape 3 : calculer l'index de la dimension (x=0, y=1, ou z=2) et la valeur du plan de coupe
    // (on découpe au milieu de la boite selon la plus grande dimension)
    Vector3f coordMin = box.min();
    Vector3f coordMax = box.max();
    int axisToCut = 0;
    float axisValue = (coordMin.x() + coordMax.x()) / 2.0;

    if (coordMax.x() - coordMin.x() < coordMax.y() - coordMin.y() || coordMax.x() - coordMin.x() < coordMax.z() - coordMin.z()){
        if (coordMax.z() - coordMin.z() < coordMax.y() - coordMin.y()){
            axisValue = (coordMin.y() + coordMax.y()) / 2.0;
            axisToCut = 1;
        } else {
            axisValue = (coordMin.z() + coordMax.z()) / 2.0;
            axisToCut = 2;
        }
    }   
    // étape 4 : appeler la fonction split pour trier (partiellement) les faces et vérifier si le split a été utile
    int splitValue = split(start, end, axisToCut, axisValue);
    // Gérer le cas des splits inutiles
    if (splitValue == start || splitValue == end){
        node.first_face_id = start;
        node.is_leaf = true;
        return;
    }
    // étape 5 : allouer les fils, et les construire en appelant buildNode...

    int size = m_nodes.size();
    node.first_child_id = size;
    m_nodes.resize(m_nodes.size() + 2);
    buildNode(size , start, splitValue, level+1, targetCellSize, maxDepth);
    buildNode(size + 1, splitValue, end, level+1, targetCellSize, maxDepth);
}
