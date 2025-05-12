import sys
from dataclasses import dataclass
from copy import deepcopy

INF = 10**9

@dataclass
class Cabecalho:
    name: str = ""
    optimalValue: int = 0
    numVehicles: int = 0
    capacity: int = 0
    depotNode: int = 0
    numNodes: int = 0
    numEdges: int = 0
    numArcs: int = 0
    numReqNodes: int = 0
    numReqEdges: int = 0
    numReqArcs: int = 0

@dataclass
class Grafo:
    vertices: int = 0
    arestas: int = 0
    arcos: int = 0
    custo: int = 0
    matriz: list = None
    verticesReq: int = 0
    arestasReq: int = 0
    arcosReq: int = 0

@dataclass
class RequiredNode:
    id: str
    demand: int
    sCost: int

@dataclass
class RequiredEdge:
    id: str
    from_node: int
    to_node: int
    tCost: int
    demand: int
    sCost: int

@dataclass
class RequiredArc:
    id: str
    from_node: int
    to_node: int
    tCost: int
    demand: int
    sCost: int

@dataclass
class Arc:
    id: str
    from_node: int
    to_node: int
    tCost: int

@dataclass
class DadosGrafo:
    grafo: Grafo
    reqNodes: list
    reqEdges: list
    reqArcs: list
    arcs: list

def criar_grafo(grafo, req_edges, req_arcs, arcs):
    size = grafo.vertices + 1
    grafo.matriz = [[INF] * size for _ in range(size)]
    
    for i in range(size):
        grafo.matriz[i][i] = 0
    
    for re in req_edges:
        grafo.matriz[re.from_node][re.to_node] = re.tCost
        grafo.matriz[re.to_node][re.from_node] = re.tCost
    
    for ra in req_arcs:
        grafo.matriz[ra.from_node][ra.to_node] = ra.tCost
    
    for a in arcs:
        grafo.matriz[a.from_node][a.to_node] = a.tCost
    
    return grafo.matriz

def mostrar_vertices(grafo):
    print(f"Quantidade de vértices: {grafo.vertices}")
    return grafo.vertices

def mostrar_arestas(grafo):
    print(f"Quantidade de arestas: {grafo.arestas}")
    return grafo.arestas

def mostrar_arcos(grafo):
    print(f"Quantidade de arcos: {grafo.arcos}")
    return grafo.arcos

def mostrar_vertices_req(grafo):
    print(f"Quantidade de vértices requisitados: {grafo.verticesReq}")
    return grafo.verticesReq

def mostrar_arestas_req(grafo):
    print(f"Quantidade de arestas requisitadas: {grafo.arestasReq}")
    return grafo.arestasReq

def mostrar_arcos_req(grafo):
    print(f"Quantidade de arcos requisitados: {grafo.arcosReq}")
    return grafo.arcosReq

def densidade_grafo(grafo):
    return (grafo.arcos + (grafo.arestas * 2)) / (grafo.vertices * (grafo.vertices - 1))

def floyd_warshall(grafo):
    n = grafo.vertices
    for k in range(1, n+1):
        for i in range(1, n+1):
            for j in range(1, n+1):
                if grafo.matriz[i][k] + grafo.matriz[k][j] < grafo.matriz[i][j]:
                    grafo.matriz[i][j] = grafo.matriz[i][k] + grafo.matriz[k][j]

def copy_grafo(original):
    return Grafo(
        vertices=original.vertices,
        arestas=original.arestas,
        arcos=original.arcos,
        custo=original.custo,
        matriz=[row.copy() for row in original.matriz],
        verticesReq=original.verticesReq,
        arestasReq=original.arestasReq,
        arcosReq=original.arcosReq
    )

def caminho_medio(grafo_original):
    grafo = copy_grafo(grafo_original)
    floyd_warshall(grafo)
    soma = 0
    cont = 0
    for i in range(1, grafo.vertices + 1):
        for j in range(1, grafo.vertices + 1):
            if i != j and grafo.matriz[i][j] < INF:
                soma += grafo.matriz[i][j]
                cont += 1
    return soma / cont if cont else 0

def diametro(grafo_original):
    grafo = copy_grafo(grafo_original)
    floyd_warshall(grafo)
    max_d = 0
    for i in range(1, grafo.vertices + 1):
        for j in range(1, grafo.vertices + 1):
            if i != j and grafo.matriz[i][j] < INF:
                max_d = max(max_d, grafo.matriz[i][j])
    return max_d

def calcula_grau(grafo, req_edges, req_arcs, arcs):
    graus = [0] * (grafo.vertices + 1)
    
    for re in req_edges:
        graus[re.from_node] += 1
        graus[re.to_node] += 1
    
    for ra in req_arcs:
        graus[ra.from_node] += 1
        graus[ra.to_node] += 1
    
    for a in arcs:
        graus[a.from_node] += 1
        graus[a.to_node] += 1
    
    return (min(graus[1:]), max(graus[1:]))

def intermed(grafo_original):
    grafo = copy_grafo(grafo_original)
    floyd_warshall(grafo)
    for v in range(1, grafo.vertices + 1):
        count = 0
        for i in range(1, grafo.vertices + 1):
            for j in range(1, grafo.vertices + 1):
                if i != j and i != v and j != v:
                    if grafo.matriz[i][v] + grafo.matriz[v][j] == grafo.matriz[i][j]:
                        count += 1
        print(f"Intermediação do vértice {v}: {count}")

def dfs_componentes(u, grafo, visitado):
    visitado[u] = True
    for v in range(1, grafo.vertices + 1):
        if (grafo.matriz[u][v] < INF or grafo.matriz[v][u] < INF) and not visitado[v]:
            dfs_componentes(v, grafo, visitado)

def componentes_conectados(grafo):
    visitado = [False] * (grafo.vertices + 1)
    count = 0
    for v in range(1, grafo.vertices + 1):
        if not visitado[v]:
            dfs_componentes(v, grafo, visitado)
            count += 1
    return count

def leitura_bat(file_path):
    cabecalho = Cabecalho()
    grafo = Grafo()
    req_nodes = []
    req_edges = []
    req_arcs = []
    arcs = []
    secao_atual = ""
    
    with open(file_path, 'r') as f:
        for linha in f:
            linha = linha.strip()
            if not linha:
                continue
            
            if linha in ("ReN.req", "ReE.req", "EDGE.req", "ReA.req", "ARC"):
                secao_atual = linha
                continue
            
            if ':' in linha and not secao_atual:
                partes = linha.split(':', 1)
                campo = partes[0].strip()
                valor = partes[1].strip()
                
                if campo == "Name":
                    cabecalho.name = valor
                elif campo == "Optimal value":
                    cabecalho.optimalValue = int(valor)
                elif campo == "#Vehicles":
                    cabecalho.numVehicles = int(valor)
                elif campo == "Capacity":
                    cabecalho.capacity = int(valor)
                elif campo == "Depot Node":
                    cabecalho.depotNode = int(valor)
                elif campo == "#Nodes":
                    grafo.vertices = int(valor)
                elif campo == "#Edges":
                    grafo.arestas = int(valor)
                elif campo == "#Arcs":
                    grafo.arcos = int(valor)
                elif campo == "#Required N":
                    grafo.verticesReq = int(valor)
                elif campo == "#Required E":
                    grafo.arestasReq = int(valor)
                elif campo == "#Required A":
                    grafo.arcosReq = int(valor)
            else:
                partes = linha.split()
                if secao_atual == "ReN.req":
                    req_nodes.append(RequiredNode(partes[0], int(partes[1]), int(partes[2])))
                elif secao_atual == "ReE.req":
                    req_edges.append(RequiredEdge(partes[0], int(partes[1]), int(partes[2]), 
                                      int(partes[3]), int(partes[4]), int(partes[5])))
                elif secao_atual == "ReA.req":
                    req_arcs.append(RequiredArc(partes[0], int(partes[1]), int(partes[2]), 
                                     int(partes[3]), int(partes[4]), int(partes[5])))
                elif secao_atual == "ARC":
                    arcs.append(Arc(partes[0], int(partes[1]), int(partes[2]), int(partes[3])))
    
    return DadosGrafo(grafo, req_nodes, req_edges, req_arcs, arcs)

def main():
    dados = leitura_bat("C:/Users/josev/Documents/SmartGraphLogistics-GCC218-Algoritmos-em-Grafos/dados/selected_instances/BHW1.dat")
    criar_grafo(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs)
    
    # Cálculo das métricas
    densidade = densidade_grafo(dados.grafo)
    medio = caminho_medio(dados.grafo)
    diam = diametro(dados.grafo)
    componentes = componentes_conectados(dados.grafo)
    grau_min, grau_max = calcula_grau(dados.grafo, dados.reqEdges, dados.reqArcs, dados.arcs)
    
    # Saída formatada
    
    print(f"#Vertices: {dados.grafo.vertices}, #Arestas: {dados.grafo.arestas}, #Arcos: {dados.grafo.arcos}")
    print(f"#Vertices requeridos: {dados.grafo.verticesReq}")
    print(f"#Arestas requeridas: {dados.grafo.arestasReq}")
    print(f"#Arcos requeridos: {dados.grafo.arcosReq}\n")
    
    mostrar_vertices(dados.grafo)
    mostrar_arestas(dados.grafo)
    mostrar_arcos(dados.grafo)
    mostrar_vertices_req(dados.grafo)
    mostrar_arestas_req(dados.grafo)
    mostrar_arcos_req(dados.grafo)
    
    print(f"\nDensidade do grafo: {densidade:.6f}")
    print(f"Caminho médio: {medio:.4f}")
    print(f"Diâmetro: {diam}")
    print(f"Componentes conectados: {componentes}")
    print(f"Grau mínimo: {grau_min}")
    print(f"Grau máximo: {grau_max}")
    
    print("\nIntermediação por vértice:")
    intermed(dados.grafo)

if __name__ == "__main__":
    main()