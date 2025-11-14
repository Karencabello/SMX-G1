###############################################################################
# Awk script to collect statistics of one output file of the saloha simulator 
# Execution in command line: awk -f saloha-table.awk < out
# It has to be executed in command line. You can install cygwin to do so.
# You have to have installed awk (usually comes with gcc)
#
# Author: Dolors Sala
###############################################################################

# Configuration:
# Set version to 0 to process saloha basic and to 1 for output of saloha full
# Set header to 0 for not printing header and to 1 to print header

## Only executed once at the beginning
BEGIN{
    # Different details in the statistics table
    VERBASIC  = 0  # Just statistics
    VERTHEORY = 1  # It includes the theoretical results
    VERCI     = 2  # It includes the basic with CI of means
    VERBOTH   = 3  # It includes BASIC, VERTHEORY and VERCI

    ON  = 1
    OFF = 0

    version = VERBOTH  # version 0 doesn't print theoretical results or CI results, version 1 does
    header  = OFF  # header OFF does not print the header on the table (it relies on the print at .sc), header ON does
    if(header == 1){
        printf("stns   load ")
        printf(" efLoad    util      avgQ      avgDly ") 
        if(version == VERTHEORY || version == VERBOTH) {
            printf("    jitter  little   MD1avgD   MD1avgQ ") 
        }
        if(version == VERCI || version == VERBOTH) {
            printf("    avgDCI    ef_r   EfSamples   TgtSamples") 
        }
        printf("\n")
    }
}

################# PROCESSING INPUT PARAMETERS #########################
$1 == "Number" && $3 == "stations" { 
    stns = $5 
}

$1 == "Contention" && $2 == "Resolution" {
    CRA = $5
}

$1 == "Probability" && $3 == "p-persistent" {
    p = $6
}

$2 == "offered" && $3 == "load" { 
    load = $5 
}

$1 == "Length" && $3 == "simulation" {
    duration = $7
}

$1 == "Start" && $2 == "statistics" {
    start = $6
}

$1 == "Seed" && $2 == "value" {
    seed = $7
}

$1 == "Significance" && $2 == "level" {
    alpha = $5
}

$1 == "Z" && $2 == "value" {
    z = $7
}

$1 == "Target" && $2 == "CI" {
    r = $9
}

$1 == "End" && $3 == "input" {
  printf("  %2d  %5.3lf ", stns, load);    
}

################# PROCESSING OUTPUT PARAMETERS #########################

$1 == "Total" && $2 == "offered" && $3 == "load" {
    EfLoad = $5
}

$1 == "Utilization" {
    util = $3
}

$1 == "Average" && $2 == "Queue" && $3 == "Length"  {
    avgQ = $7
}
$1 == "Average" && $2 == "Delay" && $3 == "across"  {
    avgD = $6
    avgDCI = $9
    ef_r = $13
    EfSamples = $16
    TgtSamples = $18
}

$1 == "Standard" && $2 == "Deviation" && $4 == "Delay"  {
    stdDly = $8
}

$1 == "Delay" && $2 == "jitter" {
    jitDly = $7
}
$1 == "95th-percentile" && $3 == "Delay"  {
    Dly95 = $7
}
$1 == "Average" && $2 == "Service"  {
    avgSrv = $7
}
$1 == "Little's" && $2 == "law"  {
    little = $6
}
$1 == "M/D/1" && $5 == "delay"   {
    MD1avgD = $7
}
$1 == "M/D/1" && $5 == "queue"   {
    MD1avgQ = $8
}
$1 == "M/G/1" && $5 == "delay"   {
    MG1avgD = $7
}
$1 == "M/G/1" && $5 == "queue"   {
    MG1avgQ = $8
}
# Only at the end
END {
#    printf("%5.3lf %5.3lf %5.2lf %6.2lf ", EfLoad, util, avgQ, avgD)
    printf("%8.6lf %8.6lf %9.6lf %10.6lf ",  EfLoad, util, avgQ, avgD)
    if(version == VERTHEORY || version == VERBOTH) {
        printf(" %8.3lf %9.4lf %8.4lf %8.4lf ",
                jitDly, little, MD1avgD, MD1avgQ) 
    }    
    if(version == VERCI || version == VERBOTH) {
        printf("%7.4lf %6.2lf %9d %9d",  avgDCI, ef_r, EfSamples, TgtSamples)
    }    
    printf("\n")

}
