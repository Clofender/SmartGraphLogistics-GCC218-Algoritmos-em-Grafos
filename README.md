# SmartGraphLogistics-GCC218-Algoritmos-em-Grafos

SmartGraphLogistics é um projeto desenvolvido para a disciplina GCC218 – Algoritmos em Grafos (UFLA), com foco na aplicação de estruturas e algoritmos de grafos para otimização de rotas e análise logística em ambientes urbanos.

## Visão Geral

**SmartGraphLogistics** é um projeto desenvolvido para a disciplina **GCC218 – Algoritmos em Grafos** (UFLA). O objetivo é aplicar estruturas e algoritmos de grafos na otimização de rotas logísticas urbanas, considerando restrições como capacidade de veículos, distâncias e custos operacionais.

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![Linguagens](https://img.shields.io/badge/linguagens-C%2B%2B%20%7C%20Python-blue)
![Licença](https://img.shields.io/badge/licen%C3%A7a-MIT-green)

## Autores

- Daniel Silva Ferraz Neto – danielsfn1@gmail.com ou daniel.neto3@estudante.ufla.br  
- José Victor Miranda De Oliveira – jose.oliveira10@estudante.ufla.br

## Estrutura do Repositório

```
SmartGraphLogistics/
├── README.md           # Documentação do projeto
├── LICENSE             # Licença MIT do projeto
├── src/                # Código-fonte principal em C++
│   ├── main.cpp
│   ├── functions.cpp
│   ├── functions.h
│   ├── leitura.cpp
│   └── solutions/      # Pasta de saída para os arquivos de solução gerados
├── dados/              # Arquivos de entrada (instâncias do problema)
└── docs/               # Documentação adicional e relatórios
```

## Metodologia Implementada

O projeto segue as etapas definidas pela disciplina, com os seguintes progressos:

1.  **Pré-processamento**: Etapa concluída. Foram implementadas rotinas para a leitura e modelagem dos dados em grafos, além do cálculo de todas as 13 métricas estatísticas solicitadas (quantidades, densidade, graus, componentes conectados, caminho médio, diâmetro, intermediação, etc.).

2.  **Solução inicial**: Etapa concluída. Foi implementado um algoritmo construtivo para gerar uma solução inicial viável. A abordagem utilizada foi a heurística **Path Scanning**, que constrói as rotas de forma sequencial, selecionando iterativamente os serviços mais próximos que respeitam a capacidade do veículo.

3.  **Métodos de melhoria**: Como próximo passo, planeja-se aprimorar a solução inicial através de um algoritmo de **Busca Local**. Serão explorados movimentos como *Relocate*, *Swap* e *2-Opt* para refinar as rotas e reduzir o custo total.

4.  **Avaliação / Relatório e entrevista**: Análise de desempenho e documentação dos resultados.

## Instruções de Execução

### Pré-requisitos
* Um compilador C++ (como o g++).

### Passos

1.  Clone o repositório:
    ```bash
    git clone https://github.com/Clofender/SmartGraphLogistics-GCC218-Algoritmos-em-Grafos.git
    ```

2.  Acesse o diretório do código-fonte:
    ```bash
    cd SmartGraphLogistics-GCC218-Algoritmos-em-Grafos/src
    ```

3.  Compile o programa:
    ```bash
    g++ main.cpp functions.cpp functions.h leitura.cpp
    ```

4.  Execute o arquivo `Execute.exe` para gerar as soluções:
    ```bash
    ./Execute.exe
    ```
    *O programa irá processar automaticamente todas as instâncias da pasta `dados/selected_instances` e salvará os arquivos de solução na pasta `src/solutions`.*

## Licença

Este projeto está licenciado sob os termos da **MIT License**. Consulte o arquivo `LICENSE` para mais detalhes.
