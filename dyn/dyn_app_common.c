/*
 * dyn_motor.c
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 *
 * High-level functions common for both dynamixel motors and sensors modules.
 *
 */

#include <stdint.h>
#include <stdio.h>

#include "dyn/dyn_instr.h"
#include "dyn_app_sensor.h"
#include "dyn_app_motors.h"

void obstacle_davant();

void corregir_esquerra();

void corregir_dreta();

/**
 * Turn on or off the LED of a given dynamixel module
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[in] val Boolean value for the LED objective state
 */
int dyn_led_control(uint8_t id, bool val) {
    return dyn_write_byte(id, DYN_REG__LED, (uint8_t) val);

}

/**
 * Read the current LED status
 *
 * @param[in] id Id of the dynamixel moduel
 * @param[out] val Current LED status
 */
int dyn_led_read(uint8_t id, uint8_t *val) {
    return dyn_read_byte(id, DYN_REG__LED, val);
}

void trobar_paret_propera() {
    uint8_t dreta, esquerra, davant;
    int dist;
    dreta = distancia_dreta();
    esquerra = distancia_esquerra();
    davant = distancia_frontal();

    if (davant >= esquerra && !(davant == esquerra && dreta == davant)) {
        // Prendrà com a fita superior la captada pel sensor esquerra
        if (dreta >= esquerra) { // esquerra la més propera (fita superior)
            // Va rotant sobre si mateix fins que s'encara amb una paret més propera o igual a la que havia detectat com
            // a més aprop, no només mirem les 3 direccions base
            tirabuixo(0);
            dist = 255;
            while(esquerra + 5 < dist) {
                dist = distancia_frontal();
            }
            parar();
        } else { // Prendrà com a fita superior la captada pel sensor dret
            tirabuixo(1);
            dist = 255;
            // El +5 és el marge d'error, donat que fet tirabuixó es pot moure alguna posició
            while(dreta + 5 < dist) {
                dist = distancia_frontal();
            }
            parar();
        }
    }
    // S'apropa cap a la paret
    moure_endavant();
    dist = 255;
    while (dist > 20){
        dist = distancia_frontal();
    }

    // Rota sobre sí mateix fins que s'ha encarat cap a la direcció que haurà de seguir desprès
    tirabuixo(1);
    dist = 255;
    while (dist - 2 > 20) { // Gira fins que li queda la paret a l'esquerra
        dist = distancia_dreta();
    }
    return;
}

void resseguir_paret() {
    printf("\nHa entrat resseguir\n\n");
    bool correcte = true, davant = false;

    moure_endavant();

    while (correcte) {
        //hem trobat un obstacle davant
        if (distancia_frontal() < 20) {
            obstacle_davant();
        }
        if (distancia_dreta() < 20) {
            corregir_esquerra();
        }
        if (distancia_dreta() > 30) {
            corregir_dreta();
        }
    }
}

void corregir_dreta() {
    printf("\nHa entrat corregir_dreta\n\n");
    moure_dreta();
    while (distancia_dreta() > 30) {}
    moure_endavant();
    return;
}

void corregir_esquerra() {
    printf("\nHa entrat corregir_esq\n\n");
    moure_esquerra();
    while (distancia_dreta() < 20) {}
    moure_endavant();
    return;
}

/*
 * Funció que prén la desició de què fer quan el robot es troba un obstacle davant a menys de 20mm.
 */
void obstacle_davant() {
    printf("\nHa entrat obstacle davant\n\n");
    if (distancia_esquerra() > 30) {
        //gira fins que no hi ha obstacles davant a menys de 30 mm
        tirabuixo(1);
    }
    else if (distancia_dreta() > 30) {
        //gira fins que no hi ha obstacles davant a menys de 30 mm
        tirabuixo(0);
    }
    else {
        //gira fins que no hi ha obstacles davant a menys de 30 mm
        tirabuixo(1);
    }
    while (distancia_frontal() < 30) {}
    printf("\nHa sortit del while i ara va endavant\n\n");
    moure_endavant();
    return;
}
