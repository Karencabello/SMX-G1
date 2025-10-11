/*
 * Program for the verification of the pseudo-random number generators 
 *  
 * Main Program
 * 
 * File:   rng.c
 * Author: Dolors Sala
 * 
 * Program usage: <executable> <num_samples> <num_bins> <seed> 
 * Example:rng 30 10 1234
 * Files: rng.h, rng.c, stochastic.c, verif.c
 */

#include "./rng.h"

FILE *ofile;                      // file to store all program output

//double  random_sample( char fd, double m);
void gen_num(double *v, char fd, char numtype, double m, int num_samples, long seed);

// init all randon number generators to use with the same seed
void init_rngs(long seed){
    //srand48(seed);
    srand(seed);
   //srandom(seed);
} //init_rngs

// It creates a vector of the dimension num_samples
double *create_v(int num_samples){
    double *v = NULL;
    
    v = (double *) calloc(num_samples, sizeof(double));
    if (v == NULL) 
        ERROR((ofile, "Not enough space to create v in gen_num"));

    return(v);
}
// Frees the vector
double *free_v(double *v){
    free(v);
    return(NULL);
}
// It resets a vector to all 0's creating it if it is the first time
double *reset_v(double *v, int num_samples){
    int i;

    if (v == NULL){
        v = (double *)calloc(num_samples, sizeof(double));
    }
    else 
        for(i = 0; i < num_samples; i++)    
            v[i] = 0;
    return(v);
}

int main(int argc, char**argv) {
    int      i;
    int      num_samples;
    int      num_bins;
    double  *v = NULL; // samples of the rng
    long    *e = NULL; // expected histogram values based on reference distribution
    long    *h = NULL; // histogram of the sample vector v
    long     seed;
    double   d; 
    double   size; // size of the bin of a histogram
    //double  *tmp;

    ofile = fopen(FILENAME, "w");
    //ofile = stdout;
    if(ofile == NULL)
        ERROR((ofile, "ERROR opening the file %s\n", FILENAME));
    
    // Prints arguments
    fprintf(ofile,"Program usage: <executable> <num_samples> <num_bins> <seed> \n");
    if(argc != 4) 
        ERROR((ofile, "Not Correct number of parameters passed to the program... see usage..."));
    fprintf(ofile,"Arguments:\n");
    for (i = 0; i < argc; i++) {
        fprintf(ofile,"%i: %s\n", i, argv[i]);
    }
    num_samples = atoi(argv[1]);
    num_bins    = atoi(argv[2])+1; // last bin contains all left over of a distribution
    seed        = atoi(argv[3]);
    
    init_rngs(seed);
    fflush(ofile);

#if (RUNRAND == 1)
    {
    // rand
    // Generation of pseudo-random sequence based on rand
    fprintf(ofile,"\nRAND    RNG ------------------------\n");
    init_rngs(seed);
    v = reset_v(v,num_samples);
    gen_num(v, 'R','L',NA,num_samples, seed);

#if(SOLUTION == 1)
    // Verification of pseudo-random generator (rand)
    double max = MAX_LONG;
    int pos;
    size = max / (num_bins-1);
    d = verify_RNG(v, num_samples, num_bins, size, 'R');
    fprintf(ofile,"Value of D for %d samples and %d bins for RNG RND is %8.2lf\n",
            num_samples, num_bins, d);

    init_rngs(seed);
    v = reset_v(v,num_samples);
    gen_num(v, 'R','L',NA,num_samples, seed);
    // change range to [0..1] instead of 0..LONG_MAX
    for(pos = 0; pos < num_samples; pos++)
        v[pos] = v[pos] / MAX_LONG;
    size = 1.0 / (num_bins-1);
    pos = (int)(num_samples/2.0);
    //d = corr(v, &(v[pos]), pos, num_bins, size);
    d = corr(v, &(v[pos]), pos);
    fprintf(ofile,"Correlation of the observations with %d samples and %d bins is %lf\n",
            num_samples, num_bins, d);
#endif
    }
#endif
    
#if (RUNRAND48 == 1)
    {
    // drand48
    // Generation of pseudo-random sequence based on drand48
    fprintf(ofile,"\ndrand48 RNG ------------------------\n");
    init_rngs(seed);
    v = reset_v(v,num_samples);
    gen_num(v, 'U','D',NA, num_samples, seed);   

#if(SOLUTION == 1)
    // Verification of pseudo-random generator (drand48)
    double max = 1;
    int pos;
    size = max / (num_bins-1);
    d = verify_RNG(v, num_samples, num_bins, size, 'U');
    fprintf(ofile,"Value of D for %d samples and %d bins for ldrand48 is %8.2lf\n\n",
            num_samples, num_bins, d);

    init_rngs(seed);
    v = reset_v(v,num_samples);
    gen_num(v, 'U','D',NA,num_samples, seed);
    pos = (int)(num_samples/2.0);
    //d = corr(v, &(v[pos]), pos, num_bins, size);
    d = corr(v, &(v[pos]), pos);
    fprintf(ofile,"Correlation of the observations with %d samples and %d bins is %lf\n",
            num_samples, num_bins, d);
#endif
    }
#endif

    free(v);
    fclose(ofile);
    return(0);
} // main
