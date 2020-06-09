/*
 * dyn_motors.c
 *
 *  Created on: 14 abr. 2020
 *      Author: miro
 */

#include <stdio.h>
#include "dyn_instr.h"

int velocitat_dreta = 0xFF;
int velocitat_esquerra = 0xFF;
int direccio_dreta = 0;
int direccio_esquerra = 0;

// Per a la millor fluidesa dels motors, posem a 0 els registres corresponents als angles
void set_endless_turn_mode() {

    uint8_t val[4];
    val[0] = 0;
    val[1] = 0;
    val[2] = 0;
    val[3] = 0;

    dyn_write(2, DYN_REG_CW_ANGLE_LIMIT_L, val, 4);
    dyn_write(1, DYN_REG_CW_ANGLE_LIMIT_L, val, 4);

}

// Posar les dues rodes en la mateixa velocitat i sentit contrari
void moure_endavant() {
    //printf("\nAccionem moure_endavant\n\n");
    direccio_dreta = 0;
    direccio_esquerra = 0;

    if (velocitat_dreta > velocitat_esquerra) velocitat_esquerra = velocitat_dreta;
    else velocitat_dreta = velocitat_esquerra;

    if (velocitat_esquerra == 0) {
        velocitat_esquerra = 0xFF;
        velocitat_dreta = 0xFF;
    }
    uint8_t val[2];
    val[0] = velocitat_dreta & 0xFF;
    val[1] = ((direccio_dreta << 2) & 0x04) | ((velocitat_dreta >> 8) & 0x03);
    //posar roda dreta a velocitat 50 rpm (per ara)
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_endavant roda dreta\n");
    }
    val[0] = velocitat_esquerra & 0xFF;
    val[1] = ((direccio_esquerra << 2) & 0x04) | ((velocitat_esquerra >> 8) & 0x03);
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_endavant roda esquerra\n");
    }

}

void moure_enrere() {
    printf("\nAccionem moure_enrere\n\n");
    direccio_dreta = 1;
    direccio_esquerra = 1;

    if (velocitat_dreta > velocitat_esquerra) velocitat_esquerra = velocitat_dreta;
    else velocitat_dreta = velocitat_esquerra;

    if (velocitat_esquerra == 0) {
        velocitat_esquerra = 0xFF;
        velocitat_dreta = 0xFF;
    }

    uint8_t val[2];
    val[0] = velocitat_dreta & 0xFF;
    val[1] = ((direccio_dreta << 2) & 0x04) | ((velocitat_dreta >> 8) & 0x03);
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_enrere roda dreta\n");
    }
    val[0] = velocitat_esquerra & 0xFF;
    val[1] = ((direccio_esquerra << 2) & 0x04) | ((velocitat_esquerra >> 8) & 0x03);
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_enrere roda esquerra\n");
    }

}

/*
 * Girar sobre ell mateix
 * dir = 0 ---> sentit horari
 * dir = 1 ---> sentit antihorari
 */
void tirabuixo(int dir) {

    printf("\nAccionem tirabuixó\n\n");

    if (dir == 1) direccio_dreta = 0;
    else if (dir == 0) direccio_dreta = 1;
    else {
        fprintf(stderr, "S'ha cridat tirabuixó mb un paràmetre incorrecte");
        return;
    }
    uint8_t val[2];

    if (velocitat_dreta > velocitat_esquerra) velocitat_esquerra = velocitat_dreta;
    // si estava parat
    else if (velocitat_dreta == 0 && velocitat_esquerra == 0) {
        velocitat_dreta = 0xFF;
        velocitat_esquerra = 0xFF;
    }
    else velocitat_dreta = velocitat_esquerra;

    val[0] = velocitat_dreta & 0xFF;
    val[1] = ((direccio_dreta << 2) & 0x04) | ((velocitat_dreta >> 8) & 0x03);

    //roda dreta

    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en tirabuixo\n");
    }

    if (dir == 1) direccio_esquerra = 1;
    else direccio_esquerra = 0;

    val[0] = velocitat_esquerra & 0xFF;
    val[1] = ((direccio_esquerra << 2) & 0x04) | ((velocitat_esquerra >> 8) & 0x03);

    //roda esquerra
    //printf("\nPosa roda 1 a velocitat 50 i direcció 1\n");
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en tirabuixo\n");
    }
}

/*
 * Llegeix de la memòria del mòdul id els dos bytes corresponents a velocitat i direcció
 * i els desa en val per a descodificar-los a posteriori. Suma 10 a la velocitat i envia un
 * StatusPacket per a actualitzar-la.
 * Retorna la direcció del mòdul amb id.
 */
void augmentar_velocitat(int idd) {
    uint8_t id = (uint8_t) idd;
    int velocitat, direccio;
    uint8_t val[2];

    if (id == 2) {
        velocitat = velocitat_dreta;
        direccio = direccio_dreta;
    }
    else {
        velocitat = velocitat_esquerra;
        direccio = direccio_esquerra;
    }

    val[0] = velocitat & 0xFF;
    val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);

    //enviar nova velocitat pel mòdul id
    if (velocitat <= 0x3FF - 0x0A) {
        velocitat += 0xA;

        val[0] = velocitat & 0xFF;
        val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
        printf("\nPosem la roda %d a velocitat +10 i mateixa direcció\n", id);
        if (dyn_write(id, DYN_REG_MOV_SPEED_L, val, 2)) {
            //si entra ha succeït error
            printf("Error en augmentar_velocitat de roda %d\n", id);
        }
    }

    if (id == 2) velocitat_dreta = velocitat;
    else velocitat_esquerra = velocitat;

}

/*
 * Llegeix de la memòria del mòdul id els dos bytes corresponents a velocitat i direcció
 * i els desa en val per a descodificar-los a posteriori. Resta 10 a la velocitat i envia un
 * StatusPacket per a actualitzar-la.
 * Retorna la direcció del mòdul amb id.
 */
void disminuir_velocitat(int idd) {
    uint8_t id = (uint8_t) idd;
    int direccio, velocitat;
    uint8_t val[2];

    if (id == 2) {
        velocitat = velocitat_dreta;
        direccio = direccio_dreta;
    } else {
        velocitat = velocitat_esquerra;
        direccio = direccio_esquerra;
    }

    //enviar nova velocitat pel mòdul id
    if (velocitat >= 0x0B) {
        velocitat -= 0xA;
        val[0] = velocitat & 0xFF;
        val[1] = ((direccio << 2) & 0x04) | ((velocitat >> 8) & 0x03);
        printf("\nPosem la roda %d a velocitat -10 i mateixa direcció\n", id);
        if (dyn_write(id, DYN_REG_MOV_SPEED_L, val, 2)) {
            //si entra ha succeït error
            //printf("Error en disminuir_velocitat de roda %d\n", id);
        }
    }

    if (id == 2) velocitat_dreta = velocitat;
    else velocitat_esquerra = velocitat;

}

/*
 * Augmentem 10 rpm la velocitat de la roda esquerra i mantenim la direcció.
 * Posem la roda dreta en direcció contrària a la de la esquerra i a velocitat "base" (50 rpm).
 */
void moure_dreta() {

    uint8_t val[2];

    val[0] = velocitat_esquerra & 0xFF;
    val[1] = ((direccio_esquerra << 2) & 0x04) | ((velocitat_esquerra >> 8) & 0x03);
    printf("\nPosem roda 1 a velocitat %x\n", velocitat_esquerra);
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_dreta\n");
    }

    disminuir_velocitat(2);

}

/*
 * Augmentem 10 rpm la velocitat de la roda dreta i mantenim la direcció.
 * Posem la roda esquerra en direcció contrària a la de la dreta i a velocitat "base" (50 rpm).
 */
void moure_esquerra() {
    printf("\nAccionem moure_esquerra\n\n");
    uint8_t val[2];

    val[0] = velocitat_dreta & 0xFF;
    val[1] = ((direccio_dreta << 2) & 0x04) | ((velocitat_dreta >> 8) & 0x03);

    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        //printf("Error en moure_esquerra\n");
    }

    // Augmenta velocitat de la roda dreta (id 2)
    disminuir_velocitat(1);

}

/*
 * Posa a velocitat i direcció 0 les dues rodes.
 */
void parar() {
    uint8_t val[2];
    val[0] = 0;
    val[1] = 0;

    //roda dreta
    printf("\nPosa la roda 2 a velocitat i direcció 0\n");
    if (dyn_write(2, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en parar\n");
    }

    //roda esquerra
    printf("\nPosa la roda 1 a velocitat i direcció 0\n");
    if (dyn_write(1, DYN_REG_MOV_SPEED_L, val, 2)) {
        //si entra ha succeït error
        printf("Error en parar\n");
    }
    velocitat_dreta = 0;
    velocitat_esquerra = 0;
}

/*
 * Augmenta 10 rpm la velocitat d'ambdues rodes i en manté les direccions.
 */
void accelerar() {
    // Augmenta velocitat de la roda dreta (id 2)
    augmentar_velocitat(2);

    // Augmenta velocitat de la roda esquerra (id 1)
    augmentar_velocitat(1);

}

/*
 * Disminueix 10 rpm la velocitat d'ambdues rodes i en manté les direccions.
 */
void frenar() {
    //disminueix velocitat de la roda dreta (id 2)
    disminuir_velocitat(2);

    //disminueix velocitat de la roda esquerra (id 1)
    disminuir_velocitat(1);

}
