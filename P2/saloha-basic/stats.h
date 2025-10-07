/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: saloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 
 * Definicions per les estadistiques i resultats
 * 
 * File:   stats.h
 * Author: Dolors Sala
 */

#ifndef STATS_H
#define	STATS_H

#include "./saloha.h"

/******************* Statistics           **********************************/

/******  Histogram dimensioning *********/
#define MAXQUHIST      200  // Dimension of the queueing histogram array stats
#define MAXDELHIST   10000  // Dimension of the delay histogram array statistics
#define MAXCOLHIST     23  // Max number of stations transmitting in the same slot
#define MAXATMHIST     300  // Dimension of the number of attempts to tx a pk

#define STSWARMUP       0  // WARMP-up period statistics: needed for error checks
#define STSSTEADY       1  // STEADY state statistics
#define STSSTATES       2  // Statistics divided in ramp-up 0 and steady state 1

// Estructure grouping all measures and metrics related to statistics and 
// simulation output results
typedef struct{
    // statistics gathered along the simulation
    long  ***qhist;  // queue length histogram in pks  [STSSTATES][nstns][MAXQUHIST]
    long  ***dhist;  // Access delay histogram in slots [STSSTATES][nstns][MAXDELHIST]
    long  ***shist;  // Service Time histogram in slots [STSSTATES][nstns][MAXDELHIST]
    long  ***ahist;  // Number of attempts it took to transmit pks [STSSTATES][nstns][MAXATMHIST]
    long   **snt;    // Paquets sent by each stn in slots [STSSTATES][nstns]
    long   **gload;  // Load generated at each station in slots [STSSTATES][nstns]
    long   **chhist; // State of the channel histogram [STSSTATES][MAXCOLHIST]
    long   **phist;  // Histogram of the n=1/p values in optimal p-persistence [STSSTATES][MAXCOLHIST]
    
    // statistics derived 
    double   utilization;      // Utilization
    double   oload;            // Total offered load
    double   av_qu_len;        // Average qu length over all stations in pks
    double   av_delay;         // Avg delay across stns in slots
    double   jitter_delay;     // Avg jitter across stns defined as max-min of delay in slots
    double   percentile_delay; // Avg (95th) percentile of delay across stns in slots
    double   stddev_delay;     // Standard deviation of the delay across stns in slots
    double   sav_delay;        // Avg service time across stns in slots
    double   sstddev_delay;    // Standard deviation of the service time across stns in slots
    
    //validity of results
    double   significance;     // Target significance level (alpha)
    double   z;                // The corresponding Z value of z distribution for double-sided CI
    double   adelCI;           // Confidence interval of average delay (response time)
    double   r;                // Target accuracy (r) for the CI 
    double   sdelCI;           // Confidence internval of service delay
 
}sstats;

extern long     start_stats;   // Time to start turning ON statistics gathering (end of warmup period)
extern sstats   sts;           // Variable with ALL statistics

void collect_stats();
void free_stats();
long samples(long *h,long dimh);
#endif	/* STATS_H */

