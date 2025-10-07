/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 * 
 * Functions that implement the contention resolucion algorithms
 * 
 * File:   cra.c
 * Author: dolors
 */

#include <math.h>
#include "saloha.h"
#include "cra.h"
//#include "stats.h"

// A deterministic CRA that returns the same ID number (n) as number of slots 
// to wait
int CRA_deterministic(int n){
             
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1 )
    TRACE((ofile,"%4d STN %2d CRA DETERMINISTIC: wait %2d \n", slot, n, n));
#endif 
    
    return(n);
} // CRA_deterministic

// Returns the number of slots to back off before retransmitting a packet
int backoff(int n, char alg){
    int wait = 0;
    
    switch(alg){
        case 'D': wait = CRA_deterministic(n);
        break;
        default:
            ERROR((ofile,"%d ERROR: CRA algorithm (%c) not known.",slot, alg));
            exit(0);
    }
    return(wait);
  
} // backoff
