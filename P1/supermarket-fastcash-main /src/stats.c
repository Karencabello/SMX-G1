/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha
 * 
 * Funcions per la captacio d'estadistiques i generacio de resultats
 * 
 * File:   stats.c
 * Author: Dolors Sala
 */ 

#include <time.h>
#include <math.h>
#include "sev.h"
#include "stats.h"
#include "cua.h"

FILE *ofile = NULL;
long     start_stats;   // Time to start turning ON statistics gathering (end of warmup period)
sstats   sts;           // Variable with ALL statistics

// Returns the number of samples in the histogram
long samples(long *h,long dimh){
    long i;
    long s = 0;
    
    for(i = 0; i < dimh; i++)
        s += h[i];        
    return(s);
} // samples

// Computes the maximum of a histogram
long max_hist(long *h,long dimh){
    long i;
    
    for(i = dimh-1; i >= 0; i--){
        if( h[i] != 0) return(i);
    }
    return(i);
} // max_hist

// Computes de minimum of a histogram
long min_hist(long *h,long dimh){
    long i;
    
    for(i = 0; i < dimh; i++){
        if( h[i] != 0) return(i);
    }
    return(i);
} // min_hist

// Computes de mean of a histogram
double mean_hist(long *h,long dimh){
    long i, samples = 0;
    double m = 0.0;
    
    for(i = 0; i < dimh; i++){
        m += (h[i] * i);
        samples += h[i];
    }
    if(samples > 0){
        m = m / samples;
    }
    else{
        if(m > 0){
            printf("ERROR mean_hist: m %lf > = but samples = %ld", m, samples);
        }
    }
    return(m);
} // mean_hist

// Suma el contingut del vector
long suma_vect(long *v,long dimh){
    long i;
    long m = 0;
    
    for(i = 0; i < dimh; i++)
        m += v[i];        
    return(m);
} // suma_vect


// Computes de mean of elements of a vector
double mean_vect(long *v,long dimh){
    long i;
    double m = 0.0;
    
    for(i = 0; i < dimh; i++)
        m += v[i];        
    return(m/dimh);
} // mean_vect

// Provides the closest integer of the double:
// round (1.6) = 2; round(1.2)= 1
int myround(double d){
    int ret = (int)floor(d);
    if(d - ceil(d) >= 0.5){
        ret++;
    }
    return ret;
} // round

// Prints a vector of longs of length length (in lines) with num_col numbers 
// providing a message to add to personalize printing for different calls 
// of same function
void print_vector(FILE *ofile, long *v, long length, char *msg, int num_col){
    int i;
    long max = length;
    
    max = max_hist(v,length) + 1;
    if(max < length - 1) max += 1; // want to visualize at least one zero.
    fprintf(ofile, "%s", msg);
    for (i = 0; i < max; i++ ){
        if(i % num_col == 0)
            fprintf(ofile, "\n%3d | ", i);
        fprintf(ofile, "%6ld ", (long)v[i]);
    }
    fprintf(ofile, "\n\n");
    fflush(ofile);
} // print_vector

// Prints a histogram of longs of length length (in lines) with num_col numbers 
// computing the mean of the histogram and adding a message given
// to add to personalize printing for different calls of same function
void print_hist(FILE *ofile, long *v, long length, char *msg, int num_col){
    int i;          
    double CI, mean;
    
    fprintf(ofile, "%s", msg);
    mean = mean_hist(v,length);
    fprintf(ofile, " Mean: %.2lf, Samples: %ld,", mean, samples(v,length));
    fprintf(ofile, " Min: %ld, Max %ld\n", min_hist(v,length), max_hist(v,length));
    print_vector(ofile, v, length,"",num_col);
    //fprintf(ofile, "\n");
    fflush(ofile);
} // print_hist

// Init of the statistics
void init_stats(sstats *stats, int ntc){
    int h,i,j;
    sstats sts = *stats;
 
    sts.nca = (long *) calloc(ntc, sizeof(long));
    if(sts.nca == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    
#if 1
    sts.gload= (long *) calloc(ntc, sizeof(long));
    if(sts.gload == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
#endif
    
    sts.qhist = (long **) malloc(ntc * sizeof(long*));
    if(sts.qhist == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    for(j = 0; j < ntc; j++){                
        sts.qhist[j]= (long *) calloc(MAXQUHIST, sizeof(long));        
        if(sts.qhist[j] == NULL )
            ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    }
    sts.dqhist = (long **) malloc(ntc * sizeof(long*));
    if(sts.dqhist == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    for(j = 0; j < ntc; j++){
        sts.dqhist[j]= (long *) calloc(MAXDELHIST, sizeof(long));
        if(sts.dqhist[j] == NULL )
            ERROR((ofile,"ERROR: allocating memory in init_stats\n"));    
    }
     
    sts.dshist = (long **) malloc(ntc * sizeof(long*));
    if(sts.dshist == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    for(j = 0; j < ntc; j++){
        sts.dshist[j]= (long *) calloc(MAXDELHIST, sizeof(long));
        if(sts.dshist[j] == NULL )
            ERROR((ofile,"ERROR: allocating memory in init_stats\n"));    
    }  
        
    sts.dthist = (long **) malloc(ntc * sizeof(long*));
    if(sts.dthist == NULL )
        ERROR((ofile,"ERROR: allocating memory in init_stats\n"));
    for(j = 0; j < ntc; j++){
        sts.dthist[j]= (long *) calloc(MAXDELHIST, sizeof(long));
        if(sts.dthist[j] == NULL )
            ERROR((ofile,"ERROR: allocating memory in init_stats\n"));    
    }  
    sts.av_delay  = 0.0;
    sts.av_qu_len = 0.0; 
    *stats = sts;
}// init_stats

// Frees all dynamic memory allocated with init_stats
void free_stats(sstats sts, int ntc){
     
    int h,i,j;
    free(sts.gload);
    
    for(j = 0; j < ntc; j++)
        free(sts.qhist[j]);  
    free(sts.qhist);
 
    for(j = 0; j < ntc; j++){
        free(sts.dqhist[j]);  
        free(sts.dshist[j]);  
        free(sts.dthist[j]);  
    }       
    free(sts.dqhist);
    free(sts.dshist);
    free(sts.dthist);
     
} // free_stats

// Function that returns the histogram of the sum of all histograms of the 
// different stations of a measure
// It is called for diferent types of histograms and hence it generates a sum
// histogram of diferent dimension. Hence, if the sum hist passed is not NULL
// it releases the memory before allocating another space.
long *sum_stn_hists(long *sum, long **h, long dimh, int numstns){
    //long *sum;
    int s,d;
    
    if(sum != NULL)
        free(sum);
    sum = (long *) calloc(dimh,sizeof(long));        
    if(sum == NULL )
        ERROR((ofile,"ERROR: allocating memory in sum_stn_hists\n"));
    
    for(s = 0; s < numstns; s++)
        for(d = 0; d < dimh; d++)
            sum[d] += h[s][d];
    return(sum);
} // sum_stn_hists

// Collect and print the statistics
void collect_stats(sstats sts, int ntc){
    int s, c;
    long *sum;
           
    sum = NULL;
    
    fprintf(ofile,"\n");
    MESSAGE((ofile, "-----------------------------------------------------------\n"));
    MESSAGE((ofile, "----- Resum dels Resultats de la Simulació ----------------\n"));
    MESSAGE((ofile, "-----------------------------------------------------------\n"));

    fprintf(ofile,"Nombre de clients atesos: ");
    fprintf(ofile,"(total %ld promig %6.1lf)", suma_vect(sts.nca,ntc), mean_vect(sts.nca, ntc));
    fprintf(ofile,"\n");
    print_vector(ofile, sts.nca, ntc,"", MAXPRINTCOL);    

    // Temps promig
    fprintf(ofile,"Numbre promig de clients a cua/caixa             : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8.1lf", mean_hist(sts.qhist[s], MAXQUHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps promig de cua a cada caixa                 : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8.1lf", mean_hist(sts.dqhist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps promig de servei a cada caixa              : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8.1lf", mean_hist(sts.dshist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps promig al super dels clients de cada caixa : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8.1lf", mean_hist(sts.dthist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"\n");
    
    // Temps min
    fprintf(ofile,"Nombre minim de clients a cua/caixa              : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", min_hist(sts.qhist[s], MAXQUHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps minim de cua a cada caixa                  : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", min_hist(sts.dqhist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps minim de servei a cada caixa               : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", min_hist(sts.dshist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps minim al super dels clients de cada caixa  : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", min_hist(sts.dthist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"\n");

    // Temps max
    fprintf(ofile,"Nombre maxim de clients a la cua/caixa           : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", max_hist(sts.qhist[s], MAXQUHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps maxim de cua a cada caixa                  : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", max_hist(sts.dqhist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps maxim de servei a cada caixa               : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", max_hist(sts.dshist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    
    fprintf(ofile,"Temps maxim al super dels clients segons caixa   : ");
    for(s = 0; s < ntc; s++){     
        fprintf(ofile,"%8ld", max_hist(sts.dthist[s], MAXDELHIST));
    }
    fprintf(ofile,"\n");
    fprintf(ofile,"\n");
    
    MESSAGE((ofile, "-----------------------------------------------------------\n"));
    MESSAGE((ofile, "------- Totes les Estadístiques calculades ----------------\n"));
    MESSAGE((ofile, "-----------------------------------------------------------\n"));
        
    for(s = 0; s < ntc; s++){
        fprintf(ofile, "CUA %d nombre clients en espera a cua ", s);
        print_hist(ofile, sts.qhist[s], MAXQUHIST,"", MAXPRINTCOL);    
    }
    
    for(s = 0; s < ntc; s++){
        fprintf(ofile, "CUA %d temps d'espera a cua ", s);
        print_hist(ofile, sts.dqhist[s], MAXDELHIST,"", MAXPRINTCOL);    
    }

    for(s = 0; s < ntc; s++){
        fprintf(ofile, "CUA %d temps d'espera de servei a caixa ", s);
        print_hist(ofile, sts.dshist[s], MAXDELHIST,"", MAXPRINTCOL);    
    }
    
    for(s = 0; s < ntc; s++){
        fprintf(ofile, "CUA %d temps total al super segons caixa ", s);
        print_hist(ofile, sts.dthist[s], MAXDELHIST,"", MAXPRINTCOL);    
    }
    
    time_header("END");
    
}// collect_stats

void time_header(char *when){   
    time_t curtime;
    time(&curtime);
    MESSAGE((ofile,"**************** %4s EXECUTION *****************\n", when));    
    MESSAGE((ofile, "     Time : %s", ctime(&curtime)));
    MESSAGE((ofile,"*************************************************\n"));  
} // time_header

void inc_stats(long **v, int row, int col, int maxrow, int maxcol){
 
    if(row > NA && row >= maxrow)
        ERROR((ofile, "ERROR: Row %d greater than max vect dimension %d. Increase vector size.", 
                row, maxrow));
    if(col > NA && col >= maxcol)
        ERROR((ofile, "ERROR: Column %d greater than max vect dimension %d. Increase vector size.",
                col, maxcol));
    //v[row][col]++;                
    if(row > NA && col > NA)
        v[row][col]++;   
    else if(col == NA && row > NA) // to use the same function to increment a one- and two- dimension stats
        (*v)[row]++;
} // inc_stats

// Actualitza les estadístiques del tamany de cua des de l'event/temps anterior 
// tant a l'event/temps actual ta
void actualitzar_stats_cua(scua *cues, int ntc, float tant, float ta, sstats *sts, int maxqu){
    int t, c;
    int posh;
    
    for(c = 0; c < ntc; c++){
        posh = cues[c].lon_cua;
        if(posh >= maxqu){
            printf("%lf ERROR: actualitzar stats incrementant pos %d i es fora de rang %d", 
                    ta, posh, maxqu);
            exit(0);
        }
        sts->qhist[c][posh] += (floor(ta) - floor(tant));
    }
    
} // actualitzar_stats_cua

void print_configuracio(long int llavor, int ntc){
    fprintf(ofile, "----------------------------------------------------------\n");
    fprintf(ofile, "---------- Configuracio Programa -------------------------\n");
    fprintf(ofile, "----------------------------------------------------------\n");

    fprintf(ofile,"Random seed, seed chosen   : %ld\n",llavor);
    fprintf(ofile,"\n");   
    fprintf(ofile,"Temps obertura supermercat : %.1lf\n", OBRIRTIME);
    fprintf(ofile,"Temps tancar supermercat   : %.1lf\n", TANCARTIME);
    fprintf(ofile,"\n");
    fprintf(ofile,"Temps mig entre arribades  : %d\n", ARRIVAL);
    fprintf(ofile,"Temps mig de servei        : %d\n", SERVICE);
    fprintf(ofile,"\n");
    fprintf(ofile,"Nombre total de caixers    : %d\n", ntc);    
    fprintf(ofile,"-----------------------------------------------------------\n");
    fprintf(ofile,"\n");
    fprintf(ofile,"--- Traces de Seguiment del programa (0 = NO, 1 = SI) : %d \n", anyDEBUG);
    fprintf(ofile,"\n");
} // print_configuracio