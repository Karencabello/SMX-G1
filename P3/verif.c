/*
 * Program for the verification of the pseudo-random number generators 
 *  
 * Statistical functions to compute D value for the verification of RNGs
 * 
 * File:   verif.c
 * Author: Dolors Sala
 */

#include "./rng.h"

// extern FILE *ofile;

// Print a histogram
void print_hist(long *h, int num_bins, char *msg){
    int i;
    
    fprintf(ofile,"%s",msg);
    for (i = 0; i < num_bins; i++ )
        fprintf(ofile,"(%2d, %4ld)", i,h[i]);
    fprintf(ofile,"\n");
}// print_hist

// Creates the histograms of the vector of samples with the number of bins 
// and size of the bin indicated. The last bin count all samples larger than the
// last bin value
long *create_hists(double *v, int num_samples, int num_bins, double size){
        
    long *h;
    long i,j,b;

    h = (long *) malloc(num_bins * sizeof(long));
    if (h == NULL) 
        ERROR((ofile, "Not enough space to create v in create_hists"));
        
    for(j = 0; j < num_bins; j++)
        h[j]=0;      

    for (i = 0; i < num_samples; i++ ){
        b = (int)floor(v[i] / size);  
        //b = v[i] / size;  
#if 0
        b = (int)floor(v[i] * num_bins);
        if (b >= num_bins) 
            b = num_bins - 1;
        h[b]++;
        
        b = (int)floor((v[RNG_RND][i]/RAND_MAX) * num_bins);   
        if (b >= num_bins) ERROR((ofile, "Histogram value too large bin %d",b));
        h[RNG_RND][b]++;
#endif
        if (b >= num_bins) 
            b = num_bins - 1;
        h[b]++;
        
    }  
    
    // checks
    int n = 0;
    for (i = 0; i < num_bins; i++ ) {
        n += h[i];        
    }
    if(n != num_samples ) 
        ERROR((ofile, "Total number of samples %d  != sum of hist values n %d ",num_samples,n));

#if (DEBUG == 1 || DEBUGhist == 1)
    print_hist(h, num_bins,"Histogram      generated : ");
#endif
    return(h);
} // create_hists

// Creates the expected histogram of an reference/ideal uniform distribution
long *create_ref_Uniform(int num_samples, int num_bins, double size){
    long *e;
    long i,j,b;
    double tp = 0;
    
    //num_bins--;
    e = (long *) malloc(num_bins * sizeof(long));
    if (e == NULL) ERROR((ofile, "Not enough space to create v in create_ref_Uniform"));    
    for(j = 0; j < num_bins; j++){
        if (tp < num_samples) 
            e[j]= (int)floor(num_samples / (num_bins-1));      
        else
            e[j] = 0;
        tp += e[j];
    }
#if (DEBUG == 1|| DEBUGhist == 1)
    fprintf(ofile, "Size %6.3f \n",size);
    print_hist(e, num_bins,"Histogram Expected Uniform: ");
#endif
    return(e);
} //create_ref_Uniform

// Creates the expected histogram of an reference/ideal Exponential distribution
long *create_ref_Exponential(double m, int num_samples, 
        int num_bins, double size){
    long *e;
    long i,j,b;
    double s, p = 0.0, prev = 0.0;
    
    e = (long *) malloc(num_bins * sizeof(long));
    if (e == NULL) ERROR((ofile, "Not enough space to create v in create_ref_Exponential"));    
    for(j = 0; j < num_bins; j++){                
        prev = p;
        p = CdfExponential(m,(j+1)*size);
        s = (p-prev) * num_samples;
        if(s < 0) 
            ERROR((ofile, "Number of samples negative %lf",s));
        e[j]= (int) floor(s);      
    }
#if (DEBUG == 1 || DEBUGhist == 1)
    printf("Size %6.3lf m %lf \n",size,m);
    print_hist(e, num_bins,"Histogram Expected Expone: ");
#endif
    return(e);
} //create_ref_Exponential

// Creates the expected histogram of an reference/ideal pareto distribution
long *create_ref_Pareto(double m, int num_samples, 
        int num_bins, double size){
    long *e;
    long i,j,b;
    double s, p = 0.0, prev = 0.0;
    
    e = (long *) malloc(num_bins * sizeof(long));
    if (e == NULL) ERROR((ofile, "Not enough space to create v in create_ref_Pareto"));    
    for(j = 0; j < num_bins; j++){                
        prev = p;
        p = CdfPareto(m,(j+1)*size);
        s = (p-prev) * num_samples;
        if(s < 0) 
            ERROR((ofile, "Number of samples negative %lf",s));
        e[j]= (int) floor(s);      
    }
#if (DEBUG == 1 || DEBUGhist == 1)
    printf("Size %6.3lf m %lf \n",size,m);
    print_hist(e, num_bins,"Histogram Expected Pareto: ");
#endif
    return(e);
} //create_ref_Pareto

// Creates the expected histogram of the corresponding reference/ideal distribution
long *create_expected(char fd, double m, int num_samples, int num_bins, double size){
    long *e;
    
    switch(fd){
        case 'U':
            e = create_ref_Uniform(num_samples,num_bins,size);
            break;
        case 'R':
            e = create_ref_Uniform(num_samples,num_bins,size);
            break;
        case 'E':
            e = create_ref_Exponential(m,num_samples,num_bins,size);
            break;
        case 'P':
            e = create_ref_Pareto(m,num_samples,num_bins,size);
            break;
        default:
            ERROR((ofile, "Distribution function not known (%c) in create_expected",fd));
    }
#if 0 // (DEBUG == 1)
    print_hist(e, num_bins,"Histogram Expected: ");
#endif
    return(e);
} // create_expected

// Computes the D value to apply the chi-square test
double err_observations(long *h, long *e, int num_bins){
    
    int i;
    double tmp = 0.0;
    double d = 0.0;

#if (DEBUG == 1 || DEBUGsamples == 1)
        fprintf(ofile,"\n\n");
        print_hist(h, num_bins,"Sample Histogram   : ");
#endif 
#if (DEBUG == 1)
       print_hist(e, num_bins,"Expected Histogram : ");
#endif
       fprintf(ofile,"         Computing errors: ");    
       for (i = 0; i < (num_bins - 1); i++){      
           tmp = pow(h[i]-e[i],2.0)/e[i];        
           d += tmp;
#if (DEBUG == 1 || DEBUGhist == 1)
        fprintf(ofile,"(%2d, %4.2lf)", i,tmp);
#endif
    } 
#if (DEBUG == 1 || DEBUGhist == 1)
        fprintf(ofile,"\n");
#endif
   return(d);
}
// It swaps two positions of a vector to order the vector of doubles
void intercanviar_d(double *n, double *m){
    double temp = *n;
    *n = *m;
    *m = temp;
}
// It swaps two positions of a vector to order the vector of longs
void intercanviar_l(long *n, long *m){
    double temp = *n;
    *n = *m;
    *m = temp;
}

// It orders a vector of doubles
void order_d(double *v, int length){
    int j,k;
 
    for(j = 0; j < length; j++)        
        for(k = 0; k < (length-j); k++)            
            if(v[k] < v[k-1])
                intercanviar_d(&v[k],&v[(k-1)]);
#if DEBUG == 1
        fprintf(ofile, "Ordered samples  : ");                
        print_vector_d(v, length,"", NUM_COLUMNS);        
#endif
}
// It orders a vector of longs
void order_l(long *v, int length){
    int j,k;
 
    for(j = 0; j < length; j++)        
        for(k = 0; k < (length-j); k++)            
            if(v[k] < v[k-1])
                intercanviar_l(&v[k],&v[(k-1)]);
#if DEBUG == 1
        fprintf(ofile, "Ordered samples  : ");                
        print_hist(v, length,"");        
#endif
}

// Verifies the RNG and returns de D value
double verify_RNG(double *v, int num_samples, int num_bins, double size, char dist){ 
    long    *e; // expected histogram values based on reference distribution
    long    *h; // histogram of the sample vector v
    double   d; 
    
    order_d(v, num_samples);
    h = create_hists(v, num_samples, num_bins, size);
    e = create_expected(dist, NA, num_samples, num_bins, size);
    d = err_observations(h, e, num_bins);
    fprintf(ofile, "Value of D for %d samples and %d bins for %c is %8.2lf\n\n",
            num_samples, num_bins, dist, d);

    return(d);
    
} // verify_RNG

// Returns the number of samples in the histogram
long samples_hist(long *h,long dimh){
    long i;
    long s = 0;
    
    for(i = 0; i < dimh; i++)
        s += h[i];        
    return(s);
} // samples

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
            ERROR((ofile, "ERROR mean_hist: m %d > = but samples = %d", m, samples));
    }
    return(m);
} // mean_hist


// Computes de mean of elements of a vector
double mean_vect(double *v,long dimh){
    long i;
    double m = 0.0;
    
    for(i = 0; i < dimh; i++)
        m += v[i];        
    return(m/dimh);
} // mean_vect

// Computes de stdandard deviation of histogram
double stddev_hist(long *v,long dimh){
    long i;
    double m = mean_hist(v, dimh);
    double s = 0, ss;
    
    ss = samples_hist(v, dimh);
    if(ss >= 0){
        for(i = 0; i < dimh; i++){
            s += v[i] * pow((i - m),2);
        }
        s = sqrt(s / ss);
    }
    else s = NA;
    return(s);
} // stddev_hist


// Computes de correlation of two vectors of same dimension
//double corr(double *v, double *w, long dimh, int num_bins, double size){
double corr(double *v, double *w, long dimh){
    long i;
    double mv = mean_vect(v,dimh);
    double mw = mean_vect(w,dimh);
    double s = 0, sv = 0, sw = 0;
    long *h = NULL;
    
    for(i = 0; i < dimh; i++){
        s  += (v[i] - mv) * (w[i] - mw);
        sv += (v[i] - mv) * (v[i] - mv);
        sw += (w[i] - mw) * (w[i] - mw);
    }
    s = s / sqrt(sv * sw);
    return(s);
} // stddev_hist

