/*
 * Program for the verification of the pseudo-random number generators 
 * provided by gcc
 *  
 * Functions needed to generate random sequences (variates) following several 
 * Distribution functions
 * 
 * File:   stochastic.c
 * Author: Dolors Sala
 * 
 */

#include "./rng.h"

// extern FILE *ofile;

// Prints a vector of longs
void print_vector_l(double *v, int length, char *msg, int num_col){
    int i;
    //num_col = 1;
    
    fprintf(ofile,"%s",msg);
    for (i = 0; i < length; i++ ){
        if(i % num_col == 0)
            //fprintf(ofile,"\n");
            fprintf(ofile,"\n %3d ", i);
        fprintf(ofile,"%12ld ", (long)v[i]);
    }
    fprintf(ofile,"\n");
}// print_vector_l

// Prints a vector of ints
void print_vector_d(double *v, int length, char *msg, int num_col){
    int i;
    //num_col = 1;
    
    fprintf(ofile,"%s",msg);
    for (i = 0; i < length; i++ ){
        if(i % num_col == 0){
            //fprintf(ofile,"\n");
            fprintf(ofile,"\n %d", i);
            if(num_col > 1)
                fprintf(ofile,"%3d ", i);
        }
        fprintf(ofile,"%12.10lf ", v[i]);
    }
    fprintf(ofile,"\n");
}// print_vector_d

// A normalized random function giving values in the range [0..1]
double drandom(void){
    double ret = 0.0;
    double r = 0;
    
    r = random();
    ret = r / (double) RAND_MAX;
    return(ret);
} // drandom

double QuantizedInvExponential(double m, int bin, double size){
    double x = (bin + 0.5) * size;
    //double u = (1.0/m) * pow(EULER_NUM, -((1.0/m)*x));
    double u = -(1.0/m)*log(x);

    return(u);
}

double CdfExponential(double m, double x){
    double u = 1.0 - pow(EULER_NUM, -((1.0/m)*x));

    return(u);
}

double InvCdfExponential(double m, double x){
    double u = -(1.0/m)*log(x);

    return(u);
}

double RandInvCdfExponential(double m){
    double u;

    u = drand48();
    u = random();
    u = InvCdfExponential(m,u);
    return(u);
} 

double CdfPareto(double m, double u){
    double w;

    if((m < 1) || (u < 1)) 
        ERROR((ofile, "Pareto parameter (m %lf, u %lf) must be larger than 1",m,u));
    w = 1.0-pow(u,-m);
    return(w);
}

// Implements the inverse cdf function of pareto distribution
double InvCdfPareto(double m, double u){
    double w;
 
    if(m < 1) ERROR((ofile, "Pareto parameter (%lf) must be larger than 1",m));    
    w = 1.0 / pow((1.0-u),(1.0/m));
    return(w);
}

// Returns the next random variate of the Pareto distribution
double RandInvCdfPareto(double m){
    double u, w;

    if(m < 1) ERROR((ofile, "Pareto parameter (%lf) must be larger than 1",m));
    u = drand48();
    w = InvCdfPareto(m,u);
    return(w);
}

// Returns the next random number generator of the distribution function fd indicated
// m is one parameter needed by some distribution functions, the value is interpreted 
// differently by each distribution needing it
double random_sample( char fd, double m){
    double r = 0.0;
    //const long MAX_LONG = 33333;
    
    switch (fd){
        case 'U': // random number generator lrand48
            r = drand48();
            r = drandom();
            break;
        case 'R': // random number generator rand
            //printf("RAND_MAX = %d, Cutted to MAX_LONG %d",RAND_MAX, MAX_LONG);
            r = rand() % MAX_LONG;
            break;
        case 'E': // exponential distribution with mean m
            r = RandInvCdfExponential(m);
            break;
        case 'P': // pareto distribution with mean m             
            if(m < 1) ERROR((ofile, "Pareto parameter (%lf) must be larger than 1",m));
            r = RandInvCdfPareto(m);
            break;
        default:
            ERROR((ofile, "Distribution function not known %c",fd));
    }
    return(r);
} // random_sample

// Function to generate n numbers of a distribution function fd
void gen_num(double *v, char fd, char numtype, double m, int num_samples, long seed){
    //double *v;
    int i;
    
    // reinitialize seed to make sure all streams generated use the same initial seed 
    init_rngs(seed);
    
#if 0
    v = (double *) malloc(num_samples * sizeof(double));
    if (v == NULL) 
        ERROR((ofile, "Not enough space to create v in gen_num"));
#endif
    for (i = 0; i < num_samples; i++ ){
        v[i] = random_sample(fd,m);
    }
#if (DEBUG == 1 || DEBUGsamples == 1)
    fprintf(ofile,"%4d Samples generated of distribution function %c ",num_samples, fd);
    switch (numtype){
        case 'D':  // Double
            print_vector_d(v, num_samples, ":", NUM_COLUMNS);
            break;
        case 'L':  // Long              
            print_vector_l(v, num_samples, ":", NUM_COLUMNS);
            break;
        default: 
            ERROR((ofile, "Type of data not known %c", numtype));
    }
#endif
    if(fd == 'E'){
        FILE *f;
        f = fopen("./src/samples.txt","w+");
        for (i = 0; i < num_samples; i++ ){
            //fprintf(f,"%5d ", i);
            fprintf(f,"%lf\n", v[i]);
        }
        fprintf(f,"\n----------------------------------\n");
        fclose(f);    
    }
    //return(v);
}// gen_num
