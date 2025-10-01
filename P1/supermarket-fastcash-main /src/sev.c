
/*
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * Programa principal
 * Modificacions segons llibre:
 *  1) Organitza codi en funcions
 *  2) Separa el codi en fitxers
 *  3) Incorpora traces per veure què fa el programa i permet activar-ho o no* 
 * 
 * File:   sev.c
 * Author: Dolors Sala
 */

#include <time.h>
#include "./sev.h"
#include "./cua.h"
#include "./agenda.h"
#include "./stats.h"

int main(void) {
    esdev e;
    scua *cues = NULL; // vector dinamic de dimensio ntc
    el_cua c;
    float t;
    float ta = 0; // Temps actual que avança la simulació
    float tant = 0; // Temps de l'event anterior per saber quan ha passat entre els dos events per stats
    int bn;     // bandera que indica si caixa oberta 1 o tancada 0  
    int *caixa;  // indica si el caixer [i] esta ocupat 1 o no 0
    float tmax; // temps maxim en el sistema
    int nca;    // nombre de clients atesos
    int ntc;    // nombre total de caixers
    int j, q;
    char *filename = OUTFILENAME;
    long int llavor = RANSEED;      //Inicialitzar generador numeros aleatoris
 
    system("mkdir -p log");
    ofile = fopen(filename, "w");
    if(ofile == NULL){
        ERROR((ofile, "Not possible to open file %s \n", filename));
        exit(1);
    }
    
    fflush(ofile);      
    //ofile = stdout;
    time_header("BEGIN");
    
    if(llavor == 0){  // Zero for random start 
        llavor = time(0); 
    }
    srand(llavor);    
    
    puts("Nombre total de caixers?");
    scanf("%d", &ntc);
    printf("See results of execution in file: %s\n", OUTFILENAME);     
    
    print_configuracio(llavor, ntc);
    
    ini_agenda(N);
    crea_cues(&cues, CUA_MAX, ntc);
    init_stats(&sts, ntc);

    //for(q = 0; q < ntc; q++){
        e = crea_esdev(OBRIR, OBRIRTIME, NA);
        posa_agenda(ta, e);
    //}
    // Tancar fa referència a tancar supermercat i no una caixa en concret
    e = crea_esdev(TANCAR, TANCARTIME, NA);
    posa_agenda(ta, e);

    bn = 0;
    // caixa = 0; es fa dins de crea_cues
    //nca = 0;
    tmax = 0.0;
    
    while (treu_agenda(ta, &e) != 0){
        
        switch (e.que){
            case OBRIR:
                bn = 1;
                //caixa = 0;
                e.on = NA;
                t = expo(ARRIVAL); // ARRIVAL/ntc
#if (DEBUGalea == 1)      
                fprintf(ofile, " utilitzat per la seguent arribada a %lf cua %d\n",
                        t, e.on);
#endif
                e = crea_esdev(ARRIBADA, t, e.on);
                posa_agenda(ta, e);
                break;
            case ARRIBADA:
                if(bn == 1){
                    tant = ta;
                    ta = e.quan;
                    actualitzar_stats_cua(cues, ntc, tant, ta, &sts, MAXQUHIST);
                    // si la caixa buida passa directament a ser servit
                    e.on = primer_caixer_buit(cues, ntc);
                    if (e.on != NA){
                        cues[e.on].caixa = 1;
                        t = 1 + expo(SERVICE);                             
#if (DEBUGalea == 1)
                        fprintf(ofile, " utilitzat pel servei a %lf cua %d\n", 
                                t, e.on);
#endif
                        inc_stats(sts.dshist, e.on, (int)round(t), ntc, MAXDELHIST);
#if DEBUGserv == 1    
                        fprintf(ofile,"%.4lf TEMPS servei %.4lf: ARRIBADA %.4lf SORTIDA %.4lf\n", ta, t, e.quan,ta+t);
                        fflush(ofile);
#endif
                        e = crea_esdev(SORTIDA, ta+t, e.on);
                        posa_agenda(ta, e);
                    }else{ // posar element a la cua d'espera
                        c.on = cua_mes_curta(cues, ntc);                         
                        c.tar = ta;
                        c.tse = 0; //??
                        //c.on = cua_mes_curta(cues, ntc); 
                        j = posa_cua(&cues[c.on], ta, c);
                        if(j == 0){
                            puts("ERROR: cua massa petita");
                            exit(0);
                        }
                    }//else
                    // Decidir la seguent arribada
                    t = ta + expo(ARRIVAL);// ARRIVAL/ntc
                    e.on = NA;
#if (DEBUGalea == 1)
                    fprintf(ofile, " utilitzat per la seguent arribada a %lf cua %d \n", 
                            t, e.on);
#endif
                    e = crea_esdev(ARRIBADA, t, e.on);
                    posa_agenda(ta, e);
                }//bn==1
                break;
            case SORTIDA:
                inc_stats(&sts.nca, e.on, NA, ntc, NA);
                tant = ta;
                ta = e.quan;
                actualitzar_stats_cua(cues, ntc, tant, ta, &sts, MAXQUHIST);
                j  = treu_cua(&cues[e.on], ta, &c);
                if (j != 0){
                    t = e.quan - c.tar;
                    inc_stats(sts.dqhist, e.on, (int)round(t), ntc, MAXDELHIST);
#if DEBUGserv == 1                                            
                    fprintf(ofile,"%.4lf Cua %d TEMPS total d'espera a la cua %.4lf (%3d)\n", ta, e.on, t, (int)round(t));
                    //print_hist(ofile, sts.dhist[e.on], MAXDELHIST, "", MAXPRINTCOL);                        
                    fflush(ofile);
#endif
                    t = 1+expo(SERVICE);                    
#if (DEBUGalea == 1)
                    fprintf(ofile, " utilitzat pel temps de servei %lf cua %d\n", 
                            t, e.on);
#endif
                    c.tse = t;
                    inc_stats(sts.dshist, e.on, (int)round(t), ntc, MAXDELHIST);
                    inc_stats(sts.dthist, e.on, (int)round((e.quan-c.tar)+c.tse), ntc, MAXDELHIST);
#if DEBUGserv == 1                               
                    fprintf(ofile,"%.4lf TEMPS SERVEI %.4lf: ARRIBADA %.4lf SORTIDA %.4lf\n", ta, t, c.tar,ta+t);
                    fflush(ofile);
#endif
                    e = crea_esdev(SORTIDA, ta+t, e.on);
                    posa_agenda(ta, e);
                }else{
                    cues[e.on].caixa = 0;
                }
                break;
            case TANCAR:
                bn = 0;
                break;
            default:
                fprintf(ofile,"ERROR: esdeveniment desconegut %d\n",e.que);
                return (-1);
        }// switch
    }// while
    
    // Prints arguments
    collect_stats(sts, ntc); 
    free_stats(sts, ntc);
    
    return (0); 

#if 0      // test queue
    c = crea_element_cua(2.1);
    j = posa_cua(c);
    c = crea_element_cua(12.1);
    j = posa_cua(c);
    c = crea_element_cua(22.1);
    j = posa_cua(c);
    j  = treu_cua(&c);
    j  = treu_cua(&c);
    c = crea_element_cua(6.1);
    j = posa_cua(c);
    c = crea_element_cua(33.1);
    j = posa_cua(c);
    j  = treu_cua(&c);
#endif
#if    0   // test agenda
    ini_agenda(N);
    e = crea_esdev(ARRIBADA, 9.2);
    posa_agenda(e);
    e = crea_esdev(OBRIR, 3.5);
    posa_agenda(e);
    e = crea_esdev(TANCAR, 30);
    posa_agenda(e);
    treu_agenda(&e);
    treu_agenda(&e);
    treu_agenda(&e);
#endif
 
}
