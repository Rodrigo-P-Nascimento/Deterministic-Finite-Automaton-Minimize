#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

//gcc grafo.c -o grafo -lgvc -lcgraph

int main() {
    Agraph_t *g;
    Agnode_t *n1, *n2, *n3;
    Agedge_t *e0, *e1, *e2, *e3, *e4, *e5;

    GVC_t *gvc = gvContext();

    // Cria o grafotesteGrafoV2
    g = agopen("g", Agdirected, NULL);

    // Adiciona os nós ao grafo
    n1 = agnode(g, "q0", 1);
    n2 = agnode(g, "q1", 1);
    n3 = agnode(g, "q2", 1);

    //setando os nós como o formato de circulo
    agsafeset(n1, "shape", "diamond", "");
    agsafeset(n2, "shape", "circle", "");
    agsafeset(n3, "shape", "circle", "");

    //dando o formato de double-circle nos nós finais
    agsafeset(n2, "peripheries", "2", "");
    agsafeset(n3, "peripheries", "2", "");

    // Adiciona as arestas ao grafo
    e0 = agedge(g, n1, n1, NULL, 1);
    e1 = agedge(g, n1, n2, NULL, 1);
    e2 = agedge(g, n2, n2, NULL, 1);
    e3 = agedge(g, n2, n3, NULL, 1);
    e4 = agedge(g, n3, n3, NULL, 1);
    e5 = agedge(g, n3, n3, NULL, 1);

    //Adicionando os pesos nas arestas
    agsafeset(e0, "label", "0", "");
    agsafeset(e1, "label", "1", "");
    agsafeset(e2, "label", "0", "");
    agsafeset(e3, "label", "1", "");
    agsafeset(e4, "label", "1", "");
    agsafeset(e5, "label", "0", "");

    agsafeset(g, "rankdir", "LR", "");

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
