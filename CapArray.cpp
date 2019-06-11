#include <Arduino.h>
#include <Wire.h>

#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"


uint8_t row_array[] = CAP_ROW_ARRAY;
uint8_t column_array[] = CAP_COLUMN_ARRAY;


// switches the given row to signal
int cap_switch_row_signal(uint8_t row, uint8_t signal) {
  // The whole row switches must be cleared before setting to a single signal
  cap_switch_clear_row(row);
  return mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(row), mux_get_addr_y(signal), MUX_SWITCH_ON, MUX_LDSW_LOAD );
}

// switches the given column to signal
int cap_switch_column_signal(uint8_t column, uint8_t signal) {
  cap_switch_clear_column(column);
  return mux_write_switch_config(I2C_ADDR_MUX2, mux_get_addr_x(column), mux_get_addr_y(signal), MUX_SWITCH_ON, MUX_LDSW_LOAD );
}


int cap_switch_all_rows_signal(uint8_t signal) {
  int i = 0;
  uint8_t row_array[] = CAP_ROW_ARRAY;
  for (i = 0; i < CAP_ROW_ARRAY_LEN; i++) {
    cap_switch_row_signal(row_array[i], signal);
  }
}


int cap_switch_all_columns_signal(uint8_t signal) {
  int i = 0;
  uint8_t col_array[] = CAP_COLUMN_ARRAY;
  for (i = 0; i < CAP_COLUMN_ARRAY_LEN; i++) {
    cap_switch_column_signal(col_array[i], signal);
  }
}

int cap_switch_clear_row(uint8_t row) {
  mux_clear_x(I2C_ADDR_MUX1, row);
}

int cap_switch_clear_column(uint8_t column) {
  mux_clear_x(I2C_ADDR_MUX2, column);
}

// To clear all switches, must go to each X to Y position and open them
int cap_switch_clear_all_rows() {
  int i = 0;
  uint8_t row_array[] = CAP_ROW_ARRAY;
  for (i = 0; i < CAP_ROW_ARRAY_LEN; i++) {
    cap_switch_clear_row(row_array[i]);
  }
}

int cap_switch_clear_all_columns() {
  int i = 0;
  uint8_t col_array[] = CAP_COLUMN_ARRAY;
  for (i = 0; i < CAP_COLUMN_ARRAY_LEN; i++) {
    cap_switch_clear_column(col_array[i]);
  }
}

int cap_print_row_connections(uint8_t row){
  uint8_t connections = 0;
  int i = 0;
  char * connection_names[] = CAP_ROW_Y_CONNECTIONS;
  connections = mux_read_x_connections(I2C_ADDR_MUX1, row_array[row]);
  Serial.print("Row ");Serial.print(row);Serial.print(" conn: ");
  for (i=0; i<MUX_READ_Y_ARRAY_LEN; i++){
    if ((connections>>i)&0x1){
      Serial.print(connection_names[i]);
      Serial.print(" ");
    }
  }
  Serial.println();
}


int cap_print_column_connections(uint8_t column){
  uint8_t connections = 0;
  int i = 0;
  char * connection_names[] = CAP_COLUMN_Y_CONNECTIONS;
  connections = mux_read_x_connections(I2C_ADDR_MUX2, column_array[column]);
  Serial.print("Col ");Serial.print(column);Serial.print(" conn: ");
  for (i=0; i<MUX_READ_Y_ARRAY_LEN; i++){
    if ((connections>>i)&0x1){
      Serial.print(connection_names[i]);
      Serial.print(" ");
    }
  }
  Serial.println();
}


int cap_print_connections(){
  int i,j;
  
  for (i=0;i<CAP_ROW_ARRAY_LEN; i++){
    cap_print_row_connections(i);
  }
  for (i=0;i<CAP_COLUMN_ARRAY_LEN; i++){
    cap_print_column_connections(i);
  }
}



// Uses the library functions to configure the measurement for a single capacitive sensor at position row x column
// For a single measurement, all rows and columns are set to SHLD1 except for the crossing row and column of the desired sensor
int cap_set_sensor_measurement_single(uint8_t position_row, uint8_t position_column){

  // Sets all rows and columns to SHLD1
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
  
  // Sets the capacitive sensor row to CIN1
  cap_switch_row_signal(row_array[position_row], CAP_ROW_CIN1);
  // Sets the capacitive sensor column to GND
  cap_switch_column_signal(column_array[position_column], CAP_COL_GND);
}



float cap_get_measurement_single(uint8_t position_row, uint8_t position_column, uint8_t capdac){
  uint32_t value;
  float capacitance;
  // Setting the capacitor configuration
  cap_set_sensor_measurement_single(position_row, position_column);

  // Setting the measurement configuration
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEAS1, CDC_ENABLE);

  // Wait for complete measurement
  while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEAS1));
  // reading value
  value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEAS1);
  // Converting value to capacitance
  capacitance = cdc_convert_capacitance(value, capdac);
  
  return capacitance;
}



float cap_get_measurement_iteration(uint8_t position_row, uint8_t position_column, uint8_t capdac){
  uint32_t value;
  float capacitance;
  // Setting the capacitor configuration
  cap_set_sensor_measurement_single(position_row, position_column);

  // Setting the measurement configuration
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEAS1, CDC_ENABLE);

  // Wait for complete measurement
  while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEAS1));
  // reading value
  value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEAS1);
  // Converting value to capacitance
  capacitance = cdc_convert_capacitance(value, capdac);
  
  return capacitance;
}
