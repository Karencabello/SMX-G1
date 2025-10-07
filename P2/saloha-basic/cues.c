/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 * 
 * LLibreria de funcions de gestio de cues
 * 
 * File:   cues.c
 * Author: Dolors Sala
 */

#include <math.h>
#include "./saloha.h"
#include "./cues.h"

// Prints a circular queue q
void print_queue(squeue q){
    int i, f,p;

    fprintf(ofile,"Queue (H %2d,T %2d, L %2d) (NM,SAV, iST,TX): ",q.head, q.tail, q.lng);

    for (i = 0; i < q.lng; i++){
        p = (q.head + i) % MAXQU;
        fprintf(ofile,"%3d (%3d, %4d, %4d, %1d)",
                p, q.pks[p].num, q.pks[p].sarv_time,
                q.pks[p].iservtime, q.pks[p].txcount);
        if (p == q.tail)
            break;
    }
    fprintf(ofile,"\n");
    fflush(ofile);
}// print_queue

#if 1
// Creates a new queue element with the information provided
equeue create_qu_element(int num, double atime){
    equeue c;
    
    c.arv_time  = atime;
    c.sarv_time = ceil(atime);
    c.iservtime = NA;
    c.num       = num;
    c.txcount   = 0;
    
#if (DEBUGqueuing == 1 || DEBUG == 1)
   TRACE((ofile, "%4d CREATING NEW ELEMENT: num %d atime %8.4lf stime %6d tx-count %d\n",
           slot,c.num, c.arv_time, c.sarv_time,c.txcount ));
#endif
      
   return(c);
   
}//create_qu_element
#endif

// Creates and initializes to empty the station s queue with a maximum capacity 
// equal to max
void create_queue(sstation *s, int max){
    int p;
    
    s->qu.pks = (equeue*) malloc(max*sizeof(equeue));
    if(s->qu.pks == NULL){
        ERROR((ofile,"CREATE QUEUE: Not enough memory to create queue %d\n",s->stnnum));
        exit(-1);
    }
    for(p = 0; p < max; p++){
        s->qu.pks[p].arv_time  = NA;
        s->qu.pks[p].sarv_time = NA;
        s->qu.pks[p].iservtime = NA;
        s->qu.pks[p].num = NA;
        s->qu.pks[p].txcount = 0;
    }
    s->qu.lng = 0;
    s->qu.tail = NA;
    s->qu.head = NA;
    s->qu.max = max;
   
#if (DEBUGqueuing == 1 || DEBUG == 1)
    print_queue(s->qu);
#endif
}//create_queue

//Releases the buffer (space to keep paquets) memory allocated to a queue 
void free_queue(squeue *q){
    free(q->pks);
} // free_queue

//Add an element e in the queue q
void add_qu_element(squeue *q, equeue e){
#if (DEBUGqueuing == 1 || DEBUG == 1) 
    TRACE((ofile,"%4d ADD QUEUE BEFORE: pos %d elem (num %4d, arv %8.4lf sarv %6d txcnt %2d): ",
            slot, q->tail,e.num, e.arv_time,e.sarv_time, e.txcount));
    print_queue(*q);
#endif
    
    if (q->lng == MAXQU) {
        ERROR((ofile,"ERROR QUEUE: Not enough memory in queue, increment MAXQU constant"));
        exit(-1);
    }
    
    q->tail++;
    if(q->tail == MAXQU) q->tail = 0; // circular implementation
    if(q->lng == 0) q->head = q->tail;
    q->lng++;
  
    q->pks[q->tail] = e;

#if (DEBUGqueuing == 1 || DEBUG == 1)
    TRACE((ofile,"%4d ADD QUEUE... pos %d elem (%4d, %8.4lf %6d %2d): ",
            slot, q->tail,q->pks[q->tail].num,q->pks[q->tail].arv_time, 
            q->pks[q->tail].sarv_time,q->pks[q->tail].txcount));
    print_queue(*q);
#endif
        
}// add_qu_element

// Treure el seguent element de la cua circular
int delete_qu_element(squeue *q, equeue *e){
    
#if (DEBUGqueuing == 1 || DEBUG == 1)
    TRACE((ofile,"%4d DELETE QUEUE BEFORE ... pos %d elem (%4d, %8.4lf, %6d) ",
            slot, q->tail,q->pks[q->tail].num,q->pks[q->tail].arv_time, 
            q->pks[q->tail].sarv_time));
   print_queue(*q);
   TRACE((ofile,"%4d DELETE QUEUE AFTER  ... pos %d ",
            slot, q->tail,q->pks[q->tail].sarv_time));
#endif

    if(q->lng == 0) return (0); //no hi ha elements a la cua
    
    *e = q->pks[q->head];
    q->pks[q->head] = create_qu_element(NA,NA); // can be deleted?
    q->head++;
    if (q->head == MAXQU) q->head = 0;
    q->lng--;
    if(q->lng == 0) q->head = q->tail = NA; // empty queue

#if (DEBUGqueuing == 1 || DEBUG == 1)
   TRACE((ofile,"elem (%4d, %8.4lf, %6d) ",
            e->num, e->arv_time, e->sarv_time, e->txcount));
    print_queue(*q);
#endif

    return(1);
}// delete_qu_element


// Sets the time the service starts
void set_start_service_time(equeue *pk, long stime){
    pk->iservtime = stime;
} // set_start_service_time


// Returns how many elements the queue has
int queu_length(squeue s){
    return(s.lng);
} // queu_length


