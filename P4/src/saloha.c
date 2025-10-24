/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: saloha.exe <name-input-file> <name-output-file>
 * Example: saloha.exe ./src/in ./src/out
 * 
 * Programa principal
 * 
 * File:   saloha.c
 * Author: Dolors Sala
 */

#include "./saloha.h"
#include "stats.h"
#include "cues.h"

long int seedval;       // random seed for all randomness except traffic generation
long int trafseed;      // random seed for traffic generation (initial seed taken from seedval random stream)

double    rho;          // System load to be generated specified by the user
long      slot;         // Slot number in the simulation, discrete simulation time
long      nslots;       // Required duration of the simulation in slots

// network definition
schannel  channel;      // The channel of the network: a stream of slots
long      nstns;        // Number of stations in the network
sstation *stns;         // Array of stations connected in the network

FILE     *ifile;        // File where to read input parameters
FILE     *ofile;        // File where to write output data

//double    TraceTime;    // Time to generate a trace as a % of simulation time

char      TrafGenType;  // bursty (E)xponencial, (P)areto


// Funtion to get all parameters from the simulation user
void input_parameters(int argc, char**argv){
    double aux=0;
    
    ofile = stdout;
    
    if(argc != 3)
        ERROR(NOSTATS, "%ld Execucion needs two input parameters: name of input and output files. Use: saloha.exe ./src/in ./src/out", slot);
    
    if(!strcmp(argv[1],"stdin"))
        ifile = stdin;
    else
        ifile = fopen(argv[1],"r");
    if(ifile == NULL)
        ERROR(NOSTATS,"%ld Input file (%s) not found.... check path!!", slot, argv[1]);
    
    if(!strcmp(argv[2],"stdout"))
        ofile = stdout;
    else{
        ofile = fopen(argv[2],"w");
        //stdout = ofile;
        //stderr = ofile;
    }
        
    time_t curtime;
    time(&curtime);
    MESSAGE("**************** START EXECUTION ********************\n ");    
    MESSAGE(" Time : %s", ctime(&curtime));
    MESSAGE("*****************************************************\n ");    

    
MESSAGE("NETWORK CONFIGURATION ---\n");
    MESSAGE("    Number of stations                      : ");
	fscanf(ifile,"%ld", &nstns);
	MESSAGE("%9ld\n", nstns);
    MESSAGE("    Slot Size (bytes)                       : ");
	fscanf(ifile,"%d", &channel.slot_size);
	MESSAGE("%9d\n", channel.slot_size);
    MESSAGE("    Transmission Rate (Mbps)                : ");
	fscanf(ifile,"%lf",&channel.rate);
	MESSAGE("%9.2lf\n",channel.rate);

    MESSAGE("    Contention Resolution Algorithm         : ");
        fscanf(ifile,"%s", &channel.cralg);
        MESSAGE("%9c (D-deterministic, P pPersistence, B BEB, O Optimal pPersistence)\n",channel.cralg);
    MESSAGE("    Probability of p-persistent protocol    : ");
        fscanf(ifile,"%lf", &channel.p);
        MESSAGE("%9.4lf (only for fixed p-persistence)\n",channel.p);

MESSAGE("TRAFFIC GENERATOR -------\n");
    MESSAGE("    Normalized offered load                 : ");
            fscanf(ifile,"%lf ", &rho);
            MESSAGE("%9.6lf ( %9.4lf Mbps)\n",rho, NORMtoMBPS(rho));

    MESSAGE("    Interarrival Distribution (E)Exp.       : ");
	fscanf(ifile,"%c", &TrafGenType); TrafGenType = toupper(TrafGenType);
	MESSAGE("%9c\n", TrafGenType);

MESSAGE("SIMULATION PARAMETERS ---\n");
    MESSAGE("    Length of simulation in miliseconds     : ");
        fscanf(ifile,"%lf", &aux);
        nslots = (int) ceil(MSECtoSLOTS(aux));
	MESSAGE("%9.2lf ( %9ld slots)\n", aux, nslots);

    MESSAGE("    Start statistics at milisecond          : ");
	fscanf(ifile,"%lf", &aux);
        start_stats = ceil(MSECtoSLOTS(aux));
	MESSAGE("%9.2lf ( %9ld slots)\n", aux, start_stats);

    MESSAGE("    Seed value (random = 0)                 : ");
    	fscanf(ifile,"%ld",&seedval);
	MESSAGE("%9ld",seedval);
        if(seedval == 0){
            seedval = time(0); 
            MESSAGE("(Random, seed chosen: %ld)",seedval);
        }
        MESSAGE("\n");

    MESSAGE("    Significance level (alpha)              : ");
	fscanf(ifile,"%lf", &sts.significance);
	MESSAGE("%9.6lf \n",sts.significance);

    MESSAGE("    Z value of %.3lf (1-alpha/2)            : ",(1-(sts.significance/2.0)));
	fscanf(ifile,"%lf", &sts.z);
	MESSAGE("%9.2lf \n",sts.z);

    MESSAGE("    Target CI accuracy r in %% [0..100]      : ");
	fscanf(ifile,"%lf", &sts.r);
	MESSAGE("%9.2lf \n",sts.r);

#if 0
MESSAGE("DEBUGGING FLAGS ---------\n");
    MESSAGE("    Trace channel time (percentage end sim) : ");
        fscanf(ifile,"%lf", &TraceTime);
        MESSAGE("%9.4lf\n",TraceTime);
#endif

    MESSAGE("\nEnd of input data---\n");

// Checking input data
// Largely not done
#if 1
        if(sts.r == 0)
            ERROR(NOSTATS, "ERROR r is zero and it cannot be............");
#endif
}// input_parameters

// Function used to initialize one station
void init_sta(sstation *stn,int snum){
    
    stn->stnnum    = snum;
    create_queue(stn,MAXQU);
    stn->p         = channel.p;
    stn->rate      = 1.0/ (rho/nstns); 
    stn->state     = 'I';
    stn->nextpkarv = 0;
    stn->tpk       = 0;
    stn->wait      = 0;
    stn->txtslot   = NA;

}//init_sta

// Generates a new slot as empty to start next slot with a clean current slot
void generate_new_slot(){
        
    channel.cslot.SA  = NA;
    channel.cslot.state = EMPTY;
    channel.cslot.DA    = NA;
    channel.cslot.pk.arv_time = NA;
    channel.cslot.pk.sarv_time = NA;
    channel.cslot.pk.iservtime = NA;
    channel.cslot.pk.num      = NA;
    channel.cslot.pk.txcount  = NA;
 
#if (DEBUGchannel == 1 || DEBUG == 1)
    TRACE("%4ld Creating new Slot empty...", slot);
    TRACE("channel SA %2d DA %2d S %2d pk (%3d, %8.4lf, %4d, %2d)\n", \
            channel.cslot.SA, channel.cslot.DA, channel.cslot.state,   \
            channel.cslot.pk.num, channel.cslot.pk.arv_time, \
            channel.cslot.pk.sarv_time, channel.cslot.pk.txcount);
    fflush(ofile);
#endif
    
}// generate_new_slot

// Function used to initialize all variables of the simulation based on input
void initialize(){
    int s;
       
    stns = (sstation *) malloc(nstns * sizeof(sstation));
    if(stns == NULL )
        ERROR(WITHSTATS,"%ld ERROR: allocating memory in initialize\n", slot);
    for (s = 0; s < nstns; s++)
        init_sta(&stns[s],s);
  
    generate_new_slot();
    srand(seedval); //srand48(seedval);

}// initialize

// Frees all dynamic memory created for the stations  
void free_stns(){
    int s;
     for (s = 0; s < nstns; s++){
         free_queue(&(stns[s].qu));    
     }
    free(stns);
}//free_stns

/********************** MAIN ***************************/
int main(int argc, char**argv) {
    int stn;
    //FILE *ftest;

    slot = 0;
    
    input_parameters(argc, argv);
    MESSAGE("Initializing......\n");
    initialize();
    init_traf();
    init_stats();
    MESSAGE(" ___________________________________________________\n\n");
    fflush(ofile);
    
    for(slot = 0; slot < nslots; slot++){
      
      generate_new_slot();

      // compute optimal p for optimal p-persistence  
      if(channel.cralg == 'O') 
          compute_optimal_p();
      
      gen_traf();

      for(stn = 0; stn < nstns; stn++){
        station(&stns[stn]);
      }
      run_sink();
    } // for nslots

    collect_stats();

    MESSAGE("\nProgram has finished Successfully!!!!!!!!!!!");
    free_stns();
    free_stats();    
    fclose(ofile);
    fclose(ifile);
    
    return(0);
} // main 


