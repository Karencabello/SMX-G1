/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 * 
 * Programa principal
 * 
 * File:   saloha.c
 * Author: dolors
 */

//#include "./saloha.h"
#include "stats.h"
#include "cues.h"

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
        ERROR((ofile,"%d Execucion needs two input parameters: name of input and output files. Use: saloha.exe ./src/in ./src/out",slot));
    
    if(!strcmp(argv[1],"stdin"))
        ifile = stdin;
    else
        ifile = fopen(argv[1],"r");
    if(ifile == NULL)
        ERROR((ofile,"%d Input file (%s) not found.... check path!!",slot,argv[1]));
    
    if(!strcmp(argv[2],"stdout"))
        ofile = stdout;
    else{
        ofile = fopen(argv[2],"w");
        //stdout = ofile;
        //stderr = ofile;
    }
        
    time_t curtime;
    time(&curtime);
    MESSAGE((ofile,"**************** START EXECUTION ********************\n "));    
    MESSAGE((ofile, " Time : %s", ctime(&curtime)));
    MESSAGE((ofile,"*****************************************************\n "));    

    
MESSAGE((ofile,"NETWORK CONFIGURATION ---\n"));
    MESSAGE((ofile,"    Number of stations                      : "));
	fscanf(ifile,"%d", &nstns); 
	MESSAGE((ofile,"%9d\n", nstns));
    MESSAGE((ofile,"    Slot Size (bytes)                       : "));
	fscanf(ifile,"%d", &channel.slot_size); 
	MESSAGE((ofile,"%9d\n", channel.slot_size));
    MESSAGE((ofile,"    Transmission Rate (Mbps)                : "));
	fscanf(ifile,"%lf",&channel.rate);
	MESSAGE((ofile,"%9.2lf\n",channel.rate));
        
    MESSAGE((ofile,"    Contention Resolution Algorithm         : "));
        fscanf(ifile,"%s", &channel.cralg);
        MESSAGE((ofile,"%9c (D-deterministic, P pPersistence, B BEB, O Optimal pPersistence)\n",channel.cralg));
    MESSAGE((ofile,"    Probability of p-persistent protocol    : "));
        fscanf(ifile,"%lf", &channel.p);
        MESSAGE((ofile,"%9.4lf (only for fixed p-persistence)\n",channel.p));

MESSAGE((ofile,"TRAFFIC GENERATOR -------\n"));
    MESSAGE((ofile,"    Normalized offered load                 : "));
            fscanf(ifile,"%lf ", &rho);
            MESSAGE((ofile,"%9.6lf ( %9.4lf Mbps)\n",rho, NORMtoMBPS(rho)));

    MESSAGE((ofile,"    Interarrival Distribution (E)Exp.       : "));
	fscanf(ifile,"%c", &TrafGenType); TrafGenType = toupper(TrafGenType);
	MESSAGE((ofile,"%9c\n", TrafGenType));
        
MESSAGE((ofile,"SIMULATION PARAMETERS ---\n"));
    MESSAGE((ofile,"    Length of simulation in miliseconds     : "));
        fscanf(ifile,"%lf", &aux);
        nslots = (int) ceil(MSECtoSLOTS(aux));
	MESSAGE((ofile,"%9.2lf ( %9d slots)\n",aux,nslots));

    MESSAGE((ofile,"    Start statistics at milisecond          : "));
	fscanf(ifile,"%lf", &aux);
        start_stats = ceil(MSECtoSLOTS(aux));
	MESSAGE((ofile,"%9.2lf ( %9d slots)\n",aux,start_stats));

    MESSAGE((ofile,"    Seed value (random = 0)                 : "));
    	fscanf(ifile,"%d",&seedval);
	MESSAGE((ofile,"%9d",seedval));
        if(seedval == 0){
            seedval = time(0); 
            MESSAGE((ofile,"(Random, seed chosen: %d)",seedval));
        }
        MESSAGE((ofile,"\n"));
        
    MESSAGE((ofile,"    Significance level (alpha)              : "));
	fscanf(ifile,"%lf", &sts.significance);
	MESSAGE((ofile,"%9.6lf \n",sts.significance));

    MESSAGE((ofile,"    Z value of %.3lf (1-alpha/2)            : ",(1-(sts.significance/2.0))));
	fscanf(ifile,"%lf", &sts.z);
	MESSAGE((ofile,"%9.2lf \n",sts.z));

    MESSAGE((ofile,"    Target CI accuracy r in %% [0..100]      : "));
	fscanf(ifile,"%lf", &sts.r);
	MESSAGE((ofile,"%9.2lf \n",sts.r));

#if 0
MESSAGE((ofile,"DEBUGGING FLAGS ---------\n"));
    MESSAGE((ofile,"    Trace channel time (percentage end sim) : "));
        fscanf(ifile,"%lf", &TraceTime);
        MESSAGE((ofile,"%9.4lf\n",TraceTime));
#endif

    MESSAGE((ofile,"\nEnd of input data---\n"));

// Checking input data
// Largely not done
#if 1
        if(sts.r == 0)
            ERRORF((ofile,"ERROR r is zero and cannot be............"),NULL);
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
    TRACE((ofile, "%4d Creating new Slot empty...", slot));
    TRACE((ofile, "channel SA %2d DA %2d S %2d pk (%3d, %8.4lf, %4d, %2d)\n", 
            channel.cslot.SA, channel.cslot.DA, channel.cslot.state, 
            channel.cslot.pk.num, channel.cslot.pk.arv_time, 
            channel.cslot.pk.sarv_time, channel.cslot.pk.txcount));
    fflush(ofile);
#endif
    
}// generate_new_slot

// Function used to initialize all variables of the simulation based on input
void initialize(){
    int s;
       
    stns = (sstation *) malloc(nstns * sizeof(sstation));
    if(stns == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in initialize\n",slot));
    for (s = 0; s < nstns; s++)
        init_sta(&stns[s],s);
  
    generate_new_slot();
    srand(seedval); // srand48(seedval);

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
    FILE *ftest;

    slot = 0;
    
    input_parameters(argc, argv);
    MESSAGE((ofile,"Initializing......\n"));
    initialize();
    init_traf();
    init_stats();
    MESSAGE((ofile," ___________________________________________________\n\n"));
    //fflush(ofile);
    
    for(slot = 0; slot < nslots; slot++){
      
      generate_new_slot();

      gen_traf();

      for(stn = 0; stn < nstns; stn++){
        station(&stns[stn]);
      }
      run_sink();
    } // for nslots

    collect_stats();
       
    MESSAGE((ofile,"\nfinished!!!!!!!!!!!"));
    free_stns();
    free_stats();    
    fclose(ofile);
    fclose(ifile);
    
    return(0);
} // main 


