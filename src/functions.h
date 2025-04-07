#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <tuple>

using namespace std;
struct Grafo{
    int vertices;
    int arestas;
    int arcos;
    int** matriz;
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

int** criarGrafo(
    int v,
    int n_aresta_dir,
    int n_aresta_nao_dir,
    const vector<tuple<int, int>>& aresta_dir,
    const vector<tuple<int, int>>& aresta_nao_dir
);

int mostrarVertices(Grafo grafo);

#endif