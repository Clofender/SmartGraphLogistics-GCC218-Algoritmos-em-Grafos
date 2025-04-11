#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "functions.h"

using namespace std;

int INF = 1000000000;

    int** criarGrafo(Grafo& grafo, const vector<RequiredEdge>& reqEdges, 
            const vector<RequiredArc>& reqArcs, const vector<Arc>& arcs) {
        // Aloca a matriz e atribui para grafo.matriz
        grafo.matriz = new int*[grafo.vertices + 1];
        for (int i = 0; i <= grafo.vertices; i++) {
        grafo.matriz[i] = new int[grafo.vertices + 1];
        }

        // Inicializa com infinito
        for (int i = 1; i <= grafo.vertices; i++) {
        for (int j = 1; j <= grafo.vertices; j++) {
        grafo.matriz[i][j] = INF;
        }
        }

        // Preenche as arestas requeridas (não direcionadas)
        for (const auto& re : reqEdges) {
        grafo.matriz[re.from][re.to] = re.tCost;
        grafo.matriz[re.to][re.from] = re.tCost; // Aresta bidirecional
        }

        // Preenche os arcos requeridos (direcionados)
        for (const auto& ra : reqArcs) {
        grafo.matriz[ra.from][ra.to] = ra.tCost;
        }

        // Preenche os arcos não requeridos (direcionados)
        for (const auto& a : arcs) {
        grafo.matriz[a.from][a.to] = a.tCost;
        }

        // Exibe a matriz usando o atributo grafo.matriz

        return grafo.matriz;
        }


int mostrarVertices(Grafo grafo) {
    cout << "Quantidade de vértices: " << grafo.vertices << endl;
    return grafo.vertices;
}
int mostrarArestas(Grafo grafo) {
    cout << "Quantidade de arestas: " << grafo.arestas << endl;
    return grafo.arestas;
}
int mostrarArcos(Grafo grafo) {
    cout << "Quantidade de arcos: " << grafo.arcos << endl;
    return grafo.arcos;
}
int mostrarVerticesReq(Grafo grafo) {
    cout << "Quantidade de vertices requisitados: " << grafo.verticesReq << endl;
    return grafo.verticesReq;
}
int mostrarArestasReq(Grafo grafo) {
    cout << "Quantidade de arestas requisitadas: " << grafo.arestasReq << endl;
    return grafo.arestasReq;
}
int mostrarArcosReq(Grafo grafo) {
    cout << "Quantidade de arcos requisitados: " << grafo.arcosReq << endl;
    return grafo.arcosReq;
}

int grauMin(Grafo grafo){
    return grafo.vertices * (grafo.vertices-1);
}

double densidadeGrafo(const Grafo& grafo) {
    return static_cast<double>(grafo.arcos + (grafo.arestas*2)) / (grafo.vertices * (grafo.vertices - 1));
}
void FloydWarshall(Grafo grafo){
    for (int k = 1; k <= grafo.vertices; k++) {
        
        for (int i = 1; i <= grafo.vertices; i++) {
            for (int j = 1; j <= grafo.vertices; j++) {
                
                if (grafo.matriz[i][k] < INF && grafo.matriz[k][j] < INF) {
                    if (grafo.matriz[i][j] > grafo.matriz[i][k] + grafo.matriz[k][j]) {
                        grafo.matriz[i][j] = grafo.matriz[i][k] + grafo.matriz[k][j];
                    }
                }
            }
        }
    }
}


float caminhoMedio(Grafo grafo){ 
    FloydWarshall(grafo);
    float somaDist = 0;
    int cont = 0;
    for(int i = 1; i <= grafo.vertices; i++){
       for(int j = 1; j <= grafo.vertices; j++){
            if(grafo.matriz[i][j]< INF && i != j){
                somaDist = somaDist + grafo.matriz[i][j];
                cont++;
            }
        }
    }
    return somaDist/cont;
}

int diametro(Grafo grafo) {
    FloydWarshall(grafo);
    int diam = 0;

    for (int i = 1; i <= grafo.vertices; i++) {
        for (int j = 1; j <= grafo.vertices; j++) {
            if (i != j && grafo.matriz[i][j] < INF) {
                diam = max(diam, grafo.matriz[i][j]);
            }
        }
    }

    return diam;
}

pair<int, int> calculaGrau(const Grafo& grafo, 
    const vector<RequiredEdge>& reqEdges, 
    const vector<RequiredArc>& reqArcs, 
    const vector<Arc>& arcs) 
{
    vector<int> grau(grafo.vertices + 1, 0);

    for (int i = 0; i < reqEdges.size(); i++) {
        grau[reqEdges[i].from]++; 
        grau[reqEdges[i].to]++;
    }

    for (int i = 0; i < reqArcs.size(); i++) {
        grau[reqArcs[i].from]++;
        grau[reqArcs[i].to]++;
    }

    for (int i = 0; i < arcs.size(); i++) {
        grau[arcs[i].from]++;
        grau[arcs[i].to]++;
    }

    int minGrau = 1000000000; 
    int maxGrau = -1;


    for (int i = 1; i <= grafo.vertices; i++) {
        if (grau[i] < minGrau)
            minGrau = grau[i];
        if (grau[i] > maxGrau)
            maxGrau = grau[i];
    }

    return {minGrau, maxGrau};
}

void intermed(Grafo grafo) {
    FloydWarshall(grafo);
    

    for (int v = 1; v <= grafo.vertices; v++) {
        int count = 0;
        for (int i = 1; i <= grafo.vertices; i++) {
            for (int j = 1; j <= grafo.vertices; j++) {
                if (i != j && i != v && j != v) {
                    if (grafo.matriz[i][v] + grafo.matriz[v][j] == grafo.matriz[i][j]) {
                        count++;
                    }
                }
            }
        }
        cout << "Intermediação do vértice " << v << ": " << count << endl;
    }
}

