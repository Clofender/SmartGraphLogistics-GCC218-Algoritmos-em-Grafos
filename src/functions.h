#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;
struct Grafo{
    int vertices;
    int arestas;
    int arcos;
    int custo;
    int** matriz;
    int verticesReq;
    int arestasReq;
    int arcosReq;
};
struct Cabecalho {
    string name;
    int optimalValue;
    int numVehicles;
    int capacity;
    int depotNode;
    int numNodes;
    int numEdges;
    int numArcs;
    int numReqNodes;
    int numReqEdges;
    int numReqArcs;
};

struct RequiredNode {
    string id;
    int demand;
    int sCost;
};

struct RequiredEdge {
    string id;
    int from;
    int to;
    int tCost;
    int demand;
    int sCost;
};

struct RequiredArc {
    string id;
    int from;
    int to;
    int tCost;
    int demand;
    int sCost;
};

struct Arc {
    string id;
    int from;
    int to;
    int tCost;
};

struct DadosGrafo{
    Grafo grafo;
    vector<RequiredNode> reqNodes;
    vector<RequiredEdge> reqEdges;
    vector<RequiredArc> reqArcs;
    vector<Arc> arcs;
};
int** criarGrafo(Grafo& grafo, const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, const vector<Arc>& arcs);

int mostrarVertices(Grafo grafo);
int mostrarArestas(Grafo grafo);
int mostrarArcos(Grafo grafo);
int mostrarVerticesReq(Grafo grafo);
int mostrarArestasReq(Grafo grafo);
int mostrarArcosReq(Grafo grafo);

double densidadeGrafo(const Grafo& grafo);
float caminhoMedio(Grafo grafo);
int diametro(Grafo grafo);
pair<int, int> calculaGrau(const Grafo& grafo, 
    const vector<RequiredEdge>& reqEdges, 
    const vector<RequiredArc>& reqArcs, 
    const vector<Arc>& arcs); 
DadosGrafo leituraBat();
void intermed(Grafo grafo);
int componentesConectados(const Grafo& grafo);

#endif