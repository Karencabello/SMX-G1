/* 
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * Declaracions per la implementacio de l'agenda d'events
 * 
 * File:   agenda.h
 * Author: Dolors Sala
 */

#ifndef AGENDA_H
#define	AGENDA_H

// Declaracions per l'agenda d'events

typedef struct {
    float quan;
    int que;
    int on;          // caixa/servidor que passa l'esdeveniment
}esdev;

void ini_agenda(int n);
esdev crea_esdev(int que, float quan, int on);
void posa_agenda(float ta, esdev e);
int treu_agenda(float ta, esdev *e);
void buida_agenda(void);
void allibera_agenda(void);

static esdev *agenda; // Llista d'events ordenats cronologicament
#endif	/* AGENDA_H */

