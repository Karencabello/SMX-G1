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

static scua *cues;  //cues ciculars amb assignacio dinamica de memoria

// Imprimeix l'element de la cua que es passa com a paràmetre
void imprimir_element_cua(el_cua elem){
    //printf(ofile,"(%7.4lf %7.4lf) ", elem.tar, elem.tse);
    fprintf(ofile,"%7.4lf ", elem.tar);
}

// Imprimeix la cua circular
void imprimir_cua(scua cua){
    int i, f;
#if DEBUGcua == 1
    fprintf(ofile,"Cua %d (I %2d,F %2d, L %2d): ", 
            cua.idcua, cua.ini_cua, cua.fin_cua, cua.lon_cua);
    if (cua.fin_cua >= cua.ini_cua)
        f = cua.fin_cua;
    else
        f = CUA_MAX-1;
#if 0
    if(cua.lon_cua > 0){
        for (i = cua.ini_cua; i <= f; i++){
            imprimir_element_cua(cua.elem[i]);
        }
        if(f != cua.fin_cua){
            for (i = 0; i <= cua.fin_cua; i++){
                imprimir_element_cua(cua.elem[i]);
            }
        }
    }
#else      
    int pos = 0;
    for (i = 0; i < long_cua(cua); i++){
        pos = ((cua.ini_cua + i) % CUA_MAX);
        imprimir_element_cua(cua.elem[pos]);
    }
#endif
    
    fprintf(ofile,"\n");
#endif
}

// Imprimeix les ntc cues
void imprimir_cues(scua *cues, int ntc){
    int i, f, c;
#if DEBUGcua == 1
    for(c = 0; c < ntc; c++){
        //fprintf(ofile,"Cua %d ", cues[c].idcua);
        imprimir_cua(cues[c]);
    }
    fprintf(ofile,"\n");
#endif
}

// Crea un element de la cua
el_cua crea_element_cua(float tar, float tse){
    el_cua c;
    
    c.tar = tar;
    c.tse = tse;
    return(c);
}//crea_element_cua

// Crea una cua circular buida de capacitat per max_cua elements
void crea_cua(scua *cua, int max, int idcua){
    int c;
    cua->elem = (el_cua*)malloc(max * sizeof(el_cua));
    if (cua == NULL){
        fprintf(ofile,"ERROR Crea cua %d: No hi ha prou memoria per crear cua buida", c);
        exit(0);
    }
    if (cua->elem == NULL){
        fprintf(stderr, "ERROR Crea cua %d: No hi ha prou memoria per crear cua buida\n", idcua);
        exit(EXIT_FAILURE);
    }
    
    cua->idcua = idcua;
    cua->ini_cua = -1;
    cua->fin_cua = -1;
    cua->lon_cua = 0; 
    cua->max_cua = max;
    cua->caixa   = 0;
}//crea_cua

// Crea totes les cues circulars buides de capacitat per max_cua elements
void crea_cues(scua **pcua, int max, int ntc){
    scua *cues = *pcua;
    int c;
    cues = (scua*) malloc(ntc * sizeof(scua));
    if (cues == NULL){
        puts("ERROR Crea cua : No hi ha prou memoria per crear cua buida");
        exit(0);
    }
    for(c = 0; c < ntc; c++)
        crea_cua(&cues[c], max, c);
    imprimir_cues(cues, ntc);
    *pcua = cues;
}//crea_cues

//Inserta un valor "c" a la cua circular cua
// El ta és el temps actual per imprimir en les traces de seguiment.
int posa_cua(scua *cua, float ta, el_cua c){
    if (cua->lon_cua == cua->max_cua) {
        fprintf(ofile,"ERROR CUA %d: No hi ha espai a la cua. Incrementa max_cua", cua->idcua);
        exit(0);
    }
    
    cua->fin_cua++;
    if(cua->fin_cua == cua->max_cua) cua->fin_cua = 0; // Primer element a la cua
    if(cua->lon_cua == 0) cua->ini_cua = cua->fin_cua;
    ++cua->lon_cua;
  
    cua->elem[cua->fin_cua] = c;

#if DEBUGcua == 1
    fprintf(ofile,"%.4lf POSA CUA %d... pos %2d elem %7.4lf ", 
            ta, cua->idcua, cua->fin_cua, c.tar);
#endif
        
    imprimir_cua(*cua);
    return (1);
}// posa_cua

// Treure el seguent element de la cua circular cua i el retorna a c
// El ta és el temps actual per imprimir en les traces de seguiment.
int treu_cua(scua *cua, float ta, el_cua *c){
    int ret = 1; 
    
    if(cua->lon_cua == 0) {
        ret = 0;
    }
    else { // si hi ha elements a la cua
#if DEBUGcua == 1
        fprintf(ofile,"%.4lf TREU CUA %d... pos %2d ", ta, cua->idcua, cua->ini_cua);
        if(cua->lon_cua > 0) imprimir_element_cua(cua->elem[cua->ini_cua]);
        fprintf(ofile,"(long %d) ", cua->lon_cua);
#endif
        *c = cua->elem[cua->ini_cua];
        ++cua->ini_cua;
        if (cua->ini_cua == cua->max_cua) cua->ini_cua = 0;
        --cua->lon_cua;
        if(cua->lon_cua == 0) cua->ini_cua = cua->fin_cua = -1;
        imprimir_cua(*cua);
    }
    return(ret);
}// treu_cua

// Retorna quants elements te la cua circula cua
int long_cua(scua cua){
    return(cua.lon_cua);
}

//Allibera l'espai de la cua circular "cua"
void elim_cua(scua *cua){
    free(cua->elem);
}

//Allibera l'espai de totes (ntc) les cues
void elim_cues(scua *cues, int ntc){
    int c;
    for(c = 0; c < ntc; c++)
        elim_cua(&cues[c]);
    free(cues);
}

// Decideix a quina cua/caixer posar el nou client
// la cua amb menys clients esperant
int cua_mes_curta(scua *cues, int inici, int final){
    int c;
    //el_cua e;
    int millor = inici;
    int minlong = long_cua(cues[inici]);
    
    // En principi la de menys clients és la primera)
    //e.on  = 0;
    //e.tar = long_cua(cues[e.on]);
    
    // recorre resta de cues per identificar la més curta
    for(c = inici + 1; c <= final; c++){
        if (long_cua(cues[c]) < minlong){
            millor = c;
            minlong = long_cua(cues[c]);
        }
#if (DEBUG-quina-cua == 1)
        fprintf(ofile, "Quina cua? Cua %d long %d (minim: cua %d long %d)\n", 
                c, cues[c].lon_cua, e.on, e.tar);
#endif
    } 
#if (DEBUGquinaCua == 1)
       // fprintf(ofile, "Quina cua? Cua %d long %f !!!!!!!!!\n", e.on, e.tar);
#endif
    return millor;
} // cua_mes_curta

int primer_caixer_buit(scua *cues, int inici, int final){
    int c = inici;
    int ret = NA;

    while(c <= final && cues[c].caixa != 0)
        c++;
    if(c <= final)
        ret = c;
    else
        ret = c;
#if (DEBUGquinaCua == 1)
    fprintf(ofile, "Primer Caixer Buit %d \n", ret);
#endif
   return(ret);
   
} // primer_caixer_buit
