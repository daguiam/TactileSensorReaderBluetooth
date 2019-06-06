#include <Arduino.h>
#include <Wire.h>

#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"




// switches the given row to signal
int cap_switch_row(uint8_t row, uint8_t signal){
    mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(row), mux_get_addr_y(signal), MUX_SWITCH_ON, MUX_LDSW_LOAD );
    
}

// switches the given column to signal
int cap_switch_column(uint8_t column, uint8_t signal){
    mux_write_switch_config(I2C_ADDR_MUX2, mux_get_addr_x(column), mux_get_addr_y(signal), MUX_SWITCH_ON, MUX_LDSW_LOAD );
    
}
