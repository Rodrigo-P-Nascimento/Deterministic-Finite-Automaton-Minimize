#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include "../machine/machine.h"

//gcc graphMaker.c -o graphMaker -lgvc -lcgraph

int main() {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //Declaramos a maquina aqui bro Essa parte pode ser apagada apos a completa interação com o codigo final Egidio
    Machine_t maquina;

    Machine_alphabet_t alfabeto[] = {0, 1};
    Machine_state_t filhos[3];
    Machine_Transition_t transicoes[3][2];

    transicoes[0][0].destState = 0;
    transicoes[0][0].symbol = 0;

    transicoes[0][1].destState = 1;
    transicoes[0][1].symbol = 1;

    transicoes[1][0].destState = 1;
    transicoes[1][0].symbol = 0;

    transicoes[1][1].destState = 2;
    transicoes[1][1].symbol = 1;

    transicoes[2][0].destState = 2;
    transicoes[2][0].symbol = 0;

    transicoes[2][1].destState = 2;
    transicoes[2][1].symbol = 1;

    filhos[0].stateID = 0;
    filhos[0].transitions = transicoes[0];
    filhos[0].isFinal = false;

    filhos[1].stateID = 1;
    filhos[1].transitions = transicoes[1];
    filhos[1].isFinal = false;

    filhos[2].stateID = 2;
    filhos[2].transitions = transicoes[2];
    filhos[2].isFinal = true;

    maquina.alphabet = alfabeto;
    maquina.states = filhos;
    maquina.initial = 0;
    
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    size_t tamAlpha = sizeof(maquina.alphabet)/sizeof(maquina.alphabet[0]);//alphabet size
    size_t quantStates = 3// POR ENQUANTO PRECISAMOS SABER A QUANTIDADE DE ESTADOS ANTES 

    Agnode_t* vertex[quantStates];//Array of vertices
    Agraph_t *g;//the graph object
    Agedge_t *e0;//this represents an edge, so we generalize it
    char auxString[20];

    GVC_t *gvc = gvContext();

    // Creat the graph itself
    g = agopen("g", Agdirected, NULL);

    for(int i = 0; i < quantStates; i++){
        sprintf(auxString, "q%d", i);//We made this custom string "q_" to be better see in the graph
        vertex[i] = agnode(g, auxString, 1);//Add vertex to the graph

        if(i == maquina.initial){//if it's the initial vertex with set it as a diamond shape, if not the vertex will be a circle
            agsafeset(vertex[i], "shape", "diamond", "");
        }else{
            agsafeset(vertex[i], "shape", "circle", "");
        }

        if(maquina.states[i].isFinal){//if the vertex is final we set it as a double circle, just add a new property
            agsafeset(vertex[i], "peripheries", "2", "");
        }
    }

    Agnode_t* noAux[0] = {};//As said, noAux is a auxiliar vertex, we must keep it!

    for(size_t i = 0; i < quantStates; i++){
        for(size_t j = 0; j < tamAlpha; j++){

            sprintf(auxString, "q%d", maquina.states[i].transitions[j].destState);
            noAux[0] = agnode(g, auxString, 1);//Add vertex to the graph
            
            e0 = agedge(g, vertex[i], noAux[0], NULL, 1);//Add a edge to the graph
            sprintf(auxString, "%d", (int)maquina.states[i].transitions[j].symbol);
            agsafeset(e0, "label", auxString, "");//Set the label on it
        }
    }

    //To make the graph horizontal and from left to right
    agsafeset(g, "rankdir", "LR", "");

    //Defines the layout of the graph
    gvLayout(gvc, g, "dot");

    //Save the graph on a file
    gvRenderFilename(gvc, g, "png", "output.png");

    //Free memory
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);

    return 0;
}
