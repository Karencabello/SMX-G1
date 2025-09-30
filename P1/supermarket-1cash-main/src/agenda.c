/*
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * LLibreria de funcions de l'agenda d'events
 * 
 * File:   agenda.c
 * 
 * Author: Dolors Sala
 */

#include "sev.h"
#include "agenda.h"

#define DEBUGagenda 1   // Bandera per fer seguiment de l'agenda 

//static esdev *agenda;
static int n_esd;
static int ara ;  // ultim component plena de l'agenda ordenada de major a menor del 0 a ara

// Inicialitza l'agenda amb n posicions per tenir n events esperant a ser executats
void ini_agenda(int n){
    n_esd = n;
    agenda = (esdev*) malloc(n_esd * sizeof (esdev));
    if (agenda == NULL){
        puts("Agenda: falta memoria");
        exit (-1);
    }
    ara = -1;
}
void imprimir_element_agenda(int i){
    printf("(%c, %8.4lf)",agenda[i].que,agenda[i].quan);
}
void imprimir_agenda(){
    int i;
    printf("Agenda (%2d): ",ara);
    for (i = 0; i <= ara; i++){
        imprimir_element_agenda(i);
    }
    printf("\n");
}
// Crea un esdeveniment per l'agenda
esdev crea_esdev(int que, float quan){
    esdev e;
    e.que = que;
    e.quan = quan;
    return (e);
}

// Afegeix l'event e a l'agenda ordenat cronologicament segons el "quan"
// El ta és el temps actual per imprimir en les traces de seguiment
void posa_agenda(float ta, esdev e) {
    int i;
    
    ++ara;
    if (ara == N){
        puts("Error, agenda plena");
        exit(1);
    }
    for (i = ara; i > 0; i--){
        if(e.quan <= agenda[i-1].quan)
                break;
        agenda[i] = agenda[i-1];
    }
    agenda[i]=e;
    
#if DEBUGagenda == 1 
   printf("%.4lf Posa AGENDA %2d: ", ta, ara);
   imprimir_agenda();
#endif
} // posa_agenda

// Elimina l'element e de l'agenda
// El ta és el temps actual per imprimir en les traces de seguiment
int treu_agenda(float ta, esdev *e){

#if DEBUGagenda == 1 
    printf("%.4lf Treu AGENDA %2d ", ta, ara);
    imprimir_element_agenda(ara);
    //printf(": ");
#endif
    if(ara == -1) {// agenda buida
        return(0); 
    }
    *e = agenda[ara];
    --ara;
    
#if DEBUGagenda == 1 
    //printf("%.4lf Treu AGENDA %2d: ", ta, ara);
    imprimir_agenda();
#endif
    
    return(1);
}//treu_agenda

void buida_agenda(void){
    ara = -1;
}

void allibera_agenda(void){
    free(agenda);
}

