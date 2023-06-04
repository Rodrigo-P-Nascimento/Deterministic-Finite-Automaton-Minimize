#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include "machine.h"

//gcc grafo.c -o grafo -lgvc -lcgraph

int main() {

    //Declaramos a maquina aqui bro
    Machine_t maquina;

    Machine_alphabet_t alfabeto[] = {0, 1};
    Machine_state_t filhos[3];
    Machine_Transition_t tranzas[3][2];

    tranzas[0][0].destState = 0;
    tranzas[0][0].symbol = 0;

    tranzas[0][1].destState = 1;
    tranzas[0][1].symbol = 1;

    tranzas[1][0].destState = 1;
    tranzas[1][0].symbol = 0;

    tranzas[1][1].destState = 2;
    tranzas[1][1].symbol = 1;

    tranzas[2][0].destState = 2;
    tranzas[2][0].symbol = 0;

    tranzas[2][1].destState = 2;
    tranzas[2][1].symbol = 1;

    filhos[0].stateID = 0;
    filhos[0].transitions = tranzas[0];
    filhos[0].isFinal = false;

    filhos[1].stateID = 1;
    filhos[1].transitions = tranzas[1];
    filhos[1].isFinal = false;

    filhos[2].stateID = 2;
    filhos[2].transitions = tranzas[2];
    filhos[2].isFinal = true;

    maquina.alphabet = alfabeto;
    maquina.states = filhos;
    maquina.initial = 0;

    size_t tamAlpha = 2;
    size_t quantStates = 3;
    size_t quantTrans = 6;

    Agnode_t* nos[quantStates];
    Agraph_t *g;
    Agedge_t *e0;
    char estados[20];

    GVC_t *gvc = gvContext();

    // Cria o grafo
    g = agopen("g", Agdirected, NULL);

    for(int i = 0; i < quantStates; i++){
        sprintf(estados, "q%d", i);//fazemos um casting de int para string
        nos[i] = agnode(g, estados, 1);//Adiciona os nós no grafo

        if(i == maquina.initial){
            agsafeset(nos[i], "shape", "diamond", "");
        }else{
            agsafeset(nos[i], "shape", "circle", "");//setando os nós como o formato de circulo
        }

        if(maquina.states[i].isFinal){
            agsafeset(nos[i], "peripheries", "2", "");
        }
    }

    //setando os nós como o formato de circulo
    //agsafeset(n1, "shape", "diamond", "");
    //agsafeset(n2, "shape", "circle", "");
    //agsafeset(n3, "shape", "circle", "");

    //dando o formato de double-circle nos nós finais
    //agsafeset(n2, "peripheries", "2", "");
    //agsafeset(n3, "peripheries", "2", "");
    //int qauntTransi = sizeof(maquina.states[0].transitions);
    //printf("Quantidade de arestas na maquina: %d\n",qauntTransi);

    Agnode_t *notf[0] = {};

    for(size_t i = 0; i < quantStates; i++){
        for(size_t j = 0; j < tamAlpha; j++){

            sprintf(estados, "q%d", maquina.states[i].transitions[j].destState);
            notf[0] = agnode(g, estados, 1);//Adiciona os nós no grafo
            
            e0 = agedge(g, nos[i], notf[0], NULL, 1);
            sprintf(estados, "%d", (int)maquina.states[i].transitions[j].symbol);
            agsafeset(e0, "label", estados, "");
        }
    }

    //Pra deixar o grafo na horizontal e da esquerda para a direita
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
