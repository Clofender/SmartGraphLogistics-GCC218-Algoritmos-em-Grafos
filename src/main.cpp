#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "functions.h"
using namespace std;

int main(){

    ofstream outFile("saida.txt");

    streambuf* origCoutStreamBuf = cout.rdbuf();

    cout.rdbuf(outFile.rdbuf());

    DadosGrafo dados = leituraBat();

    criarGrafo(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs);

    mostrarVertices(dados.grafo);
    mostrarArestas(dados.grafo);
    mostrarArcos(dados.grafo);
    mostrarVerticesReq(dados.grafo);
    mostrarArestasReq(dados.grafo);
    mostrarArcosReq(dados.grafo);
    cout << "Densidade do grafo: " << densidadeGrafo(dados.grafo) << endl;
    cout << "Caminho médio: " << caminhoMedio(dados.grafo) << endl;
    cout << "Diâmetro: " << diametro(dados.grafo) << endl;

    pair<int, int> grau = calculaGrau(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs);
    cout << "Grau mínimo: " << grau.first << endl;
    cout << "Grau máximo: " << grau.second << endl;

    intermed(dados.grafo);


    outFile.close();

    cout.rdbuf(origCoutStreamBuf);


    ifstream inFile("saida.txt");
    cout << inFile.rdbuf();
    inFile.close();

    return 0;
}
