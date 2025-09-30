/*  
 * Programa exemple del funcionament d'una simulacio orientada en events
 * basada en el codi donat en el llibre Jorba, Capitol 2
 * 
 * Declaracions per la implementacio de la cua
 * 
 * File:   cua.h
 * Author: Dolors Sala
 */

#ifndef CUA_H
#define	CUA_H

// Estructura d'un element de la cua
typedef struct{
    float tar; // temps d'arribada a la cua
}el_cua; 

#define CUA_MAX 4//7  // nombre maxim elements a la cua

el_cua crea_element_cua(float tar);
void crea_cua(int max);
int posa_cua(float ta, el_cua c);
int treu_cua(float ta, el_cua *c);
int long_cua(void);

#endif	/* CUA_H */

