#!/bin/tcsh         
### First line must be the type of shell to run
###############################################################################
# cShell Script for saloha simulator to run multiple executions and 
# generate a summary table with the rellevant statistics
#
# File: runsaloha.sc
# It needs/uses:
#   saloha-full.exe: the executable of the simulator
#   saloha-table.awk: awk script to collect statistics from one run (out file)
#
# Execution in command line: ./runaloha.sc
# It has to be executed in command line. You can install cygwin to do so.
# You have to have installed tcsh, and awk (usually comes with gcc)
#
# Author: Dolors Sala
###############################################################################
# Different details in the statistics table
#  set VERBASIC  = 0  # Just statistics
#  set VERTHEORY = 1  # It includes the theoretical results
#  set VERCI     = 2  # It includes the basic with CI of means
#  set VERBOTH   = 3  # It includes BASIC, VERTHEORY and VERCI

set subdir    = "runs"          # All runs stored in this subdirectory 
set runpath    = "."/${subdir}

mkdir -p ${runpath}             # Create subdir if it doesn't exist
# rm ${runpath}/*                  # Clean subdirectory before starting

# Set standard configuration run
    set stns      = 10
    set slotsize  = 100
    set rate      = 100
    set CRA       = P             # (P)-persistence; (B)EB, (O)ptimal, (D)eterministic
    set p         = 0.015

    set load      = 0.1
    set iarv      = E             # Exponential

    set duration  =  30
    set start     =  10
    set seed      = 4567    # random = 0
    set alpha     = 0.05
    set z         = 1.64
    set r         = 4

## Set name of Statistics Table (created in the same subdir)
   set stats     = ${runpath}/StatsTable

# generate header of Stats summary table
    echo -n "stns CRA p-pbl  load duration start   seed " >! ${stats}
    echo -n "alpha    z    r " >> ${stats}
    echo -n "efLoad util  avgQ avgDly " >> ${stats}
    echo -n "   CI    r EfSamples TgtSamples "  >> ${stats}
    echo -n "stdDly jitDly 95Dly avgSrv little MD1avgD MD1avgQ \n" >> ${stats}
    echo -n "stns   load " >! ${stats}
    echo -n " efLoad    util      avgQ      avgDly " >> ${stats}
    echo -n "    jitter  little   MD1avgD   MD1avgQ " >> ${stats}
    echo -n " avgDCI   ef_r EfSamples TgtSamples\n" >> ${stats}

# Execute multiple runs
foreach seed        (   1234  234  )     # ( 3435 78531 845978)
foreach stns        (  10  30)     # ( 15)
foreach CRA         (   D B P)     # ( B  )
foreach duration    (   30  )   # ( 10 300 )
foreach start       (    10   )   # ( 3 30 )

foreach load (  .1  .2  .3 0.3 0.35 0.4     )   #( .1  .2  .3  .32  .34  .35  )

# Set different filename for each different run
   set infile    = ${runpath}/in
   set outfile   = ${runpath}/out
   set infile  =  ${infile}.seed${seed}.s${stns}.cra${CRA}.dura${duration}.start${start}${load}
   set outfile = ${outfile}.seed${seed}.s${stns}.cra${CRA}.dura${duration}.start${start}${load}

## create the input file for current run
   echo -n "running ${subdir}: seed ${seed} stns ${stns} CRA ${CRA} p ${p} duration ${duration} start ${start} load ${load} ..."

   echo "${stns} ${slotsize} ${rate} ${CRA} ${p} " >! ${infile}
   echo "${load} ${iarv}  " >> ${infile} 
   echo "${duration} ${start} ${seed} " >> ${infile}
   echo "${alpha} ${z} ${r}" >> ${infile}
   
# Execute the simulation: comment if you just want to reprocess the output files without re-executing
   ./saloha-full.exe ${infile} ${outfile}
  
# Collect the summary of statistics
  awk -f saloha-table.awk < ${outfile} >> ${stats} # < ${outfile} > ${stats}

  echo done

end     #load
echo " " >>${stats}
echo " " 
end     #start
end     #duration
end     #CRA
end     #stns
end     #seed

# Clean as needed
# rm ${runpath}/*                  


