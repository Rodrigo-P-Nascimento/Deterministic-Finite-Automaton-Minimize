#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

//gcc grafo.c -o grafo -lgvc -lcgraph

int main() {
    Agraph_t *g;
    Agnode_t *n1, *n2, *n3;
    Agedge_t *e1, *e2, *e3;

    GVC_t *gvc = gvContext();

    // Cria o grafo
    g = agopen("g", Agdirected, NULL);

    // Adiciona os nós ao grafo
    n1 = agnode(g, "Node 1", 1);
    n2 = agnode(g, "Node 2", 1);
    n3 = agnode(g, "Node 3", 1);

    // Adiciona as arestas ao grafo
    e1 = agedge(g, n1, n2, NULL, 1);
    e2 = agedge(g, n2, n3, NULL, 1);
    e3 = agedge(g, n1, n1, NULL, 1);

    // Define o layout do grafo
    gvLayout(gvc, g, "dot");

    // Renderiza o grafo para um arquivo
    gvRenderFilename(gvc, g, "png", "output.png");

    // Libera os recursos
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);

    return 0;
}
