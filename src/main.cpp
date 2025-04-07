#include <iostream>
#include <vector>
#include <tuple>

#include "functions.h"

using namespace std;
   
int main(){
    int entrada;
    Grafo grafo;
    
    do {
        cout << "==========================================" << endl;
        cout << "              MENU PRINCIPAL              " << endl;
        cout << "==========================================" << endl;
        cout << "1. Inserir modelo da cidade" << endl;
        cout << "2. Retornar quantidade de vértices" << endl;
        cout << "3. Opcao 3" << endl;
        cout << "0. Sair" << endl;
        cout << "==========================================" << endl;
        cout << "Digite sua opcao: ";
        cin >> entrada;

        switch (entrada) {
            
            case 1: {
            int esquinas, n_rua_mao_unica, n_rua_via_dupla, r1, r2, ru1, ru2;
            vector<tuple<int, int>> rua_mao_unica;
            vector<tuple<int, int>> rua_via_dupla;

            cout << "Insira o número de esquinas" << endl;
            cin >> esquinas;
            cout << "Insira o número de ruas de via dupla" << endl;
            cin >> n_rua_via_dupla;
            cout << "Insira o número de ruas de mao unica" << endl;
            cin >> n_rua_mao_unica;

            cout << "Indique as ruas de via dupla entre as esquinas atraves dos seus numeros"
            "(ex:[1 2], conecta as esquinas 1 e 2)"<< endl;

            for(int i = 1; i < n_rua_via_dupla+1; i++){
            cin>> r1 >> r2;
            rua_via_dupla.push_back(make_tuple(r1,r2));
            }

            cout << "Indique as ruas de mao unica entre as esquinas atraves dos seus numeros"
            "(ex:[1 2], conecta as esquinas 1 e 2)"<< endl;

            for(int i = 1; i < n_rua_via_dupla+1; i++){
            cin>> ru1 >> ru2;
            rua_mao_unica.push_back(make_tuple(ru1,ru2));
            }

            grafo.vertices = esquinas;
            grafo.arestas = n_rua_via_dupla;
            grafo.arcos = n_rua_mao_unica;
            grafo.matriz = criarGrafo(esquinas,n_rua_mao_unica,n_rua_via_dupla, rua_via_dupla, rua_mao_unica);
            }
                break; 
                
            case 2:
            mostrarVertices(grafo);

            break;
        }

    } while (entrada != 0);
}