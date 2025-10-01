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

el_cua crea_element_cua(float tar, float tse);
void crea_cues(scua **pcua, int max, int ntc);
int posa_cua(scua *cua, float ta, el_cua c);
int treu_cua(scua *cua, float ta, el_cua *c);
int long_cua(scua cua);
int cua_mes_curta(scua *cues, int ntc);
int primer_caixer_buit(scua *cues, int ntc);
#endif	/* CUA_H */

