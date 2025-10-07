/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 
 * Definicions de la llibreria de funcions dels algorismes de resolucio de col.lisions
 * 
 * File:   cra.h
 * Author: Dolors Sala
 */

#ifndef CRA_H
#define	CRA_H

int backoff(int n, char alg);
void compute_optimal_p();

#endif	/* CRA_H */

