#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <stdint.h>
#include <stdlib.h>

#include "grapMaker.h"

void RenderMachine(Machine_t* maquina){

    size_t tamAlpha = maquina->alphabet.len;//alphabet size
    size_t quantStates = maquina->states.len;

    Agnode_t* vertex[quantStates];//Array of vertices
    Agraph_t *g;//the graph object
    Agedge_t *e0;//this represents an edge, so we generalize it
    char auxString[20];

    GVC_t *gvc = gvContext();

    // Creat the graph itself
    g = agopen("g", Agdirected, NULL);

    for(int i = 0; i < (int)quantStates; i++){
        if(maquina->states.data[i].stateID.S < UINT16_MAX){
            sprintf(auxString, "q%d", maquina->states.data[i].stateID.S);
        }else{
            sprintf(auxString, "q%dq%d", maquina->states.data[i].stateID.S1,
                    maquina->states.data[i].stateID.S2);
        }
        vertex[i] = agnode(g, auxString, 1);//Add vertex to the graph

        if(i == (int)maquina->initial.S){//if it's the initial vertex with set it as a diamond shape, if not the vertex will be a circle
            agsafeset(vertex[i], "shape", "diamond", "");
        }else{
            agsafeset(vertex[i], "shape", "circle", "");
        }

        if(maquina->states.data[i].isFinal){//if the vertex is final we set it as a double circle, just add a new property
            agsafeset(vertex[i], "peripheries", "2", "");
        }
    }

    Agnode_t* noAux[1] = {};//As said, noAux is a auxiliar vertex, we must keep it!

    for(size_t i = 0; i < quantStates; i++){
        for(size_t j = 0; j < tamAlpha; j++){

            //TODO mudar para o nome original do estado
            if(maquina->states.data[i].transitions.data[j].destState.S < UINT16_MAX){
                sprintf(auxString, "q%d", maquina->states.data[i].transitions.data[j].destState.S);
            }else{
                sprintf(auxString, "q%dq%d", maquina->states.data[i].transitions.data[j].destState.S1,
                        maquina->states.data[i].transitions.data[j].destState.S2);
            }
            noAux[0] = agnode(g, auxString, 1);//Add vertex to the graph

            e0 = agedge(g, vertex[i], noAux[0], NULL, 1);//Add a edge to the graph

            char* label = unhash(maquina->states.data[i].transitions.data[j].symbol);
            sprintf(auxString, "%s", label);
            agsafeset(e0, "label", auxString, "");//Set the label on it
        }
    }

    //To make the graph horizontal and from left to right
    agsafeset(g, "rankdir", "LR", "");

    //Defines the layout of the graph
    gvLayout(gvc, g, "dot");

    //Save the graph on a file
    //TODO mudar nome depois
    gvRenderFilename(gvc, g, "png", "/home/ekank/Documents/Programming/TDC/output.png");

    //Free memory
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}
