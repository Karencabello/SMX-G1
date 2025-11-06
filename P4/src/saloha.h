/* 
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: saloha.exe <name-input-file> <name-output-file>
 * Example: saloha.exe ./src/in ./src/out
 * 
 * Fitxer amb les declaracions del programa principal
 * 
 * File:   saloha.h
 * Author: Dolors Sala
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
// Use TRACE when this printout is used for debugging and it is turned off for final runs, it does a flush to get the message immediately
// Use MESSAGE when the print out is part of the output of the program
// Use ERROR when the print out informs of a problem in the program and it must abort but printing statistics before finishing

#define WITHSTATS   1 // when an error occurs, print stats before exiting
#define NOSTATS     0 // when an error occurs, do not print stats before exiting
                        
// =======================================================================
// ERROR: fatal error, it logs in ofile, collects statistics if stats flag is 1, and exits
// usage: ERROR(NOSTATS, "Invalid parameter '%s'", "filename.txt");
// =======================================================================
#define ERROR(stsflag, fmt, ...) do { \
    fprintf(ofile, "\n[ERROR] (%s:%d in %s)\n", __FILE__, __LINE__, __func__); \
    fprintf(ofile, fmt, ##__VA_ARGS__); \
    fprintf(ofile, "\n---------------------------------------------\n"); \
    if (stsflag) collect_stats(); \
    fprintf(stderr, "[ERROR] see output file for details\n"); \
    exit(EXIT_FAILURE); \
} while (0)

// =======================================================================
// MESSAGE: normal informational output in ofile
// usage: MESSAGE("Simulation started with %d stations", nstns);
// =======================================================================
#define MESSAGE(fmt, ...) do { \
    fprintf(ofile, fmt, ##__VA_ARGS__); \
} while (0)

// =======================================================================
// TRACE: debugging trace messages in ofile (flush immediately)
// usage: TRACE("Value of x is %d", x);
// =======================================================================
#define TRACE(fmt, ...) do { \
    MESSAGE(fmt, ##__VA_ARGS__); \
    fflush(ofile); \
} while (0)

// Operations flags
#define NA            -1            // Value not applicable

// ****** debugging flags *************************
// Flags to active the traces ON=1 OFF=0
#define ON              1   // Debugging ON = 1, the traces are printed in output file
#define OFF             0   // Debugging OFF = 0, no traces are printed in output file
#define DEBUG           OFF   // Debugging all (prints all traces): ON/OFF         
#define DEBUGTRAF       OFF   // Debugging traffic generator: ON/OFF              
#define DEBUGchannel    OFF   // Debugging channel: ON/OFF
#define DEBUGqueuing    OFF   // Debugging the queuing when grouping reqs: ON/OFF 
#define DEBUGCRA        OFF   // Debugging contention resolution algorithm : ON/OFF
#define DEBUGSTS        OFF   // Debugging statistics: ON/OFF     
#define DEBUGSTN        NA   // Debugging the station protocol -1=OFF=NA=-1 (not fully implemented in full version, leave it always NA)      

//int     DEBUGFIRSTSTN=  0;  /* Debugging first station to printout msgs  */
//int     DEBUGLASTSTN =  200;  /* Debugging last  station to printout msgs  */

/****** array dimensioning *********/
#define MAXQU       30  // queue size at stn 

/********* macros ********/
#define MAX(x, y)  (((x) > (y)) ? (x) : (y)) // computes the max of x and y
#define MIN(x, y)  (((x) < (y)) ? (x) : (y)) // computes the max of x and y

// MACROS for change of units
#define BYTE          8

#define MEGA          (exp(6))      // Translation from bits to Mbits        
#define MS_TIME       ((double)(channel.slot_size * BYTE)/(double)channel.rate) /* time: from slot to microsec */
#define NORMtoMBPS(u) (double)((u) * channel.rate)       // normalized util: from slots to Mbps 
#define MSECtoSLOTS(u) ((double)(u) / ((double)MS_TIME / 1000.0)) // from milisec to Slots
#define SLOTStoMSEC(u) ((double)(u) * ((double)MS_TIME / 1000.0))  // from slots to milisec 

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

extern long int seedval;       
extern long int trafseed;      

extern double    rho;          
extern long      slot;         
extern long      nslots;       

// network definition
extern schannel  channel;      
extern long      nstns;        
extern sstation *stns;         

extern FILE     *ifile;        
extern FILE     *ofile;        

//extern double    TraceTime;  
extern char      TrafGenType;  

void init_traf();
void init_stats();
void gen_traf();
void run_sink();
void station(sstation *s);
void compute_optimal_p();
#endif	/* SLOHA_H */

