/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 * 
 * Functions that implement the saloha protocol
 * 
 * File:   protocol.c
 * Author: dolors
 */

#include "./saloha.h"
#include "stats.h"
#include "cra.h"
#include "cues.h"

// The station transmits in the current slot
void transmit_now_stn(sstation *s, equeue pk){
    char stnprevstate = s->state;
    //int dbstn = s->stnnum;
    
    // Transmit
    channel.cslot.SA = s->stnnum;
    channel.cslot.DA = SINK_ADDR;
    channel.cslot.pk = pk;
    channel.cslot.state++; // one more station transmitting in this slot
    
    // Wait for response
    s->state = STNTX;
    s->txtslot = slot;    // transmission time is current slot   
    s->qu.pks[s->qu.head].txcount++; // another attempt to transmit this paquet
       
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGTRAF == 1 || DEBUGchannel == 1 || DEBUGCRA == 1 )
    TRACE((ofile,"%4d STN %2d TRANSMIT : SA %2d DA %2d pk %3d channel state %2d stn state (prev %c next %c) attempts %d\n",
            slot, s->stnnum, channel.cslot.SA, channel.cslot.DA, channel.cslot.pk.num,
            channel.cslot.state, stnprevstate, s->state, s->qu.pks[s->qu.head].txcount));
#endif
}// transmit_now_stn

// Receive ack in a station 
void receive_ack(sstation *s){
    equeue pk;
    int stsstate;
    int n;
    
    int stn = s->stnnum;
    //int allstns = nstns;
    int stnprevstate = s->state;
    
    if(s->qu.lng == 0) 
        ERROR((ofile,"Receiving and Ack and there is no paquets in the queue"));
#if 1    
    if (channel.cslot.pk.num != s->qu.pks[s->qu.head].num) 
        ERROR((ofile,"%d ERROR STNTX: an ACK received at stn %d with wrong pk num (waiting %d arrv %d)\n",
                slot, s->stnnum,s->qu.pks[s->qu.head].num,channel.cslot.pk.num));
#endif
  
    n = delete_qu_element(&s->qu, &pk);
    if (n != 1)
        ERROR((ofile," Receiving an ack and no packets to delete from the queue of pks"));
    s->state = STNIDLE;
    
    // check stats
    if(pk.txcount >= MAXATMHIST)
        ERROR((ofile,"%d Increase size of attempts histogram MAXATMHIST (%d)",slot, MAXATMHIST));
    if(slot-pk.sarv_time+1 >= MAXDELHIST)
        ERROR((ofile,"%d Increase size of delay histogram MAXDELHIST (%d) to greater to %d",slot, MAXDELHIST, slot-pk.sarv_time));
     if(slot-pk.iservtime+1 >= MAXDELHIST)
        ERROR((ofile,"%d Service time: Increase size of delay histogram MAXDELHIST (%d) to greater to %d",slot, MAXDELHIST, slot-pk.sarv_time));
    if(pk.sarv_time < 0 || pk.iservtime < 0)
        ERROR((ofile,"% d ERROR: arrival time %d initial service time %d are negative"));
    
    // update stats
    if(slot < start_stats)
        stsstate = STSWARMUP;
    else 
        stsstate = STSSTEADY;

    sts.snt[stsstate][s->stnnum]++;
    
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1)
    TRACE((ofile,"%4d STN %2d RV ACK   : SA %2d DA %2d channel state %2d stn state (prev %c next %c) tx-count %1d ",
            slot, s->stnnum, channel.cslot.SA, channel.cslot.DA, 
            channel.cslot.state, stnprevstate, s->state,pk.txcount)); 
    print_queue(s->qu);
#endif
}// receive_ack

// Executes all functionality implemented by the station, currently:
// the transmission of a paquet if there is somethign in the queue
// waiting for ack of destination to know correct transmission
// collision resolution algorithm 
void station(sstation *s){
    //equeue pk;
    int stnprevstate = s->state;
    // solo para los fprintf(ofile,s del debug que el preprocesador no accepta campos ni ptrs
    int sdbaux = s->stnnum; 

    switch(s->state){
        case STNIDLE:                 
            if(s->qu.lng > 0 ) {                                        
                if(channel.cralg == 'P' || channel.cralg == 'O')                
                    s->wait = backoff(s->stnnum,channel.cralg);          
               
                if(s->wait == 0){                
                    transmit_now_stn(s,s->qu.pks[s->qu.head]);                                                                           
                    set_start_service_time(&(s->qu.pks[s->qu.head]),slot);                             
                }
            }                    
            break;
        case STNCRA:
            if(channel.cralg == 'P' || channel.cralg == 'O'){
                s->wait = backoff(s->stnnum,channel.cralg);  
            }
           
            if(s->wait == 0) {
                transmit_now_stn(s,s->qu.pks[s->qu.head]);
            }
            else 
                if(s->wait < 0) 
                   ERROR((ofile,"%d ERROR STNCRA: negative waiting time (%d) at stn %d",
                    slot, s->wait,s->stnnum)); 
                else {
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1)
                    TRACE((ofile,"%4d STN %2d CRA: wait %2d \n", slot, s->stnnum, s->wait));
#endif          
                        s->wait--;
                }
    break;
        case STNTX:
            if(channel.cslot.DA == s->stnnum){ // Transmission successful
                // not implemented instead the sink runs the received_ack
                //received_ack(s);
            }
            else{ // assume collision: transmission time = 1 slot
                s->wait = backoff(s->stnnum,channel.cralg);
                s->state = STNCRA;
#if (DEBUG == 1 || DEBUGSTN == 1 || DEBUGCRA == 1 )
    TRACE((ofile,"%4d STN %2d COLLISION: SA %2d DA %2d pk %3d channel state %2d stn state (prev %c next %c) attempts %d wait %2d\n",
            slot, s->stnnum, channel.cslot.SA, channel.cslot.DA, s->qu.pks[s->qu.head].num,
            channel.cslot.state, stnprevstate, s->state,s->qu.pks[s->qu.head].txcount, s->wait));
#endif
            }
            break;
        default: ERROR((ofile,"Station (%d) state (%c) not known", s->stnnum, s->state));
    }//switch
    
} // station

// Runs the sink that is the station destination of all paquets:
// sends acks to origin so origin knows it was a succesfull transmission 
// (it does it violating the real system but good enough for the model)
// updates overall statistics at the end of the slot time
// The sink must be the last station to check in a slot so the state of the 
// cslot is final
void run_sink(){
    int s,i;
#if (DEBUG == 1 || DEBUGchannel == 1 )   
    //if(slot >= 188 && slot < 191)
    TRACE((ofile, "%4d END SLOT TIME:........... channel SA %2d DA %2d S %2d pk (%3d, %4d, %4d, %2d)\n", 
            slot, channel.cslot.SA, channel.cslot.DA, channel.cslot.state, 
            channel.cslot.pk.num, channel.cslot.pk.sarv_time, 
            channel.cslot.pk.iservtime, channel.cslot.pk.txcount));
#endif
    
    // update sts        
        
    // Send ack if successful transmission
    if(channel.cslot.state == 1){ 
        // to simplify simulation change station in here although this violates reality
        receive_ack(&stns[channel.cslot.SA]);
    }
    else if(channel.cslot.state > 1){
#if (DEBUG == 1 || DEBUGchannel == 1 )
    TRACE((ofile,"%4d CHANNEL COLLISION: SA %2d DA %2d pk %3d channel state %2d \n",
            slot, channel.cslot.SA, channel.cslot.DA, channel.cslot.pk.num,
            channel.cslot.state));
#endif
    }
       
    // check sts
    long d;
    if(channel.cslot.state >= MAXCOLHIST)
    //if(channel.cslot.state >= MAXCOLHIST)
        ERROR((ofile,"%d Number of collisions is larger than the histogram size. Increase MAXCOLHIST", slot));
    if(channel.cslot.state > nstns)
        ERROR((ofile,"%d ERROR Multiplicity of collision (%d) larger than total number of stations %d \n", slot, channel.cslot.state, nstns));
    // pk generated = pk sent + pk queue
    for (s = 0; s < nstns; s++){
        if( sts.gload[STSWARMUP][s] + sts.gload[STSSTEADY][s]- 
                (sts.snt[STSWARMUP][s] + sts.snt[STSSTEADY][s]) 
                != stns[s].qu.lng)                     
            ERROR((ofile,"%d ERROR CHECK STN %d: pk generated (%d + %d) - pk sent (%d + %d) != pk in queu %d",
                    slot, s,sts.gload[STSWARMUP][s], sts.gload[STSSTEADY][s],                
                    sts.snt[STSWARMUP][s], sts.snt[STSSTEADY][s], stns[s].qu.lng));

        if(sts.gload[STSWARMUP][s] + sts.gload[STSSTEADY][s]!= stns[s].tpk)     
            ERROR((ofile,"%d ERROR CHECK STN %d: gload (%d + %d) != tpk %d ",                                           
                slot, s,sts.gload[STSWARMUP][s], sts.gload[STSSTEADY][s],
                stns[s].tpk));           
    }   
}//run_sink

