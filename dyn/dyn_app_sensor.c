/*
 * dyn_sensor.c
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 *
 * TODO: High-level functions like "distance_wall_front", etc
 */

#include <stdio.h>
#include "dyn_instr.h"

uint8_t distancia_frontal() {
    uint8_t val = 0;
    printf("\nLlegim la distància frontal\n");
    while (dyn_read_byte(3, DYN_CENTER_IR_DATA, &val) != 0) {}
    return val;
}

uint8_t distancia_esquerra() {
    uint8_t val = 0;
    printf("\nLlegim la distància esquerra\n");
    while (dyn_read_byte(3, DYN_LEFT_IR_DATA, &val) != 0) {}
    return val;
}

uint8_t distancia_dreta() {
    uint8_t val = 0;
    printf("\nLlegim la distància dreta\n");
    while (dyn_read_byte(3, DYN_RIGHT_IR_DATA, &val) != 0) {}
    return val;
}


