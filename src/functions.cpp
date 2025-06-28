#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include <x86intrin.h>
#include "functions.h"

using namespace std;


const int INF = 1000000000;

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


pair<int, int> calculateRouteCostAndDemand(const vector<int>& tarefas_indices, int depotNode,
                                           const vector<Aresta>& todas_tarefas,
                                           const vector<vector<int>>& dist) {
    if (tarefas_indices.empty()) {
        return {0, 0};
    }

    int custo_total = 0;
    int total_demand = 0;
    int pos_atual = depotNode;

    for (int tarefas_i : tarefas_indices) {
        const auto& tarefa = todas_tarefas[tarefas_i];
        total_demand += tarefa.demand;

        // custo de deslocamento para a tarefa + custo de serviço
        // assumindo que a direção da tarefa já foi definida (from -> to)
        custo_total += dist[pos_atual][tarefa.from];
        custo_total += tarefa.sCost;
        pos_atual = tarefa.to;
    }

    // custo para retornar ao depósito
    custo_total += dist[pos_atual][depotNode];

    return {custo_total, total_demand};
}




void BuscaLocal(vector<vector<int>>& rotas_nodes, vector<int>& custos_rotas,
                 DadosGrafo& dados, const vector<vector<int>>& dist,
                 vector<Aresta>& caminhosObrigatorios) {

    int depotNode = dados.cabecalho.depotNode;
    int capacity = dados.cabecalho.capacity;
    bool melhoria_encontrada = true;
    
    
    while (melhoria_encontrada) { // no início de cada grande iteração, assumimos que não encontraremos melhorias.
                                // se qualquer operador encontrar uma, esta flag será setada para true.
        melhoria_encontrada = false;

        vector<vector<int>> rotas_tarefas;
        for(const auto& route_nodes : rotas_nodes) { // conversao das rotas de uma lista de vertices pra lista de indices de tarefas                                                  
            vector<int> tasks;
            if (route_nodes.size() > 2) {
                for(size_t j = 1; j + 1 < route_nodes.size(); j += 2) { // percorre a rota de nós de 2 em 2 para pegar os pares (from e to) de cada tarefa.
                    int u = route_nodes[j]; int w = route_nodes[j+1];
                    for (size_t k = 0; k < caminhosObrigatorios.size(); k++) {
                        if ((caminhosObrigatorios[k].from == u && caminhosObrigatorios[k].to == w)) {
                            tasks.push_back(k); break;
                        }
                    }
                }
            }
            rotas_tarefas.push_back(tasks);
        }

        //  INTRA-ROUTE RELOCATE
        for (size_t r_idx = 0; r_idx < rotas_tarefas.size(); r_idx++) {
            if (rotas_tarefas[r_idx].size() < 2) continue;
            
            // calcula o custo original da rota 1 vez por rota
            int original_cost = calculateRouteCostAndDemand(rotas_tarefas[r_idx], depotNode, caminhosObrigatorios, dist).first;

            for (size_t t_idx = 0; t_idx < rotas_tarefas[r_idx].size(); ++t_idx) { // itera sobre cada tarefa da rota, tratando-a como a candidata a ser movida.
                int task_to_move = rotas_tarefas[r_idx][t_idx];

                 // cria uma rota temporária sem a tarefa que esta se movendo.
                vector<int> temp_route = rotas_tarefas[r_idx];
                temp_route.erase(temp_route.begin() + t_idx);

                 // tenta inserir a tarefa em todas as posições possíveis da rota temporaria.
                for (size_t insert_pos = 0; insert_pos <= temp_route.size(); ++insert_pos) {
                    if (insert_pos == t_idx) continue;
                    vector<int> new_route = temp_route;
                    new_route.insert(new_route.begin() + insert_pos, task_to_move);
                    
                     // se o custo da nova rota for menor, achamos uma melhoria.
                    if (calculateRouteCostAndDemand(new_route, depotNode, caminhosObrigatorios, dist).first < original_cost) {
                        rotas_tarefas[r_idx] = new_route;
                        melhoria_encontrada = true;
                        goto next_iteration_ls;
                    }
                }
            }
        }

        // INTRA-ROUTE SWAP ---
        for (size_t r_idx = 0; r_idx < rotas_tarefas.size(); r_idx++) {
            if (rotas_tarefas[r_idx].size() < 2) continue;
            
            int original_cost = calculateRouteCostAndDemand(rotas_tarefas[r_idx], depotNode, caminhosObrigatorios, dist).first;
            
             // itera sobre todos os pares de tarefas (t1, t2) na rota.
            for (size_t t1_idx = 0; t1_idx < rotas_tarefas[r_idx].size(); t1_idx++) {
                for (size_t t2_idx = t1_idx + 1; t2_idx < rotas_tarefas[r_idx].size(); ++t2_idx) {
                    vector<int> new_route = rotas_tarefas[r_idx];
                    swap(new_route[t1_idx], new_route[t2_idx]); // troca
                    
                     // se o custo da nova rota for menor, achamos uma melhoria.
                    if (calculateRouteCostAndDemand(new_route, depotNode, caminhosObrigatorios, dist).first < original_cost) {
                        rotas_tarefas[r_idx] = new_route;
                        melhoria_encontrada = true;
                        goto next_iteration_ls;
                    }
                }
            }
        }

      
        // INTER-ROUTE RELOCATE (Mover entre rotas)
     
        for (size_t r1_idx = 0; r1_idx < rotas_tarefas.size(); r1_idx++) {
            for (size_t t1_idx = 0; t1_idx < rotas_tarefas[r1_idx].size(); t1_idx++) {
                int task_to_move_idx = rotas_tarefas[r1_idx][t1_idx];
                const auto& task_to_move = caminhosObrigatorios[task_to_move_idx];
                
                 // Tenta mover para cada outra rota .
                for (size_t r2_idx = 0; r2_idx < rotas_tarefas.size(); ++r2_idx) {
                    if (r1_idx == r2_idx) continue;

                    int demand_r2 = calculateRouteCostAndDemand(rotas_tarefas[r2_idx], depotNode, caminhosObrigatorios, dist).second;
                    if (demand_r2 + task_to_move.demand > capacity) continue;

                     // Calcula o custo combinado das duas rotas antes da mudança.
                    double original_total_cost = calculateRouteCostAndDemand(rotas_tarefas[r1_idx], depotNode, caminhosObrigatorios, dist).first + calculateRouteCostAndDemand(rotas_tarefas[r2_idx], depotNode, caminhosObrigatorios, dist).first;

                    // tenta inserir a tarefa em cada posição da rota de destino.
                    for (size_t insert_pos = 0; insert_pos <= rotas_tarefas[r2_idx].size(); ++insert_pos) {
                        vector<int> new_r1 = rotas_tarefas[r1_idx];
                        new_r1.erase(new_r1.begin() + t1_idx);
                        vector<int> new_r2 = rotas_tarefas[r2_idx];
                        new_r2.insert(new_r2.begin() + insert_pos, task_to_move_idx);
                        
                           // Calcula o custo combinado depois da mudança.
                        double new_total_cost = calculateRouteCostAndDemand(new_r1, depotNode, caminhosObrigatorios, dist).first + calculateRouteCostAndDemand(new_r2, depotNode, caminhosObrigatorios, dist).first;

                        if (new_total_cost < original_total_cost) {
                            rotas_tarefas[r1_idx] = new_r1;
                            rotas_tarefas[r2_idx] = new_r2;
                            melhoria_encontrada = true;
                            goto next_iteration_ls;
                        }
                    }
                }
            }
        }

      
        //INTER-ROUTE SWAP (Trocar entre rotas)
        
        for (size_t r1_idx = 0; r1_idx < rotas_tarefas.size(); ++r1_idx) {
            for (size_t t1_idx = 0; t1_idx < rotas_tarefas[r1_idx].size(); t1_idx++) {
                  // ttera sobre os pares de rotas

                for (size_t r2_idx = r1_idx + 1; r2_idx < rotas_tarefas.size(); r2_idx++) {
                    for (size_t t2_idx = 0; t2_idx < rotas_tarefas[r2_idx].size(); t2_idx++) {
                        double custo_total_original = calculateRouteCostAndDemand(rotas_tarefas[r1_idx], depotNode, caminhosObrigatorios, dist).first + calculateRouteCostAndDemand(rotas_tarefas[r2_idx], depotNode, caminhosObrigatorios, dist).first;
                        
                        vector<int> new_r1 = rotas_tarefas[r1_idx];
                        vector<int> new_r2 = rotas_tarefas[r2_idx];
                        swap(new_r1[t1_idx], new_r2[t2_idx]); // troca
                        
                        // verifica a capacidade de ambas as rotas modificadas.
                        pair<int, int> new_info1 = calculateRouteCostAndDemand(new_r1, depotNode, caminhosObrigatorios, dist);
                        pair<int, int> new_info2 = calculateRouteCostAndDemand(new_r2, depotNode, caminhosObrigatorios, dist);
                        
                        if (new_info1.second <= capacity && new_info2.second <= capacity) {
                            if (new_info1.first + new_info2.first < custo_total_original) {
                                rotas_tarefas[r1_idx] = new_r1;
                                rotas_tarefas[r2_idx] = new_r2;
                                melhoria_encontrada = true;
                                goto next_iteration_ls;
                            }
                        }
                    }
                }
            }
        }

        // destino do goto. Se qualquer melhoria for encontrada, o código salta
        // para cá e o while reinicia, testando tudo de novo a partir do RELOCATE
        next_iteration_ls:;
        
        // Se encontrou alguma melhoria, atualiza a representação principal (rotas_nodes)
        if (melhoria_encontrada) {
            rotas_nodes.clear();
            rotas_nodes.reserve(rotas_tarefas.size());
            for(const auto& task_route : rotas_tarefas) {
                vector<int> node_route;
                if (!task_route.empty()) {
                    node_route.push_back(depotNode);
                    for(int task_idx : task_route) {
                        node_route.push_back(caminhosObrigatorios[task_idx].from);
                        node_route.push_back(caminhosObrigatorios[task_idx].to);
                    }
                    node_route.push_back(depotNode);
                }
                rotas_nodes.push_back(node_route);
            }
        }
    }

    // --- CÁLCULO FINAL E ATUALIZAÇÃO DOS CUSTOS ---
    custos_rotas.assign(rotas_nodes.size(), 0);
    for(size_t i = 0; i < rotas_nodes.size(); ++i) {
        // Converte uma última vez para calcular o custo final
        vector<int> final_tasks;
        if(rotas_nodes[i].size() > 2){
            for(size_t j = 1; j + 1 < rotas_nodes[i].size(); j += 2) {
                int u = rotas_nodes[i][j]; int w = rotas_nodes[i][j+1];
                for (size_t k = 0; k < caminhosObrigatorios.size(); ++k) {
                    if ((caminhosObrigatorios[k].from == u && caminhosObrigatorios[k].to == w)) {
                        final_tasks.push_back(k); break;
                    }
                }
            }
        }
        custos_rotas[i] = calculateRouteCostAndDemand(final_tasks, depotNode, caminhosObrigatorios, dist).first;
    }
}


void PathScanning(Grafo& grafo, DadosGrafo& dados, const vector<Edge>& edges,
        const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs, 
        const vector<Arc>& arcs,const vector<RequiredNode>& reqNodes, 
        vector<Aresta>& caminhosObrigatorios,unsigned long long com1, int i, string baseName){
         unsigned long long com = __rdtsc();
         
        
        vector<int> rota_impressao;
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
                int dist1 = dist[v][caminhosObrigatorios[i].from];
                int dist2 = dist[v][caminhosObrigatorios[i].to];
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
        int it1 = 1;

        bool atendidos = true;
        while(atendidos){
        atendidos = false;
        int custo_indv = 0;

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
        int it2 = 1;
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
            custo_indv = custo_indv + dist[ponto_partida][inicio];
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
        custo_indv = edge.sCost + custo_sec + custo_indv;
        rota.push_back(u);
        rota.push_back(w);
        ponto_partida = w;
        cap_atual = cap_atual - edge.demand;  
      }
      rota_impressao.push_back(custo_indv);
      rotas.push_back(rota);

    }
    // chama a busca local para tentar melhorar os resulatdos 

    BuscaLocal(rotas, rota_impressao, dados, dist, caminhosObrigatorios);

    unsigned long long end = __rdtsc();

    int custo_total_melhorado = 0;
    for (int custo : rota_impressao) {
        custo_total_melhorado += custo;
    }
    
    cout << custo_total_melhorado<<endl;    
    cout << rotas.size()<<endl;
    cout << (end - com1)<<endl;
    cout << (end - com)<<endl;

    int deposito = 0; 
    int dia      = 1; 

for (size_t r = 0; r < rotas.size(); ++r) {
    const auto &rota      = rotas[r];        
    int custo_rota        = rota_impressao[r];  
    int num_services      = ( (int)rota.size() - 2 ) / 2;
    int total_visitas     = num_services + 2;


    int demanda_total = 0;
    for (int idx = 1; idx + 1 < (int)rota.size(); idx += 2) {
        int from = rota[idx];
        int to   = rota[idx+1];
     
        for (size_t j = 0; j < caminhosObrigatorios.size(); ++j) {
            const auto &e = caminhosObrigatorios[j];
            if ((e.from == from && e.to == to) || (e.from == to && e.to == from)) {
                demanda_total += e.demand;
                break;
            }
        }
    }

    cout << deposito << " "
         << dia      << " "
         << (r + 1)  << " "
         << demanda_total << " "
         << custo_rota     << " "
         << total_visitas;

    // triplas
    cout << " (D 1,1,1)";

   
    for (int idx = 1; idx + 1 < (int)rota.size(); idx += 2) {
        int u = rota[idx];
        int w = rota[idx+1];
        // descobre qual elemento j de caminhosObrigatorios corresponde a (u,w):
        for (size_t j = 0; j < caminhosObrigatorios.size(); ++j) {
            const auto &e = caminhosObrigatorios[j];
            if ((e.from == u && e.to == w) || (e.from == w && e.to == u)) {
                int serv_id = (int)j + 1;  
                cout << " (S " << serv_id << "," << u << "," << w << ")";
                break;
            }
        }
    }


    cout << " (D 1,1,1)"<<endl;
}

     

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


