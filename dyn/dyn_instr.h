/*
 * hal_dyn.h
 *
 *  Created on: 18 mar. 2020
 *      Author: droma
 */

#ifndef DYN_INSTR_H_
#define DYN_INSTR_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum _dyn_instr_type {
    DYN_INSTR__READ = 2,
    DYN_INSTR__WRITE = 3,
} DYN_INSTR_t;

typedef enum _dyn_reg_type {
    DYN_REG_CW_ANGLE_LIMIT_L = 0x06,
    DYN_REG_CW_ANGLE_LIMIT_H = 0x07,
    DYN_REG_CCW_ANGLE_LIMIT_L = 0x08,
    DYN_REG_CCW_ANGLE_LIMIT_H = 0x09,
    DYN_REG__LED = 0x19,
    DYN_LEFT_IR_DATA = 0x1A,
    DYN_CENTER_IR_DATA = 0x1B,
    DYN_RIGHT_IR_DATA = 0x1C,
    DYN_REG_GOAL_SPEED_L = 0x1E,
    DYN_REG_GOAL_SPEED_H  = 0x1F,
    DYN_REG_MOV_SPEED_L = 0x20,
    DYN_REG_MOV_SPEED_H = 0x21,
    DYN_REG__MAX = 0x32,
} DYN_REG_t;

int dyn_write_byte(uint8_t module_id, DYN_REG_t reg_addr, uint8_t reg_write_val);

int dyn_read_byte(uint8_t module_id, DYN_REG_t reg_addr, uint8_t *reg_read_val);

int dyn_write(uint8_t module_id, DYN_REG_t reg_addr, uint8_t *val, uint8_t len);

#endif /* DYN_INSTR_H_ */
