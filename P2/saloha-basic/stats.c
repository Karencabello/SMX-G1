/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha
 * 
 * Funcions per la captacio d'estadistiques i generacio de resultats
 * 
 * File:   stats.c
 * Author: Dolors Sala
 */
#include "./saloha.h"
#include "stats.h"

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

// Computes the requested percentile of a histogram
long percentile_hist(long *h,long dimh, long percentile){
    long i, sampls;
    double prct = 0.0;
    
    sampls = samples(h,dimh);
    if(sampls > 0){   
        for(i = 0; i < dimh; i++){        
            prct += h[i];         
            if((prct/sampls) >= (percentile/100.0))            
                return(i);    
        }
    }
    else i = NA;
    return(i);
} // percentile_hist

// Computes the maximum of a histogram
long max_hist(long *h,long dimh){
    long i;
    
    for(i = dimh-1; i >= 0; i--){
        if( h[i] != 0) return(i);
    }
    return(NA);
} // max_hist

// Computes de minimum of a histogram
long min_hist(long *h,long dimh){
    long i;
    
    for(i = 0; i < dimh; i++){
        if( h[i] != 0) return(i);
    }
    return(NA);
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
        if(m > 0)
            ERROR((ofile,"%d ERROR mean_hist: m %d > = but samples = %d",slot, m, samples));
    }
    return(m);
} // mean_hist

// Computes de mean of elements of a vector
double mean_vect(long *v,long dimh){
    long i;
    double m = 0.0;
    
    for(i = 0; i < dimh; i++)
        m += v[i];        
    return(m/dimh);
} // mean_vect

// Computes de jitter of a histogram defined as the difference between the min and the max
long jitter_hist(long *v,long dimh){
    long max,min;
    long m = 0.0;
    
    max = max_hist(v,dimh);
    min = min_hist(v,dimh);
    if(max >= 0 && min >= 0) 
        m = max - min;
    else 
        m = NA;
    return(m);
} // jitter_hist

// Computes de stdandard deviation of histogram
double stddev_hist(long *v,long dimh){
    long i;
    double m = mean_hist(v,dimh);
    double s = 0, ss;
    
    ss = samples(v, dimh);
    if(ss >= 0){
        for(i = 0; i < dimh; i++){
            s += v[i] * pow((i - m),2);
        }
        s = sqrt(s / ss);
    }
    else s = NA;
    return(s);
} // stddev_hist

// Prints a vector of longs of length length (in lines) with num_col numbers 
// providing a message to add to personalize printing for different calls 
// of same function
void print_vector(FILE *ofile, long *v, long length, char *msg, int num_col){
    int i;
    long max = length;
    
    max = max_hist(v,length) + 1;
    if(max < length - 1) max += 1; // want to visulize at least one zero.
    fprintf(ofile, "%s", msg);
    for (i = 0; i < max; i++ ){
        if(i % num_col == 0)
            fprintf(ofile, "\n %3d | ", i);
        fprintf(ofile, "%6d ", (long)v[i]);
    }
    fprintf(ofile, "\n\n");
} // print_vector

// Prints a histogram of longs of length length (in lines) with num_col numbers 
// computing the mean of the histogram and adding a message given
// to add to personalize printing for different calls of same function
void print_hist(FILE *ofile, long *v, long length, char *msg, int num_col){
    int i;          
    double CI, mean;
    
    fprintf(ofile, "%s\n", msg);
    mean = mean_hist(v,length);
    fprintf(ofile, " Mean: %.2lf, Samples: %d,", mean, samples(v,length));
    fprintf(ofile, " Min: %d, Max %d\n", min_hist(v,length), max_hist(v,length));
    fprintf(ofile, " Stddev: %lf, Jitter: %d, 95th-percentile: %d\n", 
            stddev_hist(v,length),jitter_hist(v,length),percentile_hist(v,length, 95));
    print_vector(ofile, v, length,"",num_col);
    //fprintf(ofile, "\n");
} // print_hist

// Init of the statistics
void init_stats(){
    int h,i,j;
    sts.gload = (long **) malloc(STSSTATES * sizeof(long*));
    if(sts.gload == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.gload[i]= (long *) calloc(nstns, sizeof(long));
        if(sts.gload[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    }
   
    sts.snt= (long **) malloc(STSSTATES * sizeof(long*));
    if(sts.snt == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
     for(i = 0; i < STSSTATES; i++){
        sts.snt[i]= (long *) calloc(nstns, sizeof(long));
        if(sts.snt[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    }
   
    sts.chhist= (long **) malloc(STSSTATES * sizeof(long*));
    if(sts.chhist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.chhist[i]= (long *) calloc(MAXCOLHIST, sizeof(long));
        if(sts.chhist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    }
    sts.phist= (long **) malloc(STSSTATES * sizeof(long*));
    if(sts.phist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.phist[i]= (long *) calloc(MAXCOLHIST, sizeof(long));
        if(sts.phist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    }

    sts.qhist= (long ***) malloc(STSSTATES * sizeof(long**));
    if(sts.qhist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.qhist[i]= (long **) malloc(nstns * sizeof(long*));
        if(sts.qhist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
        for(j = 0; j < nstns; j++){                
            sts.qhist[i][j]= (long *) calloc(MAXQUHIST, sizeof(long));        
            if(sts.qhist[i][j] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
        }
    }
    sts.dhist= (long ***) malloc(STSSTATES * sizeof(long**));
    if(sts.dhist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.dhist[i]= (long **) malloc(nstns * sizeof(long*));
        if(sts.dhist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
        for(j = 0; j < nstns; j++){
            sts.dhist[i][j]= (long *) calloc(MAXDELHIST, sizeof(long));
            if(sts.dhist[i][j] == NULL )
                ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));    
        }
    }
     
    sts.shist= (long ***) malloc(STSSTATES * sizeof(long**));
    if(sts.shist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.shist[i]= (long **) malloc(nstns * sizeof(long*));
        if(sts.shist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
        for(j = 0; j < nstns; j++){
            sts.shist[i][j]= (long *) calloc(MAXDELHIST, sizeof(long));
            if(sts.shist[i][j] == NULL )
                ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));    
        }
    }
     
    sts.ahist= (long ***) malloc(STSSTATES * sizeof(long**));
    if(sts.ahist == NULL )
        ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
    for(i = 0; i < STSSTATES; i++){
        sts.ahist[i]= (long **) malloc(nstns * sizeof(long*));
        if(sts.ahist[i] == NULL )
            ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));
         for(j = 0; j < nstns; j++){
            sts.ahist[i][j]= (long *) calloc(MAXATMHIST, sizeof(long));
            if(sts.ahist[i][j] == NULL )
                ERROR((ofile,"%d ERROR: allocating memory in init_stats\n",slot));            
         }
    }
    sts.av_delay         = 0.0;
    sts.jitter_delay     = 0.0;
    sts.percentile_delay = 0.0;
    sts.av_qu_len        = 0.0;
    sts.stddev_delay     = 0.0;
    sts.sav_delay        = 0.0;
    sts.sstddev_delay    = 0.0;
   
}// init_stats

// Frees all dynamic memory allocated with init_stats
void free_stats(){
     
    int h,i,j;
    for(i = 0; i < STSSTATES; i++)
        free(sts.gload[i]);
    free(sts.gload);
   
    for(i = 0; i < STSSTATES; i++)
        free(sts.snt[i]);
    free(sts.snt);
   
    for(i = 0; i < STSSTATES; i++)
        free(sts.chhist[i]);
    free(sts.chhist);
   
    for(i = 0; i < STSSTATES; i++)
        free(sts.phist[i]);
    free(sts.phist);
    
    for(i = 0; i < STSSTATES; i++){
        for(j = 0; j < nstns; j++)
            free(sts.qhist[i][j]);
        free(sts.qhist[i]);
    }
    free(sts.qhist);
 
    for(i = 0; i < STSSTATES; i++){
        for(j = 0; j < nstns; j++)
            free(sts.dhist[i][j]);                        
        free(sts.dhist[i]);
    }
    free(sts.dhist);
     
    for(i = 0; i < STSSTATES; i++){
        for(j = 0; j < nstns; j++)
            free(sts.shist[i][j]);
        free(sts.shist[i]);
    }
    free(sts.shist);
        
    for(i = 0; i < STSSTATES; i++){
         for(j = 0; j < nstns; j++)
            free(sts.ahist[i][j]);
        free(sts.ahist[i]);
    }
    free(sts.ahist);

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
        ERROR((ofile,"%d ERROR: allocating memory in sum_stn_hists\n",slot));
    
    for(s = 0; s < numstns; s++)
        for(d = 0; d < dimh; d++)
            sum[d] += h[s][d];
    return(sum);
} // sum_stn_hists

// Collect and print the statistics
void collect_stats(){
    int s;
    long *sum;
           
    sum = NULL;
    
    MESSAGE((ofile, "\nSUMMARY OF SIMULATION MEASURES ----------------\n"));

    long total_pks_gen = 0;
    for(s = 0; s < nstns; s++)
        total_pks_gen += sts.gload[STSSTEADY][s];
    sts.oload       = (double)total_pks_gen/(double)(nslots-start_stats);
    
    sts.utilization = samples(sts.snt[STSSTEADY],nstns)/(double)(nslots-start_stats);
    
    MESSAGE((ofile, "Total offered load                             : %8.6lf\n",
            sts.oload));
    MESSAGE((ofile, "Utilization                                    : %8.6lf\n",
            sts.utilization));
        
    time_t curtime;
    time(&curtime);
    MESSAGE((ofile,"\n**************** END EXECUTION ******************\n "));    
    MESSAGE((ofile, " Time : %s", ctime(&curtime)));
    MESSAGE((ofile,"*************************************************\n "));    
}// collect_stats

