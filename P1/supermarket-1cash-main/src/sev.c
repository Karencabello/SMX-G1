/*
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * Programa principal
 * Modificacions segons llibre:
 *  1) Organitza codi en funcions
 *  2) Separa el codi en fitxers
 *  3) Incorpora traces per veure què fa el programa i permet activar-ho o no
 * 
 * File:   sev.c
 * Author: Dolors Sala
 */

#include "./sev.h"
#include "./cua.h"
#include "./agenda.h"
#include <time.h>


#define DEBUGserv 1  // Bandera per fer seguiment del servei

int main(void) {
    esdev e;
    el_cua c;
    float t;
    float ta = 0; // Temps actual que avança la simulació
    int bn;     // bandera que indica si caixa oberta 1 o tancada 0  
    int caixa;  // indica si el caixer esta ocupat 1 o no 0
    float tmax; // temps maxim en el sistema
    int nca;    // nombre de clients atesos
    int j;
    //Inicialitzar generador numeros aleatoris
    long int llavor = RANSEED;
    if(llavor == 0){  // Zero for random start 
        llavor = time(0); 
        printf("\nRandom seed, seed chosen: %ld\n",llavor);
    }
    srand(llavor);    

    
    ini_agenda(N);
    crea_cua(CUA_MAX);

    e = crea_esdev(OBRIR, OBRIRTIME);
    posa_agenda(ta, e);
    
    e = crea_esdev(TANCAR, TANCARTIME);
    posa_agenda(ta, e);
    
    bn = 0;
    caixa = 0;
    nca = 0;
    tmax = 0.0;
    
    while (treu_agenda(ta, &e) != 0){
        
        switch (e.que){
            case OBRIR:
                bn = 1;
                caixa = 0;
                t = expo(ARRIVAL);
                e = crea_esdev(ARRIBADA, t);
                posa_agenda(ta, e);
                break;
            case ARRIBADA:
                if(bn == 1){
                    ta = e.quan;
                    // si la caixa buida passa directament a ser servit
                    if (caixa == 0){
                        caixa = 1;
                        t = 1 + expo(SERVICE);
#if DEBUGserv == 1    
                        printf("%.4lf TEMPS servei %.4lf: ARRIBADA %.4lf SORTIDA %.4lf\n", ta, t, e.quan,ta+t);
#endif
                        e = crea_esdev(SORTIDA, ta+t);
                        posa_agenda(ta, e);
                    }else{ // posar element a la cua d'espera
                        c.tar = ta;
                        j = posa_cua(ta, c);
                        if(j == 0){
                            puts("ERROR: cua massa petita");
                            exit(1);
                        }
                    }//else
                    // Decidir la seguent arribada
                    t = ta + expo(ARRIVAL);
                    e = crea_esdev(ARRIBADA, t);
                    posa_agenda(ta, e);
                }//bn==1
                break;
            case SORTIDA:
                ++nca;
                ta = e.quan;
                j  = treu_cua(ta, &c);
                if (j != 0){
                    t = e.quan - c.tar;
                    if (t > tmax) tmax = t;
                    t = 1+expo(SERVICE);                    
#if DEBUGserv == 1                               
                        printf("%.4lf TEMPS servei %.4lf: ARRIBADA %.4lf SORTIDA %.4lf\n", ta, t, c.tar,ta+t);
#endif
                    e = crea_esdev(SORTIDA, ta+t);
                    posa_agenda(ta, e);
                }else{
                    caixa = 0;
                }
                break;
            case TANCAR:
                bn = 0;
                break;
            default:
                printf("ERROR: esdeveniment desconegut %d\n",e.que);
                return (-1);
        }// switch
    }// while
    
    // Prints arguments
    printf("\n\n-------------------------\n");
    printf("Nombre de clients atesos: %d\n",nca);
    printf("Temps maxim de cua: %f\n",tmax);
  
    return (1);

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
