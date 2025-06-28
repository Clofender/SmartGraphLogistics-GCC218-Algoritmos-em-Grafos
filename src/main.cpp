#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include <dirent.h>   
#include <sys/stat.h> 
#include <x86intrin.h>
#include "functions.h"

using namespace std;


bool endsWith(const string& str, const string& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main() {
    const string inputDir  = "../dados/selected_instances";
    const string outputDir = "./apaga";

   
    struct stat st = {0};
    if (stat(outputDir.c_str(), &st) == -1) {
#ifdef _WIN32
        _mkdir(outputDir.c_str());
#else
        mkdir(outputDir.c_str(), 0755);
#endif
    }

    // Abre diretório de entrada
    DIR* dir = opendir(inputDir.c_str());
    if (!dir) {
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
      unsigned long long com1 = __rdtsc();
        string fileName = entry->d_name;
        if (!endsWith(fileName, ".dat")) continue;

       
        string inPath = inputDir + "/" + fileName;
       
        string baseName = fileName.substr(0, fileName.find_last_of('.'));

        // Leitura e processamento
        DadosGrafo dados = leituraBat(inPath);

        // Arquivo de saída: solutions/BHW1.sol
        string outPath = outputDir + "/" + "sol-" + baseName + ".dat";
        ofstream outFile(outPath);
        if (!outFile.is_open()) {
            cerr << "Erro ao criar arquivo de saída: " << outPath << endl;
            continue;
        }

      
        streambuf* origCout = cout.rdbuf();
        cout.rdbuf(outFile.rdbuf());
        int i = 1;
        // Executa a heurística
        criarLista(dados.grafo, dados.edges, dados.reqEdges,
                   dados.reqArcs, dados.arcs, dados.reqNodes,
                   dados.caminhosObrigatorios);
        PathScanning(dados.grafo, dados, dados.edges, dados.reqEdges, dados.reqArcs, dados.arcs, dados.reqNodes, dados.caminhosObrigatorios,com1,i,baseName);


        cout.rdbuf(origCout);
        outFile.close();

        cout << "Instância" << baseName << " processada"<< endl;
    }
    // funções da parte 1 que não estão sendo executadas
    
    // mostrarVertices(dados.grafo);
    // mostrarArestas(dados.grafo);
    // mostrarArcos(dados.grafo);
    // mostrarVerticesReq(dados.grafo);
    // mostrarArestasReq(dados.grafo);
    // mostrarArcosReq(dados.grafo);
    // cout << "Densidade do grafo: " << densidadeGrafo(dados.grafo) << endl;
    // cout << "Caminho médio: " << caminhoMedio(dados.grafo) << endl;
    // cout << "Diâmetro: " << diametro(dados.grafo) << endl;
    // cout << "Componentes conectados: " << componentesConectados(dados.grafo) << endl;
    // pair<int, int> grau = calculaGrau(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs);
    // cout << "Grau mínimo: " << grau.first << endl;
    // cout << "Grau máximo: " << grau.second << endl;

    // intermed(dados.grafo);


    closedir(dir);
    return 0;
}
