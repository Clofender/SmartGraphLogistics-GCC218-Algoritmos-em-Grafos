#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "functions.h"

using namespace std;

int INF = 1000000000;

    int** criarGrafo(Grafo& grafo, const vector<RequiredEdge>& reqEdges, 
            const vector<RequiredArc>& reqArcs, const vector<Arc>& arcs) {

        grafo.matriz = new int*[grafo.vertices + 1];
        for (int i = 0; i <= grafo.vertices; i++) {
        grafo.matriz[i] = new int[grafo.vertices + 1];
        }

        for (int i = 1; i <= grafo.vertices; i++) {
        for (int j = 1; j <= grafo.vertices; j++) {
        grafo.matriz[i][j] = INF;
        }
        }

        for (const auto& re : reqEdges) {
        grafo.matriz[re.from][re.to] = re.tCost;
        grafo.matriz[re.to][re.from] = re.tCost; 
        }

        for (const auto& ra : reqArcs) {
        grafo.matriz[ra.from][ra.to] = ra.tCost;
        }

        for (const auto& a : arcs) {
        grafo.matriz[a.from][a.to] = a.tCost;
        }

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


void dfsComponentes(int u, const Grafo& grafo, vector<bool>& visitado) {
    visitado[u] = true; 

    for (int v = 1; v <= grafo.vertices; ++v) {
       
        if (grafo.matriz[v]) {
           
          
            if ((grafo.matriz[u][v] < INF || grafo.matriz[v][u] < INF) && !visitado[v]) {
                dfsComponentes(v, grafo, visitado); // Chama recursivamente para o vizinho
            }
        }
    }
}

// Função principal para calcular o número de componentes conectados
int componentesConectados(const Grafo& grafo) {
    if (grafo.vertices <= 0 || !grafo.matriz) { // Verifica se o grafo é válido
        return 0;
    }

    // Vetor para marcar os vértices visitados (índice 0 não usado)
    vector<bool> visitado(grafo.vertices + 1, false);
    int numComponentes = 0;

    // Itera por todos os vértices
    for (int i = 1; i <= grafo.vertices; ++i) {
        // Se o vértice i ainda não foi visitado, inicia DFS a partir dele
        if (!visitado[i]) {
            dfsComponentes(i, grafo, visitado); // Explora todo o componente
            numComponentes++; // Incrementa a contagem de componentes
        }
    }

    return numComponentes; 
}


