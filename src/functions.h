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
    int verticesReq;
    int arestasReq;
    int arcosReq;
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