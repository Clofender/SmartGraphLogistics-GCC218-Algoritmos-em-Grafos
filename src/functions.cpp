#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
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
        grafo.matriz[re.from][re.to
        ] = re.tCost;
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
        void criarLista(Grafo& grafo,  const vector<Edge>& edges,
            const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, 
            const vector<Arc>& arcs, const vector<RequiredNode>& reqNodes,
            vector<Aresta>& caminhosObrigatorios) {
                
                for (const auto& e : edges) {
                    grafo.lista[e.from].push_back({e.from,e.to,e.tCost,0,0,false});
                    grafo.lista[e.to].push_back({e.to,e.from,e.tCost,0,0,false});
                    }

                for (const auto& re : reqEdges) {
                    grafo.lista[re.from].push_back({re.from,re.to,re.tCost,re.demand,re.sCost,false});
                    grafo.lista[re.to].push_back({re.to,re.from,re.tCost,re.demand,re.sCost,false});
                    caminhosObrigatorios.push_back({re.from,re.to,re.tCost,re.demand,re.sCost,false});
                    
                    }
                
                for (const auto& a : arcs) {
                        grafo.lista[a.from].push_back({a.from,a.to,a.tCost,0,0,false});
                       
                    }

                for (const auto& ra : reqArcs) {
                    grafo.lista[ra.from].push_back({ra.from,ra.to,ra.tCost,ra.demand,ra.sCost,false});
                    caminhosObrigatorios.push_back({ra.from,ra.to,ra.tCost,ra.demand,ra.sCost,false});
                    }
                
                for (const auto& rn : reqNodes) {
                        grafo.lista[rn.num].push_back({rn.num,rn.num,0,rn.demand,rn.sCost,false});
                        caminhosObrigatorios.push_back({rn.num,rn.num,0,rn.demand,rn.sCost,false});
                        }
        }

    void PathScanning(Grafo& grafo, DadosGrafo& dados, const vector<Edge>& edges,
        const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, 
        const vector<Arc>& arcs,const vector<RequiredNode>& reqNodes, 
        vector<Aresta>& caminhosObrigatorios){
        
        int veiculos;
        if(dados.cabecalho.numVehicles == -1){
            veiculos = INF;
        }
        else{
            veiculos = dados.cabecalho.numVehicles;
        }
        int inicio = dados.cabecalho.depotNode;

        //dijkstra de todos os vertices

        vector<vector<int>> dist(grafo.vertices + 1, vector<int>(grafo.vertices + 1, INF));

        for(int itVertice = 1; itVertice<=grafo.vertices; itVertice++){
        vector<bool> visitado(grafo.vertices+1,false);
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>> > heap;
        
        dist[itVertice][itVertice] = 0;
        heap.push({0,itVertice});

        while(!heap.empty()){
            pair<int,int> vertice = heap.top();
            int u = vertice.second;
            heap.pop();

            if(visitado[u] == true){
                continue;
            }
            visitado[u] = true;

            for(auto i = grafo.lista[u].begin(); i!= grafo.lista[u].end();i++){
                int v = i->to;
                int peso = i->tCost;

                if(dist[itVertice][u] + peso < dist[itVertice][v]){
                    dist[itVertice][v] = dist[itVertice][u] + peso;
                    heap.push({dist[itVertice][v],v});
                }
            }
        }

    }


    // construindo a lista de candidatos
    vector<vector<int>> listaCand(grafo.vertices + 1);
        //lista temporária para armazenar os caminhos requiridos
        for(int v = 1; v <= grafo.vertices; v++){
            vector<pair<int,int>> temp;
            temp.reserve(caminhosObrigatorios.size());
            //vê qual ponta da aresta está mais próxima
            for(int i = 0; i < (int)caminhosObrigatorios.size();i++){
                const auto& r = caminhosObrigatorios[i];
                int dist1 = dist[v][r.from];
                int dist2 = dist[v][r.to];
                int distmin = min(dist1,dist2);
                temp.emplace_back(distmin,i);
            }

            sort(temp.begin(), temp.end(),[](auto &a, auto &b){ 
                return a.first < b.first; 
            });

           listaCand[v].reserve(temp.size());
           for (auto &p : temp) {
                listaCand[v].push_back(p.second);
            }
        }

    //agora, constrói a rota
        //verifica se tem alguma aresta nao atendida
        vector<vector<int>> rotas;
        int custo_total = 0;
        bool atendidos = true;
        while(atendidos){
        atendidos = false;

        for(auto& i : caminhosObrigatorios){
            if(i.atendido == false){
                atendidos = true;
                break;
            }
        }
        if(atendidos == false){
            break;
        }

        int cap_atual = dados.cabecalho.capacity;
        vector<int> rota;
        int ponto_partida = inicio;
        rota.push_back(ponto_partida);
        
        //verifica as arestas candidatas podem ser atendidas
        while(true){
            int p = -1;
            for(int v : listaCand[ponto_partida]){
                auto& i = caminhosObrigatorios[v];
                if(i.demand <= cap_atual && i.atendido == false){
                    p = v;
                    break;
                }
            }
        //se nao, volta ao inicio
        if(p == -1){
            rota.push_back(inicio);
            custo_total = custo_total + dist[ponto_partida][inicio];
            break;
        }

        auto &edge = caminhosObrigatorios[p];
        int custo_sec = 0;
        int u,w;
        edge.atendido = true;

        if(dist[ponto_partida][edge.from] <= dist[ponto_partida][edge.to]){
            u = edge.from;
            w = edge.to;
            custo_sec = dist[ponto_partida][edge.from];                      
        }
        else{
            u = edge.to;
            w = edge.from;
            custo_sec = dist[ponto_partida][edge.to];        
        }
        custo_total = edge.sCost + custo_sec + custo_total ;
        rota.push_back(u);
        rota.push_back(w);
        ponto_partida = w;
        cap_atual = cap_atual - edge.demand;  
      }

      rotas.push_back(rota);
    }
    cout<<"rotas: "<<rotas.size()<<endl;
      cout<<"o custo total eh "<<custo_total;

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


