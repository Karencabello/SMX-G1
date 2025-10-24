/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: saloha.exe <name-input-file> <name-output-file>
 * Example: saloha.exe ./src/in ./src/out
 * 
 * Functions that implement the contention resolucion algorithms
 * 
 * File:   cra.c
 * Author: Dolors Sala
 */

#include <math.h>
#include "saloha.h"
#include "cra.h"
#include "stats.h"

// A deterministic CRA that returns the same ID number (n) as number of slots 
// to wait
int CRA_deterministic(int n){
             
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1 )
    TRACE("%4ld STN %2d CRA DETERMINISTIC: wait %2d \n", slot, n, n);
#endif 
    
    return(n);
} // CRA_deterministic

// p-persistence returns either 0 or 1 and it must be called every slot to 
// to decide to transmit or to wait
int CRA_pPersistence(int n){
    double u = 0.0;
    int wait = -1;
    
    if(channel.p < 0 || channel.p > 1)
        ERROR(WITHSTATS,"%ld ERROR optimal P value is %lf", slot, channel.p);
    
    u = drand(); // drand48();  
    if (u < channel.p)
        wait = 0;
    else 
        wait = 1;
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1)
    if(channel.cralg == 'P')    
        TRACE("%4ld STN %2d CRA p-persistence: wait %2d (u %lf)\n", slot, n, wait, u);
    else if (channel.cralg == 'O')
        TRACE("%4ld STN %2d CRA optimal p-persistence: wait %2d (u %lf, p %lf)\n", slot, n, wait, u, channel.p);
    else ERROR(WITHSTATS, "%ld ERROR CRA %c and in p-persistence function with n %d", slot, channel.cralg, n);
#endif 
     
    return(wait);
} // CRA_pPersistence

// Computes at every slot how many stations will try to transmit and hence the 
// optimal p as 1/n where n is exact number of stations to transmit in this slot
// Puts this value in the p of the channel as it is used as the 
void compute_optimal_p(){
    int s;
    int c = 0;
    int state;
    
    for(s = 0; s < nstns; s++){
        if((stns[s].state == STNIDLE && stns[s].qu.lng > 0) ||
           (stns[s].state == STNIDLE && ceil(stns[s].nextpkarv) == slot) ||           
           (stns[s].state == STNTX ) ||           
           (stns[s].state == STNCRA ) )
            c++;
    }    
    channel.p = 1.0/c;   

#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1 )
    if(c > 0){    
        TRACE("%4ld CRA optimal p-persistence: n %d p %lf\n", slot, c, channel.p);    
    }
#endif 

    //update sts
    if(slot < start_stats)
        state = STSWARMUP;
    else 
        state = STSSTEADY;
    if(c >= MAXCOLHIST)
        ERROR(WITHSTATS,"%4ld ERROR: increase MAXCOLHIST %d as it gets a n optimal = %d", slot, MAXCOLHIST,c);
    sts.phist[state][c]++;
} // compute_optimal_p

int CRA_optimalPersistence(int n){
    
    int wait = CRA_pPersistence(n);
        
    return(wait);
} // CRA_optimalPersistence

// Truncated binary exponential backoff
int CRA_TBEB(int n){
    
    long u = rand(), wait;   // lrand48()
    int m = stns[n].qu.pks[stns[n].qu.head].txcount;
    float f;
    int ceiling = 10;
    
    f = MIN(m,ceiling); // truncated BEB to max = 2^ceiling;
    f = pow(2,f);
    wait = u % (int)f;
    //fprintf(ofile, "f %.1lf m %d u %d wait %d\n", f, m, u, wait);
     
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1 )
    //if(n == 0)
    TRACE("%4ld STN %2d CRA TBEB: wait %2ld (u %ld, m %d, pk %d, txcount %d) \n", \
            slot, n, wait, u, m, stns[n].qu.pks[stns[n].qu.head].num, m);
#endif 
    return(wait);
} // CRA_TBEB

// Returns the number of slots to back off before retransmitting a packet
int backoff(int n, char alg){
    int wait = 0;
    
    switch(alg){
        case 'D': wait = CRA_deterministic(n);
        break;
        case 'P': wait = CRA_pPersistence(n);
        break;
        case 'O': wait = CRA_optimalPersistence(n);
        break;
        case 'B': wait = CRA_TBEB(n);
        break;
        default:
            ERROR(WITHSTATS, "%ld ERROR: CRA algorithm (%c) not known.", slot, alg);
            exit(0);
    }
    return(wait);
  
} // backoff
