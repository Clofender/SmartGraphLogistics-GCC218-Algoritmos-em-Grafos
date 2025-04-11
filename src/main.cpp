#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "functions.h"

using namespace std;
   
int main(){
    int entrada;
    DadosGrafo dados;
    Grafo grafo;
    dados = leituraBat();
    criarGrafo(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs);
    do {
        cout << "==========================================" << endl;
        cout << "              MENU PRINCIPAL              " << endl;
        cout << "==========================================" << endl;
        cout << "1. Inserir modelo da cidade" << endl;
        cout << "2. Retornar quantidade de vértices" << endl;
        cout << "3. Retornar quantidade de arestas" << endl;
        cout << "0. Sair" << endl;
        cout << "==========================================" << endl;
        cout << "Digite sua opcao: ";
        cin >> entrada;

        switch (entrada) {
            
            case 1: {
           
            }
                break; 
                
            case 2:
            
            mostrarVertices(dados.grafo);

            break;
            case 3:            
            mostrarArestas(dados.grafo);
            break;
            case 4:
            mostrarArcos(dados.grafo);

            break;
            case 5:
            
            mostrarVerticesReq(dados.grafo);

            break;
            case 6:
            
            mostrarArestasReq(dados.grafo);

            break;
            case 7:
            
            mostrarArcosReq(dados.grafo);

            break;

            case 8:
            
            cout<<densidadeGrafo(dados.grafo);

            break;
            case 9:
            
            cout<<caminhoMedio(dados.grafo);

            break;
            case 10:
            
            cout<<diametro(dados.grafo);

            break;

            case 11:{

            pair<int, int> grau = calculaGrau(dados.grafo, dados.reqEdges,dados.reqArcs,dados.arcs);
            cout<<grau.first;
            
            break;
            }
            case 12:{

            pair<int, int> grau2 = calculaGrau(dados.grafo, dados.reqEdges,dados.reqArcs,dados.arcs);
            cout<<grau2.second;
            
            break;

            }
            case 13:

            intermed(dados.grafo);
                
            break;
        }

    } while (entrada != 0);
    return 0;
}