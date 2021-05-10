#include "halfedge.h"

bool HalfEdgeGraph::almostEqual(float a, float b)
{
    //return fabs(a - b) <= FLT_EPSILON;
    return fabs(a - b) <= 0.1;
}



void HalfEdgeGraph::svgPathsTohalfedgestructure(QVector<QVector<float>> pathsList)
{

    ///https://cs.stackexchange.com/questions/2450/how-do-i-construct-a-doubly-connected-edge-list-given-a-set-of-line-segments/2516#2516
    ///1. For each endpoint, create a vertex.
    ///
    ///
    // ---------------------- Cria Lista de Vértices-----------------------------
    for (int i = 0; i < pathsList.size(); i++){
        Vertex *vertex1 = new Vertex;
        Vertex *vertex2 = new Vertex;

        vertex1->x = pathsList[i][0];
        vertex1->y = pathsList[i][1];

        vertex2->x = pathsList[i][6];
        vertex2->y = pathsList[i][7];

        vertexList.push_back(vertex1);
        vertexList.push_back(vertex2);
    }

    // ---------------------- Limpa itens duplicados da Lista de Vértices-----------------------------
    std::cout << "# de vertices: " << vertexList.size() << std::endl;

    //int numberofDuplicatedItems;

    for (int j = 0; j < vertexList.size(); j++){

        for (int k = j+1; k < vertexList.size(); k++){

            if (almostEqual(vertexList[j]->x,vertexList[k]->x) && (almostEqual(vertexList[j]->y, vertexList[k]->y))){
                //numberofDuplicatedItems+=1;
                //qDebug () << vertexList[j]->x << "  " << vertexList[k]->x << j  <<" "<< k;
                vertexList.remove(k);
                k--;

            } else {


            }
        }
    }

    std::cout << "# de vertices apos limpeza: " << vertexList.size() << std::endl;

    // ---------------------- Limpa itens duplicados da Lista de Vértices-----------------------------
    ///2. For each input segment, create two half-edges, and assign their tail vertices and twins.
    ///
    //-----------------------Cria Half-edges para as curvas----------------------------------

    for (int i = 0; i < pathsList.size(); i++){

        HalfEdge *he0 = new HalfEdge;
        HalfEdge *he1 = new HalfEdge;

        he0->next = nullptr;
        he0->prev = nullptr;

        he1->next = nullptr;
        he1->prev = nullptr;

        he0->twin = he1;
        he1->twin = he0;

        he0->p0.setX(pathsList[i][0]);
        he0->p0.setY(pathsList[i][1]);
        he0->p1.setX(pathsList[i][2]);
        he0->p1.setY(pathsList[i][3]);
        he0->p2.setX(pathsList[i][4]);
        he0->p2.setY(pathsList[i][5]);
        he0->p3.setX(pathsList[i][6]);
        he0->p3.setY(pathsList[i][7]);

        he1->p0.setX(pathsList[i][6]);
        he1->p0.setY(pathsList[i][7]);
        he1->p1.setX(pathsList[i][4]);
        he1->p1.setY(pathsList[i][5]);
        he1->p2.setX(pathsList[i][2]);
        he1->p2.setY(pathsList[i][3]);
        he1->p3.setX(pathsList[i][0]);
        he1->p3.setY(pathsList[i][1]);

        Vertex *vertex1 = new Vertex;
        Vertex *vertex2 = new Vertex;

        vertex1->x = pathsList[i][0];
        vertex1->y = pathsList[i][1];

        vertex2->x = pathsList[i][6];
        vertex2->y = pathsList[i][7];

        he0->tail = vertex1;
        he1->tail = vertex2;



        halfedgeslist.push_back(he0);
        halfedgeslist.push_back(he1);

    }

    //-----------------------Cria Half-edges para as curvas----------------------------------


    ///3. For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.

    std::vector<HalfEdge *> clockwisehalfedgeslist; //Lista de half-edges whose tail vertex is that endpoint in clockwise order.


    int tjunctions = 0;

    int linhassugestivas = 0;

    for (int i=0; i < vertexList.size(); i++){

        for (int j=0; j < halfedgeslist.size(); j++){
            if (almostEqual(vertexList[i]->x,halfedgeslist[j]->tail->x) && (almostEqual(vertexList[i]->y, halfedgeslist[j]->tail->y))){
                clockwisehalfedgeslist.push_back(halfedgeslist[j]);
            }
        }
        // Uma half-edge começa naquele vértice

        // É ponto da linha sugestiva!!

        if (clockwisehalfedgeslist.size() == 1){
            // clockwisehalfedgeslist[0]->prev =clockwisehalfedgeslist[0];
            //  clockwisehalfedgeslist[0]->twin->next = clockwisehalfedgeslist[0]->twin;

            clockwisehalfedgeslist[0]->prev =clockwisehalfedgeslist[0]->twin;
            clockwisehalfedgeslist[0]->twin->next = clockwisehalfedgeslist[0];
            linhassugestivas +=1;

        }

        // Duas half-edges começam naquele vértice
        if (clockwisehalfedgeslist.size() == 2) {

            clockwisehalfedgeslist[0]->twin->next = clockwisehalfedgeslist[1];
            clockwisehalfedgeslist[1]->twin->next = clockwisehalfedgeslist[0];

            clockwisehalfedgeslist[0]->prev = clockwisehalfedgeslist[1]->twin;
            clockwisehalfedgeslist[1]->prev = clockwisehalfedgeslist[0]->twin;



        }

        // Três half-edges começam naquele vértice

        if (clockwisehalfedgeslist.size() == 3) {

            tjunctions+=1;

            QVector <float> desordenado;
            QVector <float> ordenado;

            float he1atan = atan2 (clockwisehalfedgeslist[0]->twin->tail->y - clockwisehalfedgeslist[0]->tail->y,clockwisehalfedgeslist[0]->twin->tail->x - clockwisehalfedgeslist[0]->tail->x);
            float he2atan = atan2 (clockwisehalfedgeslist[1]->twin->tail->y - clockwisehalfedgeslist[1]->tail->y,clockwisehalfedgeslist[1]->twin->tail->x - clockwisehalfedgeslist[1]->tail->x);
            float he3atan = atan2 (clockwisehalfedgeslist[2]->twin->tail->y - clockwisehalfedgeslist[2]->tail->y,clockwisehalfedgeslist[2]->twin->tail->x - clockwisehalfedgeslist[2]->tail->x);

            desordenado.push_back(he1atan);
            desordenado.push_back(he2atan);
            desordenado.push_back(he3atan);

            ordenado = desordenado;

            std::sort(ordenado.begin(),ordenado.end());

            // qDebug () << ordenado;
            // qDebug () << desordenado;
            // qDebug () << "--";

            ///4. For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
            ///
            ///

            int e1 = desordenado.indexOf(ordenado[0]);
            int e2 = desordenado.indexOf(ordenado[1]);

            clockwisehalfedgeslist[e1]->twin->next = clockwisehalfedgeslist[e2];
            clockwisehalfedgeslist[e2]->prev = clockwisehalfedgeslist[e1]->twin;

            e1 = desordenado.indexOf(ordenado[1]);
            e2 = desordenado.indexOf(ordenado[2]);

            clockwisehalfedgeslist[e1]->twin->next = clockwisehalfedgeslist[e2];
            clockwisehalfedgeslist[e2]->prev = clockwisehalfedgeslist[e1]->twin;

            e1 = desordenado.indexOf(ordenado[2]);
            e2 = desordenado.indexOf(ordenado[0]);

            clockwisehalfedgeslist[e1]->twin->next = clockwisehalfedgeslist[e2];
            clockwisehalfedgeslist[e2]->prev = clockwisehalfedgeslist[e1]->twin;


            //qDebug () << clockwisehalfedgeslist[0]->tail->x << ", " <<  clockwisehalfedgeslist[0]->tail->y << "-->" <<clockwisehalfedgeslist[0]->twin->tail->x << ", " <<  clockwisehalfedgeslist[0]->twin->tail->y << " Angulo:" << he1atan;
            //qDebug () << clockwisehalfedgeslist[1]->tail->x << ", " <<  clockwisehalfedgeslist[1]->tail->y << "-->" <<clockwisehalfedgeslist[1]->twin->tail->x << ", " <<  clockwisehalfedgeslist[1]->twin->tail->y << " Angulo:" << he2atan;
            //qDebug () << clockwisehalfedgeslist[2]->tail->x << ", " <<  clockwisehalfedgeslist[2]->tail->y << "-->" <<clockwisehalfedgeslist[2]->twin->tail->x << ", " <<  clockwisehalfedgeslist[2]->twin->tail->y << " Angulo:" << he3atan;

        }

        if (clockwisehalfedgeslist.size() == 4) {
            qDebug () << "Erro, T-junction com 4 linhas" ;

            qDebug () << clockwisehalfedgeslist[0]->p0;
        }

        clockwisehalfedgeslist.clear();

    }


    qDebug () << "# de Half-Edges " << halfedgeslist.size();
    qDebug () << "# de T-junctions: " << tjunctions;

    qDebug () << "# de Linhas Sugestivas: " << linhassugestivas;

    ///Pick one of the half-edges and assign it as the representative for the endpoint.
    ///


    int halfedgesconectadas =0;
    for (int i = 0; i < halfedgeslist.size(); i++){

        if (halfedgeslist[i]->next == nullptr || halfedgeslist[i]->prev == nullptr){
            qDebug () << "Nao tem prev ou next";
            qDebug () << halfedgeslist[i]->tail->x <<"," <<halfedgeslist[i]->tail->y;

        } else {
            halfedgesconectadas +=1;
        }

    }


    qDebug () << "# de Half-Edges Conectadas:" << halfedgesconectadas;



    qDebug () << " ---------ESTRUTURA DE HALF-EDGES CRIADA--------"    ;


}
//--------------- Separa em ciclos ------------

/// Agora cria uma lista com as half-edges que representam um ciclo
///
///
///

QVector<QPainterPath> HalfEdgeGraph::separaciclos(){

    QVector<QPainterPath> paths;

    while (halfedgeslist.size() != 0){

        HalfEdge* begin = halfedgeslist[halfedgeslist.size()-1];
        HalfEdge* half = begin;

        cyclesList.push_back(half);

        do
        {
            half->numerodociclo = cyclesList.size();

            halfedgeslist.erase(find(halfedgeslist.begin(), halfedgeslist.end(), half));

            half = half->next;

        } while(half != begin);

    }

    qDebug () << "# de ciclos: " << cyclesList.size();

    /// Find the first half-edge of the path. (The end of the path)
    ///
    ///

    for (int i = 0; i < cyclesList.size(); i++) {

        HalfEdge* begin = cyclesList[i];
        HalfEdge* half = begin;


        do
        {

            if (half->prev == half->twin){
                cyclesList[i] = half;
                break;
            }

            half = half->next;

        } while (half != begin);
    }

    /// Eliminate duplicate cycles (Outers vs Borders)
    ///
    ///

    //    for (int i = 0; i < cyclesList.size(); i++) {

    //        HalfEdge* begin = cyclesList[i];
    //        HalfEdge* half = begin;

    //        if (half->cycle != half->twin->cycle){
    //            cyclesList.removeAt(i);
    //        }
    //    }

    /// Extract the path from the cycle
    for (int i = 0; i < cyclesList.size(); i++) {

        HalfEdge* begin = cyclesList[i];
        HalfEdge* half = begin;

        QPainterPath totalPath;

        do
        {

            QPainterPath myPath;

            myPath.moveTo(half->p0);
            myPath.cubicTo(half->p1,half->p2,half->p3);

            totalPath.addPath(myPath);

            if (half->next == half->twin){

                break;
            }

            half = half->next;

        } while (half != begin);

        paths.append(totalPath);

    }

    /// Remove Duplicated Paths (Outer vs Border)
    ///
    ///

    qDebug () << " SVG Paths" << paths.size();
    for (int i = 0 ; i < paths.size(); i++ ) {
        for (int j = i+1; j < paths.size(); j++) {
            if (paths[i].boundingRect() == paths[j].boundingRect()){
                paths.removeAt(i);
                i++;
            }
        }
    }

    qDebug () << " SVG Paths" << paths.size();

    return paths;
}


