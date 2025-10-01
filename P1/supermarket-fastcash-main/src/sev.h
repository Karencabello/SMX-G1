/*  
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * Declaracions generals del program
 * 
 * File:   sev.h
 * Author: Dolors Sala 
 */
#ifndef _SEV
#define _SEV

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//--------------------- Configuració del programa --------------------
// Aquest paràmetres permeten definir diferents escenaris d'execució
#define OUTFILENAME "log/dbg.txt" // Nom del fitxer de sortida
#define RANSEED     56721   // Llavor del generador de números aleatorio // Zero for random start

#define OBRIRTIME      0.0    // temps d'obertura del supermercat
#define TANCARTIME    1000.0  // Temps de tancar el supermercat

#define ARRIVAL       15    // Temps mig entre arribades
#define SERVICE       60     // Temps mig de servei

/******  Dimensions dels Vectors *********/
#define CUA_MAX        10    // nombre maxim elements a la cua
#define N              10     // Nombre maxim d'events pendents d'executar (a function of ntc)

#define MAXQUHIST      5000    // Dimension of the queueing histogram array stats
#define MAXDELHIST     50000  // Dimension of the delay histogram array statistics

// Configuració pel seguiment de l'execució (Debugging), banderas : 0=NO, 1=SI
#define DEBUGserv      1      // Bandera per fer seguiment del servei
#define DEBUGagenda    1      // Bandera per fer seguiment de l'agenda
#define DEBUGcua       1      // Bandera per fer seguiment de la cua
#define DEBUGquinaCua  1      // Bandera per fer seguiment per decidir la cua on posar clients
#define DEBUGalea      1      // Visualitza els streams aleatoris, desactivar totes les altres per utilitzar aquesta
#define anyDEBUG       (DEBUGserv + DEBUGagenda + DEBUGcua + DEBUGquinaCua + DEBUGalea) 

//--------------------- Constants de programació ---------------------
#define NA            -1   // Value not applicable

// Parametres del sistema de cues
#define OBRIR     'O'
#define ARRIBADA  'A'
#define SORTIDA   'S'
#define TANCAR    'T'
#define N_RAPIDS 2

extern FILE *ofile;              // Fitxer per debuggar
// Use ERROR when the print out informs of a problem in the program and it must abort but printing statistics before finishing
// Use ERRORF when the print out informs of a problem in the program and it must abort without any stats printing
// WARNING currently not used, but can be used to provide non-fatal errors in the program and the program can continue

/* usage: ERROR(("Warning: Note the two brackets\n")) */
#define ERROR(message)({fprintf(ofile,"\nERROOORRRRRRRRRR\n");fprintf message;\
                        printf("\nERROOORRRRRRRRRR see output file\n");exit(0);})

#define WARNING(message1, message2) ({fprintf(ofile, message1,message2);})
#define TRACE(message) ({fprintf message;fflush(ofile);}) // To create debugging traces
#define MESSAGE(message) ({fprintf message;}) // To generate program output

// Estructura d'un element de la cua
typedef struct{
    float tar; // temps d'arribada a la cua
    float tse; // temps de servei
    int on;    // caixer
}el_cua; 

typedef struct{
    int idcua;    // posició de la cua per debug
    int max_cua;  // capacitat maxima d'elements a la cua
    int ini_cua;  // Primer element de la cua
    int fin_cua;  // ultim element de la cua
    int lon_cua;  // Quantitat d'elements a la cua 
    el_cua *elem; // Elements guardats a la cua
    int caixa;    // Estat de la caixa: 
} scua;

float expo(float m);
   
#endif



