#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <posicion.h>
#include <stdio.h>

#include "main.h"
#include "dyn/dyn_app_common.h"
#include "dyn/dyn_app_motors.h"
#include "dyn/dyn_app_sensor.h"
#include "dyn_test/dyn_emu.h"
#include "dyn_test/b_queue.h"
#include "joystick_emu/joystick.h"
#include "habitacion_001.h"

uint8_t estado = Ninguno, estado_anterior = Ninguno, finalizar = 0;
uint32_t indice;

/**
 * main.c
 */
int main(void) {
    pthread_t tid, jid;
    uint8_t tmp;

    // Per a no haver de fer fflush rere cada print
    setbuf(stdout, NULL);

    //Init queue for TX/RX data
    init_queue(&q_tx);
    init_queue(&q_rx);

    //Start thread for dynamixel module emulation
    pthread_create(&tid, NULL, dyn_emu, (void *) datos_habitacion);
    pthread_create(&jid, NULL, joystick_emu, (void *) &jid);

    printf("\nDimensiones habitacion %d ancho x %d largo mm2\n", ANCHO, LARGO);
    printf("En memoria: %I64u B = %I64u MiB\n", sizeof(datos_habitacion), sizeof(datos_habitacion) >> 20);

    printf("Pulsar 'q' para terminar, qualquier tecla para seguir\n");
    fflush(stdout);

    // Establim l'angle límit de rotació de les rodes a 0 (moviment continu)
    set_endless_turn_mode();
    // Anem a la paret més propera (en cas que trobi una a menys de 255 de la posició incial), sino endavant
    trobar_paret_propera();
    // Reseguim la paret en direcció antihorària (loop principal de l'aplicació)
    resseguir_paret();

    fflush(stdout);


    //return 0;
    while (estado != Quit) {
        if (simulator_finished) {
            break;
        }
        Get_estado(&estado, &estado_anterior);
        if (estado != estado_anterior) {
            Set_estado_anterior(estado);
            printf("estado = %d\n", estado);
            switch (estado) {
                case Sw1:
                    printf("Boton Sw1 ('a') apretado\n");
                    //dyn_led_control(1, 1); //Probaremos de encender el led del motor 2
                    //printf("\n");
                    parar();

                    break;
                case Sw2:
                    printf("Boton Sw2 ('s') apretado\n");
                    //dyn_led_control(1, 0); //Probaremos de apagar el led del motor 2
                    //aprintf("\n");
                    tirabuixo(0);
                    break;
                case Up:

                    break;
                case Down:

                    break;
                case Left:
                	//Comprobaremos si detectamos las esquinas de la pared izquierda:
    				printf("Esquina inferior izquierda:\n");
    				printf("(1, 1): %d (fuera pared)\n", obstaculo(1, 1, datos_habitacion));
    				printf("(0, 1): %d (pared izq.)\n", obstaculo(0, 1, datos_habitacion));
    				printf("(1, 0): %d (pared del.)\n", obstaculo(1, 0, datos_habitacion));
    				printf("(0, 0): %d (esquina)\n", obstaculo(0, 0, datos_habitacion));
    				printf("Esquina superior izquierda:\n");
    				printf("(1, 4094): %d (fuera pared)\n", obstaculo(1, 4094, datos_habitacion));
    				printf("(0, 4094): %d (pared izq.)\n", obstaculo(0, 4094, datos_habitacion));
    				printf("(1, 4095): %d (pared fondo.)\n", obstaculo(1, 4095, datos_habitacion));
    				printf("(0, 4095): %d (esquina)\n", obstaculo(0, 4095, datos_habitacion));
                    break;
                case Right:
                	//Comprobaremos si detectamos las esquinas de la pared derecha:
    				printf("Esquina inferior derecha:\n");
    				printf("(4094, 1): %d (fuera pared)\n", obstaculo(4094, 1, datos_habitacion));
    				printf("(4094, 0): %d (pared del.)\n", obstaculo(4094, 0, datos_habitacion));
    				printf("(4095, 1): %d (pared der.)\n", obstaculo(4095, 1, datos_habitacion));
    				printf("(4095, 0): %d (esquina)\n", obstaculo(4095, 0, datos_habitacion));
    				printf("Esquina superior derecha:\n");
    				printf("(4094, 4094): %d (fuera pared)\n", obstaculo(4094, 4094, datos_habitacion));
    				printf("(4094, 4095): %d (pared fondo)\n", obstaculo(4094, 4095, datos_habitacion));
    				printf("(4095, 4094): %d (pared der.)\n", obstaculo(4095, 4094, datos_habitacion));
    				printf("(4095, 4095): %d (esquina)\n", obstaculo(4095, 4095, datos_habitacion));
                    break;
                case Center:

                    break;
                case Quit:
                    printf("Adios!\n");
                    break;
                //etc, etc...
            }
            fflush(stdout);
        }
    }
    //Signal the emulation thread to stop
    pthread_kill(tid, SIGTERM);
    pthread_kill(jid, SIGTERM);
    printf("Programa terminado\n");
    fflush(stdout);
}
