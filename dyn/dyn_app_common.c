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

uint8_t trobar_pared_propera() {
    uint8_t dreta, esquerra, davant;
    dreta = distancia_dreta();
    esquerra = distancia_esquerra();
    davant = distancia_frontal();
    //
    if (davant > esquerra) {
        // Prendrà com a fita superior la captada pel sensor esquerra
        if (dreta > esquerra) { // esquerra la més propera (fita superior)
            // NO SABEM QUANT ÉS 90 GRAUS
            // Va rotant sobre si mateix fins que s'encara amb una paret més propera o igual a la que havia detectat com
            // a més aprop, no només mirem les 3 direccions base
            while(esquerra < distancia_frontal()) {
                tirabuixo();
            }

        } else { // Prendrà com a fita superior la captada pel sensor dret
            while(dreta < distancia_frontal()) {
                tirabuixo();
            }
        }
    }
    while (distancia_frontal() > 5) {
        moure_endavant(); //TODO: Passar-li per paràmetre velocitat
    }
    while (distancia_dreta() > 5) { // Es mou mentre gira??
        tirabuixo();
    }
}
