/*
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * LLibreria de funcions del sistema de cues
 * 
 * File:   cua.c
 * Author: Dolors Sala
 */

#include "./sev.h"
#include "./cua.h"

#define DEBUGcua 1  // Bandera per fer seguiment de la cua

static el_cua *cua; //cua cicular amb assignacio dinamica de memoria
static int max_cua; // capacitat maxima d'elements a la cua
static int ini_cua; // Primer element de la cua
static int fin_cua; // ultim element de la cua
static int lon_cua; // Quantitat d'elements a la cua

// Imprimeix element i de la cua circular
void imprimir_element_cua(el_cua *cua, int i){
    printf("%7.4lf ",cua[i].tar);
}
// Imprimeix la cua circular
void imprimir_cua(el_cua *cua){
    int i, f;

    printf("Cua (I %2d,F %2d, L %2d): ",ini_cua, fin_cua, lon_cua);
    if (fin_cua >= ini_cua)
        f = fin_cua;
    else
        f = CUA_MAX-1;
 
    if(lon_cua > 0){
        for (i = ini_cua; i <= f; i++){
            printf("%7.4lf ",cua[i].tar);
        }
        if(f != fin_cua){
            for (i = 0; i <= fin_cua; i++){
                imprimir_element_cua(cua, i);
            }
        }
    }
     
    printf("\n");
}

// Crea un element de la cua
el_cua crea_element_cua(float tar){
    el_cua c;
    
    c.tar = tar;
    return(c);
}//crea_element_cua

// Crea una cua circular buida de capacitat per max_cua elements
void crea_cua(int max){
    max_cua = max;
    cua = (el_cua*) malloc( max_cua*sizeof(el_cua));
    if (cua == NULL){
        puts("Crea cua : No hi ha prou memoria per crear cua buida");
        exit(-1);
    }
    ini_cua = -1;
    fin_cua = -1;
    lon_cua = 0;     

#if DEBUGcua == 1
        imprimir_cua(cua);
#endif
}//crea_cua

//Inserta un valor "c" a la cua circular
// El ta és el temps actual per imprimir en les traces de seguiment.
int posa_cua(float ta, el_cua c){
    if (lon_cua == max_cua) {
        puts("ERROR CUA: No hi ha espai a la cua. Incrementa max_cua");
        exit(-1);
    }
    
    ++fin_cua;
    if(fin_cua == max_cua) fin_cua = 0; // Primer element a la cua
    if(lon_cua == 0) ini_cua = fin_cua;
    ++lon_cua;
  
    cua[fin_cua] = c;

#if DEBUGcua == 1
    printf("%.4lf POSA CUA... pos %2d elem %7.4lf ",ta,fin_cua,c.tar);
    imprimir_cua(cua);
#endif

    return (1);
}// posa_cua

// Treure el seguent element de la cua circular
// El ta és el temps actual per imprimir en les traces de seguiment.
int treu_cua(float ta, el_cua *c){
    int ret = 1; 
    
    if(lon_cua == 0) {
        ret = 0;
    }
    else { // si hi ha elements a la cua
#if DEBUGcua == 1
        printf("%.4lf TREU CUA... pos %2d ",ta,ini_cua);
        if(lon_cua > 0) imprimir_element_cua(cua, ini_cua);
        printf("(long %d) ", lon_cua);
#endif
        *c = cua[ini_cua];
        ++ini_cua;
        if (ini_cua == max_cua) ini_cua = 0;
        --lon_cua;
        if(lon_cua == 0) ini_cua = fin_cua = -1;

#if DEBUGcua == 1
    imprimir_cua(cua);
#endif
    }
    return(ret);
}// treu_cua

// Retorna quants elements te la cua circula
int long_cua(void){
    return(lon_cua);
}

//Allibera l'espai de la cua circular "cua"
void elim_cua(void){
    free(cua);
}


