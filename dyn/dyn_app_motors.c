/*
 * dyn_motors.c
 *
 *  Created on: 14 abr. 2020
 *      Author: miro
 */

#include <stdio.h>
#include "dyn_instr.h"

// Per a la millor fluidesa dels motors, posem a 0 els registres corresponents als angles
void set_endless_turn_mode() {

    uint8_t val = 0;

    dyn_write(2, DYN_REG_CW_ANGLE_LIMIT_L, &val, 4);
    dyn_write(1, DYN_REG_CW_ANGLE_LIMIT_L, &val, 4);

}

// Posar les dues rodes en la mateixa velocitat i sentit contrari
void moure_endavant() {

    int velocitat = 0x032;
    int direccio = 1;
    uint8_t val[2];
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nEstem posant a velocitat %d el motor 1 i a direcció %d\n", velocitat, direccio);
    //posar roda dreta a velocitat 50 rpm (per ara)
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_endavant roda dreta\n");
    }
    direccio = 0;
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nEstem posant a velocitat %d el motor 2 i a direccio %d\n", velocitat, direccio);
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_endavant roda esquerra\n");
    }

}

void moure_enrere() {

    int velocitat = 0x032;
    int direccio = 0;
    uint8_t val[2];
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nEstem posant a velocitat %d el motor 1 i a direcció %d\n", velocitat, direccio);
    //posar roda dreta a velocitat 50 rpm (per ara)
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_enrere roda dreta\n");
    }
    direccio = 1;
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nEstem posant a velocitat %d el motor 2 i a direccio %d\n", velocitat, direccio);
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_enrere roda esquerra\n");
    }

}

/*
 * Girar sobre ell mateix en sentit horari a velocitat 50 rpm i direcció 1 les dues rodes
 */
void tirabuixo() {
    int direccio = 1, velocitat = 0x32;
    uint8_t val[2];

    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);

    //roda dreta
    printf("\nPosa roda 1 a velocitat 50 i direcció 1\n");
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en tirabuixo\n");
    }

    //roda esquerra
    printf("\nPosa roda 2 a velocitat 50 i direcció 1\n");
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en tirabuixo\n");
    }
}

/*
 * Llegeix de la memòria del mòdul id els dos bytes corresponents a velocitat i direcció
 * i els desa en val per a descodificar-los a posteriori. Suma 10 a la velocitat i envia un
 * StatusPacket per a actualitzar-la.
 * Retorna la direcció del mòdul amb id.
 */
int augmentar_velocitat(int idd) {
    uint8_t id = (uint8_t) idd;
    int direccio, velocitat;
    uint8_t val[2];

    val[0] = 0;
    val[1] = 0;

    //llegir velocitat i direcció de mòdul id
    printf("\nLlegim la velocitat i direcció de la roda %d\n", id);
    dyn_read_byte(id, DYN_REG_MOV_SPEED_L, &val[0]);
    dyn_read_byte(id, DYN_REG_MOV_SPEED_H, &val[1]);
    direccio = val[1];
    direccio &= 0x04;
    direccio = direccio >> 2;
    val[1] &= 0x03;
    velocitat = val[0] + (val[1] << 8);

    //enviar nova velocitat pel mòdul id
    if (velocitat == 0x3FF - 0x0A) velocitat += 0xA;
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nPosem la roda %d a velocitat +10 i mateixa direcció\n", id);
    if (dyn_write(id, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en augmentar_velocitat de roda %d\n", id);
    }

    return direccio;
}

/*
 * Llegeix de la memòria del mòdul id els dos bytes corresponents a velocitat i direcció
 * i els desa en val per a descodificar-los a posteriori. Resta 10 a la velocitat i envia un
 * StatusPacket per a actualitzar-la.
 * Retorna la direcció del mòdul amb id.
 */
int disminuir_velocitat(int idd) {
    uint8_t id = (uint8_t) idd;
    int direccio, velocitat;
    uint8_t val[2];

    val[0] = 0;
    val[1] = 0;

    //llegir velocitat i direcció de mòdul id
    dyn_read_byte(id, DYN_REG_MOV_SPEED_L, &val[0]);
    dyn_read_byte(id, DYN_REG_MOV_SPEED_H, &val[1]);
    direccio = val[1];
    direccio &= 4;
    direccio = direccio >> 2;
    val[1] &= 3;
    velocitat = val[0] + (val[1] << 8);

    //enviar nova velocitat pel mòdul id
    if (velocitat >= 0x0B)velocitat -= 0xA;
    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nPosem la roda %d a velocitat -10 i mateixa direcció\n", id);
    if (dyn_write(id, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en disminuir_velocitat de roda %d\n", id);
    }

    return direccio;
}

/*
 * Augmentem 10 rpm la velocitat de la roda esquerra i mantenim la direcció.
 * Posem la roda dreta en direcció contrària a la de la esquerra i a velocitat "base" (50 rpm).
 */
void moure_dreta() {
    int velocitat = 0x32, direccio;
    uint8_t val[2];

    // Augmenta velocitat de la roda esquerra (id 2)
    direccio = augmentar_velocitat(2);

    //enviar velocitat "base" a la roda dreta per assegurar que es mantingui en aquesta
    //la posem a direcció contrària a la de la esquerra per que segueixi el robot en la direcció que estava
    if (direccio == 1) direccio = 0;
    else direccio = 1;

    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    printf("\nPosem roda 2 a velocitat 50 i direcció contrària a la roda 1\n");
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_dreta\n");
    }
}

/*
 * Augmentem 10 rpm la velocitat de la roda dreta i mantenim la direcció.
 * Posem la roda esquerra en direcció contrària a la de la dreta i a velocitat "base" (50 rpm).
 */
void moure_esquerra() {
    int velocitat = 0x32, direccio;
    uint8_t val[2];

    // Augmenta velocitat de la roda dreta (id 1)
    direccio = augmentar_velocitat(1);

    //enviar velocitat "base" a la roda esquerra per assegurar que es mantingui en aquesta
    //la posem a direcció contrària a la de la dreta per que segueixi el robot en la direcció que estava
    if (direccio == 1) direccio = 0;
    else direccio = 1;

    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en moure_esquerra\n");
    }
}

/*
 * Posa a velocitat i direcció 0 les dues rodes.
 */
void parar() {
    uint8_t val[2];
    val[0] = 0;
    val[1] = 0;

    //roda dreta
    printf("\nPosa la roda 1 a velocitat i direcció 0\n");
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en parar\n");
    }

    //roda esquerra
    printf("\nPosa la roda 2 a velocitat i direcció 0\n");
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en parar\n");
    }
}

/*
 * Augmenta 10 rpm la velocitat d'ambdues rodes i en manté les direccions.
 */
void accelerar() {
    // Augmenta velocitat de la roda dreta (id 1)
    augmentar_velocitat(1);

    // Augmenta velocitat de la roda esquerra (id 2)
    augmentar_velocitat(2);

}

/*
 * Disminueix 10 rpm la velocitat d'ambdues rodes i en manté les direccions.
 */
void frenar() {
    //disminueix velocitat de la roda dreta (id 1)
    disminuir_velocitat(1);

    //disminueix velocitat de la roda esquerra (id 2)
    disminuir_velocitat(2);

}
