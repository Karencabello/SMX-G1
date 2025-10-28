/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: saloha.exe <name-input-file> <name-output-file>
 * Example: saloha.exe ./src/in ./src/out
 * 
 * LLibreria de funcions estocastiques 
 * 
 * File:   stochastic.c
 * Author: Dolors Sala
 */

#include "./saloha.h"
#include "./cues.h"
#include "stats.h"
#include <math.h>

double decide_interarrival(sstation *s);

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

    d = drand();
    //d = drand48();
    return(-m*log(d));
} // expo

// It initializes the traffic generators of all stations using a different
// random seed for the traffic generator than other randomness of the program 
void init_traf(){
  int stn, i;

  // Decide the seed of the traffic stream: nslot rn's ahead of the seedval
  srand(seedval); // srand48(trafseed);
  for(i = 0; i <  nslots; i++)
        trafseed = rand(); //lrand48();
  
  MESSAGE("\nSeed for traffic generator : %ld\n",trafseed);

  for(stn = 0; stn < nstns; stn++){
      if(stns[stn].rate > 0)
	stns[stn].nextpkarv = (double)(decide_interarrival(&(stns[stn])));

#if (DEBUG == 1 || DEBUGSTN == stn || DEBUGTRAF == 1)
      TRACE("%4ld STN %2d INIT TRAF: Next pak Arrival %lf slots %d \n", \
              slot, stns[stn].stnnum, stns[stn].nextpkarv, \
              (int)ceil(stns[stn].nextpkarv));
              //(int)ceil(MSECtoSLOTS(stns[stn].nextpkarv));
#endif
  }

  trafseed = rand();  // lrand48(); // Keep state of the trafseed stream
  srand(seedval);   // srand48(seedval);   // reestablish the seed to its original value
} // init_traf

// Creates the arrival of the station: puts it in the queue
void create_arrival(sstation *s){
    //int dbstn = s->stnnum;
    equeue e;
    
    e = create_qu_element(s->tpk, stns[s->stnnum].nextpkarv);
    add_qu_element(&s->qu, e);
    s->tpk++;    
  
    // update stats  
    if(slot < start_stats)
        sts.gload[STSWARMUP][s->stnnum]++;
    else
        sts.gload[STSSTEADY][s->stnnum]++;

#if (DEBUG == 1 || DEBUGSTN == 1  || DEBUGTRAF == 1 || DEBUGCRA == 1 )
    TRACE("%4ld STN %2d PK  ARRIVAL: pk (num %3d, arv %9.6lf sarv %4d Tx-count %2d) QU ", \
            slot, s->stnnum, s->qu.pks[s->qu.tail].num, \
            s->qu.pks[s->qu.tail].arv_time, s->qu.pks[s->qu.tail].sarv_time, \
            s->qu.pks[s->qu.tail].txcount);
    print_queue(s->qu);
#endif
    
} // create_arrival


// Decides the interarrival time of the next packet of the particular station
// computing the interarrival time based on station rate and considering the 
// smothness of the starting process indicated by the flag
double decide_interarrival(sstation *s){
    double ia; 
    ia = expo(s->rate);
    //if(flag == FIRSTPK) ia = -ia;
    
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGTRAF == 1)
    TRACE("%4ld STN %2d NEXT INTARV: next-pk %d Next time %8.4lf ms %8.4lf ia %lf\n", \
            slot, s->stnnum, s->qu.pks[s->qu.tail].num+1, \
            s->nextpkarv, SLOTStoMSEC(s->nextpkarv), ia); // MSECtoSLOTS
#endif

      return(ia);
} // decide_interarrival


// Decides the time of the next packet arrival of the particular station
// computing the interarrival time based on station rate and adding from 
// previous arrival
void decide_next_arrival(sstation *s){
    //int dbstn = s->stnnum;
    s->nextpkarv += decide_interarrival(s);
    
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGTRAF == 1)
    TRACE("%4ld STN %2d NEXT PK ARV: next-pk %d Next time %8.4lf ms %8.4lf\n", \
            slot, s->stnnum, s->qu.pks[s->qu.tail].num+1, \
            s->nextpkarv, SLOTStoMSEC(s->nextpkarv));  // MSECtoSLOTS
#endif
} // decide_next_arrival


// Traffic generator generates packets according to the arrival rate in each 
// station
void gen_traf(){
  int s;
  //double tmp;
  
  seedval = rand();  // lrand48();
  srand(trafseed); // srand48(trafseed);
    
  for(s = 0; s < nstns; s++){
      while(ceil(stns[s].nextpkarv) == slot){
          create_arrival(&stns[s]);
          decide_next_arrival(&stns[s]);
      }
  }

  trafseed = rand();//  trafseed = lrand48();
  srand(seedval);// srand48(seedval);
  
} // gen_traf

