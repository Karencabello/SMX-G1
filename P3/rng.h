/*
 * Program for the verification of the pseudo-random number generators 
 *   
 * File:   rng.h
 * Author: Dolors Sala
 */

#ifndef _SXS_SP2_RNG_H
#define	_SXS_SP2_RNG_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NA       -1
#define SOLUTION  1
#define RUNRAND   1 // execute or not the RAND analysis
#define RUNRAND48 1 // execute or not the rand48 analysis

// DEBUG flags to generate traces when ON: ON = 1, OFF = 0
#define DEBUG        1 // TRACE with all messages
#define DEBUGsamples 1 // TRACE to debug/track the generation of random samples
#define DEBUGhist    1 // TRACE to debug histograms

// Identifiers of the different random number generators (RNG)
#define RNG_D48      0 // RNG based on drand48
#define RNG_RND      1 // RNG based on rand
#define NUM_RNGS     2 // how many different RNGs

// MACROS to generate traces and program output
// Use ERROR to print a fatal error and terminate the program
// Use MESSAGE to print out information
#define ERROR(message)({fprintf(ofile, "\nERROOORRRRRRRRRR\n");fprintf message;fflush(ofile);exit(0);})
#define MESSAGE(message) ({printf message;})

// Program constants
#define MAX_LONG        RAND_MAX  // Maximum number of the drand48
#define EULER_NUM       2.7182818 // e constant
#define NUM_COLUMNS     10        // Number of columns to print the vectors/histograms (typically set to 10)
                                  // set to 1 when you want to gather the raw random numbers to operate outside

#define FILENAME        "out.txt" // Name of the output file

extern FILE *ofile;                      // file to store all program output

void init_rngs(long seed);
//double  CdfExponential(double m, double x);
double  CdfExponential(double m, double x);
double  RandInvCdfExponential(double m);
double  RandInvCdfPareto(double m);
double  CdfPareto(double m, double x);
void print_vector_l(double *v, int length, char *msg, int num_col);
void print_vector_d(double *v, int length, char *msg, int num_col);
double verify_RNG(double *v, int num_samples, int num_bins, double size, char dist);
//double corr(double *v, double *w, long dimh, int num_bins, double size);
double corr(double *v, double *w, long dimh);
//double  random_sample( char fd, double m);
//double *gen_num(char fd, char numtype, double m, int num_samples);

#endif	// _SXS_SP2_RNG_H 

