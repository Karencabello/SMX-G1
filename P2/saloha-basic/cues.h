/*
 * Programa exemple del funcionament d'una simulacio orientada a temps
 * Implementa slotted aloha (model simplificat)
 * 
 * Use: sloha.exe <name-input-file> <name-output-file>
 * Example: sloha.exe ./src/in ./src/out
 
 * Definicions de la llibreria de funcions de gestio de cues
 * 
 * File:   cues.h
 * Author: Dolors Sala
 */

#ifndef CUES_H
#define	CUES_H

equeue create_qu_element(int num, double atime);
void create_queue(sstation *s, int max);
void add_qu_element(squeue *q, equeue e);
int delete_qu_element(squeue *q, equeue *e);
void set_start_service_time(equeue *pk, long stime);
int queu_length(squeue s);
void free_queue(squeue *q);
void print_queue(squeue q);

#endif	/* CUES_H */

