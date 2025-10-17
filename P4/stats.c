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

long     start_stats;   
sstats   sts;           

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
    long i, s = samples(h,dimh);
    double m = 0.0;
    
    for(i = 0; i < dimh; i++){
        m += (h[i] * i);
    }
    if(s > 0){
        m = m / s;
    }
    else{
        if(m > 0)
            ERROR((ofile,"%d ERROR mean_hist: m %d > = but samples = %d",slot, m, s));
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

// Computes the confidence interval of the average delay/ response time
double compute_confidence_interval(double stddev, long n, double z){
        
    double x = z * stddev/sqrt(n);
    return(x);    
} // computer_confidence_interval

// Computes de resolution r as a percentage
double compute_r(double CI, double mean){
    return(100*CI/mean);   
} // compute_r

// Computes the target number of observations required to obtein a resolution r
// in a given significance level alpha/z
long compute_target_n(double z, double r, double mean, double stddev){
    
    double n;
               
    if(r == 0)    
        ERRORF((ofile,"ERROR compute target n: r is zero and cannot be"),NULL);
    
    n = pow((100 * z * stddev)/(r*mean),2);
    
#if (DEBUG == 1 || DEBUGSTS == 1)
    TRACE((ofile, "\ncompute target n: z %lf r %lf mean %lf stddev %lf n %lf return %d \n", z,r,mean,stddev, n, (long)ceil(n)));
#endif
    return((long)ceil(n));
    
} // compute_target_n

// Prints the confidence interval of a mean
void print_CI(double CI, double mean){    

    fprintf(ofile, " CI: %lf (%.2lf, %.2lf) r %.2lf %%", CI, mean - CI, mean + CI, compute_r(CI, mean));
} // print_CI

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
    CI = compute_confidence_interval(stddev_hist(v,length), samples(v,length), sts.z);
    //fprintf(ofile, " CI: %lf (%.2lf, %.2lf) r %.2lf %%", CI, mean - CI, mean + CI, 100*CI/mean);
    print_CI(CI, mean);
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

// Function to compute and print the theoretical results to verify & 
// validate simulation
void compute_theoretical_results(){
    double d = 0.0;
    double CoV = sts.sstddev_delay/sts.sav_delay;
    double rho = (sts.oload / nstns); // load per station
   
    MESSAGE((ofile, "\nTHEORETICAL FORMULAS ----------------\n"));
    
    // Little's law: response time = mean in system / arrival rate      
    MESSAGE((ofile, "Little's law average delay              : %8.4lf slots\n",
            sts.av_qu_len/rho));
    
    rho = (sts.oload ); // total load
   
    // M/D/1 formulas applicable at very small loads
    // avg_delay = service_time *(1+rho/(2(1-rho)))
    // service time = 1, constant one slot (without colisions)
    MESSAGE((ofile, "M/D/1 (small load) average delay        : %8.4lf slots\n",
             1*(1+(rho/(2*(1-rho))))));
    // qu_length = rho+rho^2/((2*(1-rho))
    MESSAGE((ofile, "M/D/1 (small load) average queue length : %8.4lf pks\n",
             rho + (pow(rho,2)/(2*(1-rho)))));

} // compute_theoretical_results

// Collect and print the statistics
void collect_stats(){
    int s;
    long *sum;
           
    sum = NULL;
    
    MESSAGE((ofile, "PRINTING STATISTICS -----------------------------\n\n"));

    // Queue histogram statistics
    sts.av_qu_len = 0.0;
    for(s = 0; s < nstns; s++){
        MESSAGE((ofile, "Queue Histogram of station %d",s));
        print_hist(ofile, sts.qhist[STSSTEADY][s],MAXQUHIST,"",10);
        sts.av_qu_len += mean_hist(sts.qhist[STSSTEADY][s],MAXQUHIST);
    }
    sts.av_qu_len = sts.av_qu_len / nstns;
    sum = sum_stn_hists(sum, sts.qhist[STSSTEADY],MAXQUHIST,nstns);   
    MESSAGE((ofile, "Queue Histogram over all stns"));
    print_hist(ofile, sum,MAXQUHIST,"",10);
    
    // Delay histogram statistics
    sts.av_delay         = 0.0;
    sts.stddev_delay     = 0.0;
    sts.jitter_delay     = 0.0;
    sts.percentile_delay = 0.0;
    for(s = 0; s < nstns; s++){          
        MESSAGE((ofile, "Delay Histogram of station %d",s));        
        print_hist(ofile, sts.dhist[STSSTEADY][s],MAXDELHIST,"",10);    
        if(samples(sts.dhist[STSSTEADY][s],MAXDELHIST) > 0){ 
            sts.av_delay += mean_hist(sts.dhist[STSSTEADY][s],MAXDELHIST);            
            sts.stddev_delay += stddev_hist(sts.dhist[STSSTEADY][s],MAXDELHIST);            
            sts.jitter_delay += (max_hist(sts.dhist[STSSTEADY][s],MAXDELHIST) -             
                    min_hist(sts.dhist[STSSTEADY][s],MAXDELHIST));        
            sts.percentile_delay += percentile_hist(sts.dhist[STSSTEADY][s],MAXDELHIST,95);
        }
    }
    // Each metric is the average over all stns of the metric for each individual stn
    sts.av_delay         = sts.av_delay / nstns;
    sts.stddev_delay     = sts.stddev_delay / nstns;
    sts.jitter_delay     = sts.jitter_delay / nstns;
    sts.percentile_delay = sts.percentile_delay / nstns;
    sum = sum_stn_hists(sum, sts.dhist[STSSTEADY],MAXDELHIST,nstns);   
    MESSAGE((ofile, "Delay Histogram over all stns"));
    print_hist(ofile, sum,MAXDELHIST,"",10);

    sts.adelCI = compute_confidence_interval(stddev_hist(sum,MAXDELHIST), 
                                            samples(sum,MAXDELHIST), sts.z);
    
    //compute_confidence_interval(sum, MAXDELHIST, s.significance);
    
     // Service histogram statistics
    sts.sav_delay        = 0.0;
    sts.sstddev_delay    = 0.0;
    for(s = 0; s < nstns; s++){                   
        MESSAGE((ofile, "Service Time Histogram of station %d",s));        
        print_hist(ofile, sts.shist[STSSTEADY][s],MAXDELHIST,"",10);                    
        if(samples(sts.shist[STSSTEADY][s],MAXDELHIST) > 0){                              
            sts.sav_delay += mean_hist(sts.shist[STSSTEADY][s],MAXDELHIST);                 
            sts.sstddev_delay += stddev_hist(sts.shist[STSSTEADY][s],MAXDELHIST);                
        }    
    }
    sts.sav_delay        = sts.sav_delay / nstns;
    sts.sstddev_delay    = sts.sstddev_delay / nstns;
    sum = sum_stn_hists(sum, sts.shist[STSSTEADY],MAXDELHIST,nstns);   
    MESSAGE((ofile, "Service Time Histogram over all stns"));
    print_hist(ofile, sum,MAXDELHIST,"",10);

    sts.sdelCI = compute_confidence_interval(stddev_hist(sum,MAXDELHIST), 
                                            samples(sum,MAXDELHIST), sts.z);
    
    //MESSAGE((ofile, "Maximum delay %8d Minimum delay %8d Jitter Delay %8.2lf\n",
    //         max_hist(sum,MAXDELHIST), min_hist(sum,MAXDELHIST), sts.jitter_delay));
   
    // Transmission Attempt histogram statistics
    for(s = 0; s < nstns; s++){
        MESSAGE((ofile, "Transmission Attempt Histogram of station %d",s));
        print_hist(ofile, sts.ahist[STSSTEADY][s],MAXATMHIST,"",10);
    }
    sum = sum_stn_hists(sum, sts.ahist[STSSTEADY],MAXATMHIST,nstns);   
    MESSAGE((ofile, "Transmission Attempt Histogram over all stns"));
    print_hist(ofile, sum,MAXATMHIST,"",10);

    // Collisions histogram statistics
    MESSAGE((ofile, "Collision Histogram over all stns"));
    print_hist(ofile, sts.chhist[STSSTEADY],MAXCOLHIST,"",10);

    // Optimal N histogram statistics for optimal p-persistence
    if(channel.cralg == 'O'){    
        MESSAGE((ofile, "Optimal N Histogram for optimal p-persistence"));    
        print_hist(ofile, sts.phist[STSSTEADY],MAXCOLHIST,"",10);
    }
    
    // Paquets sent by each station STSWARMUP
    MESSAGE((ofile, "Paquets sent by each station STSWARMUP (mean %8.2lf)",mean_vect(sts.snt[STSWARMUP],nstns)));
    print_vector(ofile, sts.snt[STSWARMUP],nstns,"",10);    
    // Paquets sent by each station STSSTEADY
    MESSAGE((ofile, "Paquets sent by each station STSSTEADY (mean %8.2lf)",mean_vect(sts.snt[STSSTEADY],nstns)));
    print_vector(ofile, sts.snt[STSSTEADY],nstns,"",10);
  
    // Paquets generated by each station STSWARMUP
    MESSAGE((ofile, "Paquets generated by each station STSWARMUP (mean %8.2lf)",mean_vect(sts.gload[STSWARMUP],nstns)));
    print_vector(ofile, sts.gload[STSWARMUP],nstns,"",10);
    // Paquets generated by each station STSSTEADY
    MESSAGE((ofile, "Paquets generated by each station STSSTEADY (mean %8.2lf)",mean_vect(sts.gload[STSSTEADY],nstns)));
    print_vector(ofile, sts.gload[STSSTEADY],nstns,"",10);
   
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
    
    MESSAGE((ofile, "Average Queue Length across stns               : %8.6lf pks\n",
            sts.av_qu_len));
    //MESSAGE((ofile, "Average Delay across stns                      : %8.4lf slots CI %.4lf (%.4lf %.4lf) %.4lf %%\n",
    //        sts.av_delay, sts.adelCI, sts.av_delay - sts.adelCI, sts.av_delay + sts.adelCI, 100*sts.adelCI/sts.av_delay));
    MESSAGE((ofile, "Average Delay across stns                      : %8.4lf slots ", sts.av_delay);
    print_CI(sts.adelCI,sts.av_delay));
       
    sum = sum_stn_hists(sum, sts.dhist[STSSTEADY],MAXDELHIST,nstns);                   
    MESSAGE((ofile, " samples %d target %d",samples(sum, MAXDELHIST),
            compute_target_n(sts.z,sts.r,sts.av_delay, sts.stddev_delay)));
    MESSAGE((ofile,"\n"));
    
    MESSAGE((ofile, "Standard Deviation of Delay across stns        : %8.4lf slots\n",
            sts.stddev_delay));
    MESSAGE((ofile, "Delay jitter (max-min) across stns             : %8.4lf slots\n",
            sts.jitter_delay));
    MESSAGE((ofile, "95th-percentile of Delay across stns           : %8.4lf slots \n", 
            sts.percentile_delay));
    //MESSAGE((ofile, "Average Service Time across stns               : %8.4lf slots CI %.4lf (%.4lf %.4lf) %.4lf %%\n",
    //        sts.sav_delay, sts.sdelCI, sts.sav_delay - sts.sdelCI, sts.sav_delay + sts.sdelCI, 100*sts.sdelCI/sts.sav_delay));
    MESSAGE((ofile, "Average Service Time across stns               : %8.4lf slots ", sts.sav_delay));
    print_CI(sts.sdelCI,sts.sav_delay);

    sum = sum_stn_hists(sum, sts.shist[STSSTEADY],MAXDELHIST,nstns);                   
    MESSAGE((ofile, " samples %d target %d", samples(sum, MAXDELHIST),
            compute_target_n(sts.z,sts.r,sts.sav_delay, sts.sstddev_delay)));
    MESSAGE((ofile,"\n"));
    
    MESSAGE((ofile, "Standard Deviation of Service Time across stns : %8.2lf slots\n",
            sts.sstddev_delay));

    compute_theoretical_results();
        
    time_t curtime;
    time(&curtime);
    MESSAGE((ofile,"\n**************** END EXECUTION ******************\n "));    
    MESSAGE((ofile, " Time : %s", ctime(&curtime)));
    MESSAGE((ofile,"*************************************************\n "));    
}// collect_stats

