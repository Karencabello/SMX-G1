/* 
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 
 * Definicions per les estadistiques i resultats
 * 
 * File:   stats.h
 * Author: Dolors Sala
 */

#ifndef STATS_H
#define	STATS_H

/******************* Statistics           **********************************/
#define MAXPRINTCOL    10  // Nombre columnes per línia al imprimir vectors i histogrames

/******  Histogram dimensioning *********/
//#define MAXQUHIST      22  // Dimension of the queueing histogram array stats
//#define MAXDELHIST     3400  // Dimension of the delay histogram array statistics

#define STSWARMUP       0  // WARMP-up period statistics: needed for error checks
#define STSSTEADY       1  // STEADY state statistics
#define STSSTATES       2  // Statistics divided in ramp-up 0 and steady state 1

// Estructure grouping all measures and metrics related to statistics and 
// simulation output results
typedef struct{
    // statistics gathered along the simulation
    long  **qhist;  // queue length histogram [ntc][MAXQUHIST]
    long  **dqhist; // Delay histogram in queue in ?? units [ntc][MAXDELHIST]
    long  **dshist;  // Delay service histogram in ?? units [ntc][MAXDELHIST]
    long  **dthist;  // Delay total histogram in ?? units [ntc][MAXDELHIST]
    long   *nca;    // Number of served clients by each cashier [ntc]
    long   *gload;  // Clients generated at each cashier in slots [ntc]
    
    // statistics derived 
    double   utilization;      // Utilization
    double   oload;            // Total offered load
    double   av_qu_len;        // Average qu length over all stations in pks
    double   av_delay;         // Avg delay across stns in slots
    
}sstats;

extern long     start_stats;   // Time to start turning ON statistics gathering (end of warmup period)
extern sstats   sts;           // Variable with ALL statistics

void init_stats(sstats *sts, int ntc);
void collect_stats(sstats sts, int ntc);
//void collect_stats();
void free_stats(sstats sts, int ntc);
long samples(long *h, long dimh);
int myround(double d);
void print_hist(FILE *ofile, long *v, long length, char *msg, int num_col);
void time_header(char *when);
void inc_stats(long **v, int row, int col, int maxrow, int maxcol);
void actualitzar_stats_cua(scua *cues, int ntc, float tant, float ta, sstats *sts, int maxqu);
void print_configuracio(long int llavor, int ntc);
#endif	/* STATS_H */

