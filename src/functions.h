#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <string> 
#include <tuple>
#include <algorithm>

using namespace std;

struct Aresta{
    int from;
    int to;
    int tCost;
    int demand;
    int sCost;
    bool atendido = false;
};
struct Grafo {
    int vertices = 0;
    int arestas = 0;
    int arcos = 0;
    int custo = 0;
    int** matriz = nullptr;
    vector<vector<Aresta>> lista;
    int verticesReq = 0;
    int arestasReq = 0;
    int arcosReq = 0;

    Grafo() = default;
 
    Grafo(int v) : vertices(v) {
        lista = vector<vector<Aresta>>(v + 1);
    }

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
    int num;
    int tCost = 0;
    int demand;
    int sCost;
};

struct Edge {
    string id;
    int from;
    int to;
    int tCost;

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
    Cabecalho cabecalho;
    Grafo grafo;
    vector<RequiredNode> reqNodes;
    vector<Edge> edges;
    vector<RequiredEdge> reqEdges;
    vector<RequiredArc> reqArcs;
    vector<Arc> arcs;
    vector<Aresta> caminhosObrigatorios;
};
struct Rota {
    Cabecalho cab;
    int id;
    int custo_demanda = 0;
    int custo_total;
    int qtd = 0;
    vector<Aresta> arcos; 
    int carga_atual = cab.capacity;
    int ponto_partida = cab.depotNode;
};

int** criarGrafo(Grafo& grafo, const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, const vector<Arc>& arcs);
void criarLista(Grafo& grafo,  const vector<Edge>& edges,
    const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, 
    const vector<Arc>& arcs, const vector<RequiredNode>& reqNodes,
    vector<Aresta>& caminhosObrigatorios);

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
DadosGrafo leituraBat(string l);
void intermed(Grafo grafo);
int componentesConectados(const Grafo& grafo);

void PathScanning(Grafo& grafo, DadosGrafo& dados,  const vector<Edge>& edges,
    const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, 
    const vector<Arc>& arcs,const vector<RequiredNode>& reqNodes, 
    vector<Aresta>& caminhosObrigatorios, unsigned long long com1, int i, string base);
#endif