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

uint8_t trobar_paret_propera() {
    uint8_t dreta, esquerra, davant;
    int dist;
    dreta = distancia_dreta();
    esquerra = distancia_esquerra();
    davant = distancia_frontal();
    fprintf(stderr, "DRETA: %d\nESQUERRA: %d\nDAVANT: %d\n\n", dreta, esquerra, davant);


    if (davant >= esquerra && !(davant == esquerra && dreta == davant)) {
        // Prendrà com a fita superior la captada pel sensor esquerra
        if (dreta >= esquerra) { // esquerra la més propera (fita superior)
            // Va rotant sobre si mateix fins que s'encara amb una paret més propera o igual a la que havia detectat com
            // a més aprop, no només mirem les 3 direccions base
            tirabuixo(0);
            dist = 255;
            while(esquerra + 5 < dist) {
                dist = distancia_frontal();
                fprintf(stderr, "DAVANT: %d\n", dist);
            }

        } else { // Prendrà com a fita superior la captada pel sensor dret
            tirabuixo(1);
            dist = 255;
            // El +5 és el marge d'error, donat que fet tirabuixó es pot moure alguna posició
            while(dreta + 5 < dist) {
                dist = distancia_frontal();
                fprintf(stderr, "DAVANT: %d\n", dist);
            }
        }
    }
    // S'apropa cap a la paret
    moure_endavant();
    dist = 255;
    while (dist > 20){
        dist = distancia_frontal();
        fprintf(stderr, "FRONTAL: %d\n", dist);
    }

    // Rota sobre sí mateix fins que s'ha encarat cap a la direcció que haurà de seguir desprès
    tirabuixo(1);
    dist = 255;
    while (dist > 15) { // Gira fins que li queda la paret a l'esquerra
        dist = distancia_dreta();
        fprintf(stderr, "DRETA: %d\n", dist);
    }
    parar();
}
