/* 
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 * 
 * Fitxer amb les declaracions del programa principal
 * 
 * File:   saloha.h
 * Author: dolors
 */

#ifndef SLOHA_H
#define	SLOHA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "stats.h"

// MACROS used for the printouts instead of using f/printfs so that all prints
// in the program are treated the same way.
// Use TRACE when this printout is used for debugging and it is turned off for final runs
// Use MESSAGE when the print out is part of the output of the program
// Use ERROR when the print out informs of a problem in the program and it must abort but printing statistics before finishing
// Use ERRORF when the print out informs of a problem in the program and it must abort without any stats printing
// WARNING currently not used, but can be used to provide non-fatal errors in the program and the program can continue

/* usage: ERROR(("Warning: Note the two brackets\n")) */
#define ERROR(message)({fprintf(ofile,"\nERROOORRRRRRRRRR\n");fprintf message;\
                        collect_stats();\
                        printf("\nERROOORRRRRRRRRR see output file\n");exit(0);})
#define ERRORF(message, stsflag)({fprintf(ofile,"\nERROOORRRRRRRRRR\n");fprintf message;\
                        if(stsflag)collect_stats();\
                        printf("\nERROOORRRRRRRRRR see output file\n");exit(0);})
#define WARNING(message1, message2) ({fprintf(ofile, message1,message2);})
#define TRACE(message) ({fprintf message;fflush(ofile);}) // To create debugging traces
#define MESSAGE(message) ({fprintf message;}) // To generate program output

// ****** debugging flags *************************
// Flags to active the traces ON=1 OFF=0
#define DEBUG           0   // Debugging all (prints all traces)            
#define DEBUGTRAF       0   // Debugging traffic generator              
#define DEBUGchannel    0   // Debugging channel           
#define DEBUGSTN        0   // Debugging the station protocol -1=OFF       
#define DEBUGqueuing    0   // Debugging the queuing when grouping reqs 
#define DEBUGCRA        0   // Debugging contention resolution algorithm 
#define DEBUGSTS        0   // Debugging statistics

//int     DEBUGFIRSTSTN=  0;  /* Debugging first station to printout msgs  */
//int     DEBUGLASTSTN =  200;  /* Debugging last  station to printout msgs  */

/****** array dimensioning *********/
#define MAXQU        200  // queue size at stn 

/********* macros ********/
#define MAX(x, y)  (((x) > (y)) ? (x) : (y)) // computes the max of x and y
#define MIN(x, y)  (((x) < (y)) ? (x) : (y)) // computes the max of x and y

// MACROS for change of units
#define NA            -1            // Value not applicable
#define BYTE          8

#define MEGA          (exp(6))      // Translation from bits to Mbits        
#define MS_TIME       ((double)(channel.slot_size * BYTE)/(double)channel.rate) /* time: from slot to microsec */
#define NORMtoMBPS(u) (double)((u) * channel.rate)       // normalized util: from slots to Mbps 
#define MSECtoSLOTS(u) ((double)(u) / ((double)MS_TIME / 1000.0)) // from milisec to Slots

/*********** constant values set at the beginning of the simulation *********/

// Channel (current slot) state
#define  EMPTY       0 // No station has transmitted
#define  SUCCESS     1 // One station has trasmitted
#define  COLLISION   2 // Multiple stations have transmitted
//#define  TOTAL       3 // at the end, sum of all slots gathered of this type  
//#define  MAXMSSTATES TOTAL // slots states:0-empty / 1-busy / 2-colision      

// Station states: (I)idle (T)ransmit (R)esolution
#define STNIDLE   'I' // Idle state
#define STNTX     'T' // Transmitting state
#define STNCRA    'R' // Resolution state
//#define STNSTATES 3   // Number of station states
#define CHSTATES  3  // number of states the channel can be: I, TX, Collision

#define SINK_ADDR (nstns) // The identification of one extra station to be destination of all msgs

/******************* vector file handles ***********************************/
//FILE *traces;          /* file with all the traces temporal statistics     */

/***************** DECLARATIONS	********************************************/

/********  Station Structure  *********************************************/
// Definition of one register in the station queue: defines all fields defining a paquet
typedef struct{
  int    num;           // paquet number to identify the different paquests
  double arv_time;      // paquet arrival time (fraction of slos) 
  int    sarv_time;     // paquet arrival time (in slot units)
  int    iservtime;     // Time service starts (in slots)
  int    txcount;       // Times this paquet has been transmitted (in slots)
}equeue;

// Definition of the station queue
typedef struct{
    equeue *pks;        // List of paquets in queue: circular array from head to tail
    int lng;            // Queue length: Number of elements in the queue
    int head;           // Head of the queue to eliminate elements
    int tail;           // Tail of the queue to add elements
    int max;            // Dimension of the queue array
}squeue;

// Definition of a station
typedef struct {
  int    stnnum;         // Position of the station in array 
  double rate;           // Mean packet arrival rate, exponential
  double nextpkarv;      // Next paquet arrival in this station (in miliseconds)
  char   state;		 // States are : (I)idle (R) counting down (T)ransmit
  squeue qu;             // Queue of paquets pending to be transmitted 
  long   tpk;            // Total paquets created for this station: pk number
  double p;              // Value of p-persistence used by this station
  int    txtslot;        // Slot transmission time of the current transmission
  int    wait;           // Number of slots to wait until next retransmission
} sstation;

// ----- SLOT and channel STRUCTURE -------------
// Definition of a (channel) slot and transmitting information
typedef struct {
  int     state;    // Transmission state in this slot: 0-Empty; 1-Busy; x-multiplicity of collision         
  int     SA;       // Source station address/num     
  int     DA;       // Destination station address/num
  equeue  pk;       // Content that identifies a pk (for checking purposes only)
} sslot;

// Definition of the (network) channel: it only models one (the current) slot
typedef struct{
    double rate;      // Transmission rate in Mbps
    int    slot_size; // Size of the slot in bytes
    char   cralg;     // Contention resolution alg. : P p-persistence; B binary exponential backoff; O optimal p-persistence; D Deterministic
    double p;         // General p persistent value
    sslot  cslot;     // Actual channel as a stream of slots: it only needs 1 slot                      
}schannel;

extern int seedval;       // random seed for all randomness except traffic generation

extern double    rho;          // System load to be generated specified by the user
extern long      slot;         // Slot number in the simulation, discrete simulation time
extern long      nslots;       // Required duration of the simulation in slots

// network definition
extern schannel  channel;      // The channel of the network: a stream of slots
extern long      nstns;        // Number of stations in the network
extern sstation *stns;         // Array of stations connected in the network

extern FILE     *ifile;        // File where to read input parameters
extern FILE     *ofile;        // File where to write output data

//extern double    TraceTime;    // Time to generate a trace as a % of simulation time

extern char      TrafGenType;  // bursty (E)xponencial, (P)areto

void init_traf();
void init_stats();
void gen_traf();
void run_sink();
void station(sstation *s);
void compute_optimal_p();
#endif	/* SLOHA_H */

