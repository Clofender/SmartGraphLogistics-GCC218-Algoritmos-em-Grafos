#include <iostream>
#include <vector>
#include <tuple>

#include "functions.h"

using namespace std;



int** criarGrafo(int v, int n_aresta_dir, int n_aresta_nao_dir,
const vector<tuple<int, int>>& aresta_dir, const vector<tuple<int, int>>& aresta_nao_dir){

    int** matriz = new int*[v+1];

    for(int i = 1; i<=v; i++){
        for(int j = 1; j<=v; j++){
        matriz[i][j] = 0;
        } 
    }

    for (const auto& tup : aresta_nao_dir) {
        int i, j;
        tie(i, j) = tup;
        if(i <= j && j <= i) {
            matriz[i][j] = 1;
            matriz[i][j] = 1;
        }
    }

    for (const auto& tup : aresta_dir) {
        int i, j;
        tie(i, j) = tup;
        if(i <= j && j <= i) {
            matriz[i][j] = 1;
            matriz[i][j] = 1;
        }
    }

    return matriz;
}

int mostrarVertices(Grafo grafo){
        return grafo.vertices;
}