#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <tuple>

using namespace std;

int** criarGrafo(
    int v,
    int n_aresta_dir,
    int n_aresta_nao_dir,
    const vector<tuple<int, int>>& aresta_dir,
    const vector<tuple<int, int>>& aresta_nao_dir
);

#endif