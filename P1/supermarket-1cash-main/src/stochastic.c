/*
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * LLibreria de funcions estocastiques del sistema de cues
 * 
 * File:   stochastic.c
 * Author: Dolors Sala
 */

#include <math.h>
#include <stdlib.h>

// A normalized random function giving values in the range [0..1]
double drand(void){
    double ret = 0.0;
    double r = 0;
    
    r = rand();
    ret = r / (double) RAND_MAX;
    return(ret);
} // drand

// Provides the next random value of an exponential distribution of mean m
float expo(float m){
    static double d = 0.0;

    //float alea1(long int *ap_llavor);
    d = drand();
    return(-m*log(d));
}


