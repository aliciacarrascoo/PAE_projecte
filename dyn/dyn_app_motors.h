/*
 * dyn_motors.h
 *
 *  Created on: 14 abr. 2020
 *      Author: miro
 */

#ifndef DYN_DYN_APP_MOTORS_H_
#define DYN_DYN_APP_MOTORS_H_

void set_endless_turn_mode();
void moure_endavant();
void moure_enrere();
void moure_dreta();
void moure_esquerra();
void tirabuixo(int);
int augmentar_velocitat(int);
int disminuir_velocitat(int);
void parar();
void accelerar();
void frenar();

#endif /* DYN_DYN_APP_MOTORS_H_ */
