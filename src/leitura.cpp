#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "functions.h"

using namespace std;


string parte(const string &s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) start++;
    auto end = s.end();
    do { end--; } while (distance(start, end) > 0 && isspace(*end));
    return string(start, end+1);
}

DadosGrafo leituraBat(){
    ifstream arquivo("../dados/selected_instances/BHW1.dat");
    if (!arquivo.is_open()){
        cout << "Erro ao abrir o arquivo!" << endl;
    }
    
    Cabecalho cabecalho;
    Grafo grafo;
    vector<RequiredNode> reqNodes;
    vector<RequiredEdge> reqEdges;
    vector<RequiredArc> reqArcs;
    vector<Arc> arcs;
    
    string linha;
    string secaoAtual = "";

    while (getline(arquivo, linha)) {
        linha = parte(linha);

        if (!linha.empty()) {
            if (linha.find("ReN.") != string::npos ||
                linha.find("ReE.") != string::npos ||
                linha.find("EDGE") != string::npos ||
                linha.find("ReA.") != string::npos ||
                linha.find("ARC") != string::npos) {
                
                secaoAtual = linha;
            }
            else if (linha.find(":") != string::npos && secaoAtual.empty()) {
                istringstream iss(linha);
                string campo;
                getline(iss, campo, ':');
                string valor;
                getline(iss, valor);
                valor = parte(valor);

                if (campo.find("Name") != string::npos)
                cabecalho.name = valor;
                else if (campo.find("Optimal value") != string::npos)
                cabecalho.optimalValue = stoi(valor);
                else if (campo.find("#Vehicles") != string::npos)
                cabecalho.numVehicles = stoi(valor);
                else if (campo.find("Capacity") != string::npos)
                cabecalho.capacity = stoi(valor);
                else if (campo.find("Depot Node") != string::npos)
                cabecalho.depotNode = stoi(valor);
                else if (campo.find("#Nodes") != string::npos)
                grafo.vertices = stoi(valor);
                else if (campo.find("#Edges") != string::npos)
                grafo.arestas = stoi(valor);
                else if (campo.find("#Arcs") != string::npos)
                grafo.arcos = stoi(valor);
                else if (campo.find("#Required N") != string::npos)
                grafo.verticesReq = stoi(valor);
                else if (campo.find("#Required E") != string::npos)
                grafo.arestasReq = stoi(valor);
                else if (campo.find("#Required A") != string::npos)
                grafo.arcosReq = stoi(valor);
                
            }
            else {
                istringstream iss(linha);
                if (secaoAtual.find("ReN.") != string::npos) {
                    RequiredNode rn;
                    iss >> rn.id >> rn.demand >> rn.sCost;
                    reqNodes.push_back(rn);
                }
                else if (secaoAtual.find("ReE.") != string::npos) {
                    RequiredEdge re;
                    iss >> re.id >> re.from >> re.to >> re.tCost >> re.demand >> re.sCost;
                    reqEdges.push_back(re);
                }
                else if (secaoAtual.find("ReA.") != string::npos) {
                    RequiredArc ra;
                    iss >> ra.id >> ra.from >> ra.to >> ra.tCost >> ra.demand >> ra.sCost;
                    reqArcs.push_back(ra);
                }
                else if (secaoAtual.find("ARC") != string::npos) {
                    Arc a;
                    iss >> a.id >> a.from >> a.to >> a.tCost;
                    arcs.push_back(a);
                }
            }
        }
    }

    arquivo.close();


    return {grafo, reqNodes, reqEdges, reqArcs, arcs};;
}