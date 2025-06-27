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


pair<int, int> calculateRouteCostAndDemand(const vector<int>& task_indices, int depotNode,
                                           const vector<Aresta>& all_tasks,
                                           const vector<vector<int>>& dist) {
    if (task_indices.empty()) {
        return {0, 0};
    }

    int total_cost = 0;
    int total_demand = 0;
    int current_pos = depotNode;

    for (int task_idx : task_indices) {
        const auto& task = all_tasks[task_idx];
        total_demand += task.demand;

        // Custo de deslocamento para a tarefa + custo de serviço
        // Assumindo que a direção da tarefa já foi definida (from -> to)
        total_cost += dist[current_pos][task.from];
        total_cost += task.sCost;
        current_pos = task.to;
    }

    // Custo para retornar ao depósito
    total_cost += dist[current_pos][depotNode];

    return {total_cost, total_demand};
}




void localSearch(vector<vector<int>>& rotas_nodes, vector<int>& custos_rotas,
                 DadosGrafo& dados, const vector<vector<int>>& dist,
                 vector<Aresta>& caminhosObrigatorios) {

    int depotNode = dados.cabecalho.depotNode;
    int capacity = dados.cabecalho.capacity;
    bool improvement_found = true;

    while (improvement_found) {
        improvement_found = false;

        
        // Converte para formato de tarefas a cada iteração, pois as rotas podem mudar
        vector<vector<int>> rotas_tarefas;
        for(const auto& route_nodes : rotas_nodes) {
            vector<int> tasks;
            if (route_nodes.size() > 2) {
                for(size_t j = 1; j + 1 < route_nodes.size(); j += 2) {
                    int u = route_nodes[j]; int w = route_nodes[j+1];
                    for (size_t k = 0; k < caminhosObrigatorios.size(); ++k) {
                        if ((caminhosObrigatorios[k].from == u && caminhosObrigatorios[k].to == w)) {
                            tasks.push_back(k); break;
                        }
                    }
                }
            }
            rotas_tarefas.push_back(tasks);
        }
        for (size_t r_idx = 0; r_idx < rotas_tarefas.size(); ++r_idx) {
    if (rotas_tarefas[r_idx].size() < 4) continue; // Precisa de pelo menos 4 tarefas para o 2-opt fazer sentido

    // 'i' e 'j' são os índices das TAREFAS que vêm ANTES das arestas a serem trocadas
    for (size_t i = 0; i < rotas_tarefas[r_idx].size() - 1; ++i) {
        for (size_t j = i + 1; j < rotas_tarefas[r_idx].size(); ++j) {
            
            // Cria uma cópia da rota para testar a inversão
            vector<int> new_route = rotas_tarefas[r_idx];

            // Inverte o segmento entre a tarefa (i+1) e a tarefa (j)
            // Esta é a operação chave do 2-Opt
            reverse(new_route.begin() + i + 1, new_route.begin() + j + 1);

            // Calcula o custo da rota original e da nova rota
            // Nota: A forma mais eficiente seria calcular apenas a diferença (delta),
            // mas para uma primeira implementação, recalcular tudo é mais seguro.
            int original_cost = calculateRouteCostAndDemand(rotas_tarefas[r_idx], depotNode, caminhosObrigatorios, dist).first;
            int new_cost = calculateRouteCostAndDemand(new_route, depotNode, caminhosObrigatorios, dist).first;

            if (new_cost < original_cost) {
                rotas_tarefas[r_idx] = new_route; // Aplica a melhora
                improvement_found = true;
                cout << "[LS] Melhora encontrada com 2-Opt na rota " << r_idx << "!" << endl;
                goto next_iteration_ls; // Reinicia a busca
            }
        }
    }
}
        // --- VIZINHANÇA 1: INTRA-ROUTE RELOCATE (Mover dentro da rota) ---
        for (size_t r_idx = 0; r_idx < rotas_tarefas.size(); ++r_idx) {
            if (rotas_tarefas[r_idx].size() < 2) continue; // Precisa de pelo menos 2 tarefas
            for (size_t t_idx = 0; t_idx < rotas_tarefas[r_idx].size(); ++t_idx) {
                int task_to_move = rotas_tarefas[r_idx][t_idx];
                pair<int,int> original_info = calculateRouteCostAndDemand(rotas_tarefas[r_idx], depotNode, caminhosObrigatorios, dist);
                int original_cost = original_info.first;

                vector<int> temp_route = rotas_tarefas[r_idx];
                temp_route.erase(temp_route.begin() + t_idx);

                for (size_t insert_pos = 0; insert_pos <= temp_route.size(); ++insert_pos) {
                    vector<int> new_route = temp_route;
                    new_route.insert(new_route.begin() + insert_pos, task_to_move);
                    
                    pair<int,int> new_info = calculateRouteCostAndDemand(new_route, depotNode, caminhosObrigatorios, dist);
                    if (new_info.first < original_cost) {
                        rotas_tarefas[r_idx] = new_route;
                        improvement_found = true;
                        goto next_iteration_ls;
                    }
                }
            }
        }

        // --- VIZINHANÇA 2: INTRA-ROUTE SWAP (Trocar dentro da rota) ---
        for (size_t r_idx = 0; r_idx < rotas_tarefas.size(); ++r_idx) {
            if (rotas_tarefas[r_idx].size() < 2) continue;
            for (size_t t1_idx = 0; t1_idx < rotas_tarefas[r_idx].size(); ++t1_idx) {
                for (size_t t2_idx = t1_idx + 1; t2_idx < rotas_tarefas[r_idx].size(); ++t2_idx) {
                    pair<int,int> original_info = calculateRouteCostAndDemand(rotas_tarefas[r_idx], depotNode, caminhosObrigatorios, dist);
                    vector<int> new_route = rotas_tarefas[r_idx];
                    swap(new_route[t1_idx], new_route[t2_idx]);
                    pair<int,int> new_info = calculateRouteCostAndDemand(new_route, depotNode, caminhosObrigatorios, dist);
                    if (new_info.first < original_info.first) {
                        rotas_tarefas[r_idx] = new_route;
                        improvement_found = true;
                        goto next_iteration_ls;
                    }
                }
            }
        }
        
        // --- VIZINHANÇA 3 & 4: INTER-ROUTE (Seus operadores originais) ---
        // (Código de Inter-Route Insert/Swap aqui... se desejar mantê-los)

        next_iteration_ls:;
        
        // Se houve melhoria, atualiza rotas_nodes para a próxima iteração do while
        if (improvement_found) {
            rotas_nodes.clear();
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
        pair<int, int> final_info = calculateRouteCostAndDemand(final_tasks, depotNode, caminhosObrigatorios, dist);
        custos_rotas[i] = final_info.first;
    }
}


   void PathScanning(Grafo& grafo, DadosGrafo& dados, const vector<Edge>& edges,
                  const vector<RequiredEdge>& reqEdges, const vector<RequiredArc>& reqArcs,
                  const vector<Arc>& arcs,const vector<RequiredNode>& reqNodes,
                  vector<Aresta>& caminhosObrigatorios,unsigned long long com1, int i, string baseName){
    unsigned long long com = __rdtsc();

    vector<vector<int>> rotas;
    vector<int> rota_impressao; // Vetor para custos individuais de cada rota

    int veiculos = (dados.cabecalho.numVehicles == -1) ? INF : dados.cabecalho.numVehicles;
    int inicio = dados.cabecalho.depotNode;

    // Dijkstra de todos os vertices
    vector<vector<int>> dist(grafo.vertices + 1, vector<int>(grafo.vertices + 1, INF));
    for(int itVertice = 1; itVertice <= grafo.vertices; itVertice++){
        vector<bool> visitado(grafo.vertices + 1, false);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> heap;
        dist[itVertice][itVertice] = 0;
        heap.push({0, itVertice});

        while(!heap.empty()){
            int u = heap.top().second;
            heap.pop();
            if(visitado[u]) continue;
            visitado[u] = true;
            for(auto const& i : grafo.lista[u]){
                int v = i.to;
                int peso = i.tCost;
                if(dist[itVertice][u] + peso < dist[itVertice][v]){
                    dist[itVertice][v] = dist[itVertice][u] + peso;
                    heap.push({dist[itVertice][v], v});
                }
            }
        }
    }

    // Construindo a lista de candidatos
    vector<vector<int>> listaCand(grafo.vertices + 1);
    for(int v = 1; v <= grafo.vertices; v++){
        vector<pair<int, int>> temp;
        temp.reserve(caminhosObrigatorios.size());
        for(int j = 0; j < (int)caminhosObrigatorios.size(); j++){
            int distmin = min(dist[v][caminhosObrigatorios[j].from], dist[v][caminhosObrigatorios[j].to]);
            temp.emplace_back(distmin, j);
        }
        sort(temp.begin(), temp.end());
        listaCand[v].reserve(temp.size());
        for (auto const& p : temp) {
            listaCand[v].push_back(p.second);
        }
    }

    // Construção da rota inicial
    int custo_total = 0;
    while(true){
        bool algum_nao_atendido = false;
        for(auto& r : caminhosObrigatorios){
            if(!r.atendido){
                algum_nao_atendido = true;
                break;
            }
        }
        if(!algum_nao_atendido) break;

        int custo_indv = 0;
        int cap_atual = dados.cabecalho.capacity;
        vector<int> rota;
        int ponto_partida = inicio;
        rota.push_back(ponto_partida);

        while(true){
            int p = -1;
            for(int v_idx : listaCand[ponto_partida]){
                if(!caminhosObrigatorios[v_idx].atendido && caminhosObrigatorios[v_idx].demand <= cap_atual){
                    p = v_idx;
                    break;
                }
            }
            if(p == -1){
                custo_total += dist[ponto_partida][inicio];
                custo_indv += dist[ponto_partida][inicio];
                rota.push_back(inicio);
                break;
            }

            auto &edge = caminhosObrigatorios[p];
            edge.atendido = true;
            int u, w;
            int custo_sec;
            if(dist[ponto_partida][edge.from] <= dist[ponto_partida][edge.to]){
                u = edge.from;
                w = edge.to;
                custo_sec = dist[ponto_partida][edge.from];
            } else {
                u = edge.to;
                w = edge.from;
                custo_sec = dist[ponto_partida][edge.to];
            }
            custo_total += edge.sCost + custo_sec;
            custo_indv += edge.sCost + custo_sec;
            rota.push_back(u);
            rota.push_back(w);
            ponto_partida = w;
            cap_atual -= edge.demand;
        }
        rotas.push_back(rota);
        rota_impressao.push_back(custo_indv);
    }
    
    // =================================================================
    // ETAPA DE MELHORIA: CHAMADA PARA A BUSCA LOCAL
    // =================================================================
    cout << "Solucao inicial construida. Custo: " << custo_total << ". Iniciando refinamento..." << endl;
    localSearch(rotas, rota_impressao, dados, dist, caminhosObrigatorios);

    // AGORA, OS CÁLCULOS E IMPRESSÕES USARÃO OS RESULTADOS MELHORADOS
    unsigned long long end = __rdtsc();

    int custo_total_melhorado = 0;
    for (int custo : rota_impressao) {
        custo_total_melhorado += custo;
    }
    
    cout << custo_total_melhorado << "\n";
    cout << rotas.size()  << "\n";
    cout << (end - com1)  << "\n";
    cout << (end - com)   << "\n";

    int deposito = 0;
    int dia = 1;
    for (size_t r = 0; r < rotas.size(); ++r) {
        const auto &rota = rotas[r];
        if (rota.size() <= 2) continue; // Pular rotas vazias que podem surgir da busca local
        int custo_rota = rota_impressao[r];
        int num_services = ((int)rota.size() - 2) / 2;
        int total_visitas = num_services + 2;

        int demanda_total = 0;
        for (int idx = 1; idx + 1 < (int)rota.size(); idx += 2) {
            int from = rota[idx];
            int to = rota[idx+1];
            for (const auto &e : caminhosObrigatorios) {
                if ((e.from == from && e.to == to) || (e.from == to && e.to == from)) {
                    demanda_total += e.demand;
                    break;
                }
            }
        }

        cout << deposito << " " << dia << " " << (r + 1) << " "
             << demanda_total << " " << custo_rota << " " << total_visitas;
        cout << " (D 1,1,1)";

        for (int idx = 1; idx + 1 < (int)rota.size(); idx += 2) {
            int u = rota[idx];
            int w = rota[idx+1];
            for (size_t j = 0; j < caminhosObrigatorios.size(); ++j) {
                const auto &e = caminhosObrigatorios[j];
                if ((e.from == u && e.to == w) || (e.from == w && e.to == u)) {
                    int serv_id = (int)j + 1;
                    cout << " (S " << serv_id << "," << u << "," << w << ")";
                    break;
                }
            }
        }
        cout << " (D 1,1,1)" << endl;
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


