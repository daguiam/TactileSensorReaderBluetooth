#include <Arduino.h>
#include <Wire.h>

#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"
#include "MemStorage.h"


uint8_t row_array[] = CAP_ROW_ARRAY;
uint8_t column_array[] = CAP_COLUMN_ARRAY;





// switches the given row to signal
int cap_switch_row_signal(uint8_t row, uint8_t signal, uint8_t data, uint8_t clear) {
  // The whole row switches must be cleared before setting to a single signal
  if (clear){
    cap_switch_clear_row(row);  
  }
  
  return mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(row), mux_get_addr_y(signal), data, MUX_LDSW_LOAD );
}

// switches the given column to signal
int cap_switch_column_signal(uint8_t column, uint8_t signal, uint8_t data, uint8_t clear) {
  if (clear){
    cap_switch_clear_column(column);
  }
  return mux_write_switch_config(I2C_ADDR_MUX2, mux_get_addr_x(column), mux_get_addr_y(signal), data, MUX_LDSW_LOAD );
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

//    cap_print_connections();

}



float cap_get_measurement_single(uint8_t position_row, uint8_t position_column, uint8_t capdac ){
  uint32_t value;
  float capacitance;
  // Setting the capacitor configuration
  cap_set_sensor_measurement_single(position_row, position_column);

  // Setting the measurement configuration
  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEAS1, CDC_ENABLE);

  // Wait for complete measurement (around 2.5ms)
  while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEAS1));
  // reading value
  value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEAS1);

//  Serial.println(value,HEX);
  // Converting value to capacitance
  capacitance = cdc_convert_capacitance_normalized(value);
//  capacitance = cdc_convert_capacitance(value, capdac);
  
  return capacitance;
}



float * cap_get_measurement_iteration(float * mem_sensor_array,
                                    int * mem_sensor_capdac_array, 
                                    int * mem_sensor_offset_array,
                                    uint8_t row_len, 
                                    uint8_t col_len
                                    ){
  uint32_t value;
  float capacitance;
  int capdac=0, offset=0;

  int row=0, col=0;
  
  uint8_t row_array[] = CAP_ROW_ARRAY;
  uint8_t column_array[] = CAP_COLUMN_ARRAY;
  
  // Initialization
  mem_clear_float(mem_sensor_array, row_len, col_len);

  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
  

  // capdac value is the same for all capacitors
  capdac = mem_get_int(mem_sensor_capdac_array, row, col, row_len, col_len);
  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

  // Sets all rows and columns to SHLD1
  for (row=0; row<row_len; row++){


      // Disconnects SHLD from current row
      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1, MUX_SWITCH_OFF, 0);
      // Sets the capacitive sensor connections to row
      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_ON, 0);
    
    for (col=0; col<col_len; col++){

      // Reading the corresponding CAPDAC value [NOT IMPLEMENTED]
//      capdac = mem_get_int(mem_sensor_capdac_array, row, col, row_len, col_len);
      offset = mem_get_int(mem_sensor_offset_array, row, col, row_len, col_len);
      cdc_set_offset(I2C_ADDR_CDC, CDC_CHANNEL_CIN1, (int16_t) offset);


//      cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);


//      Serial.print("Offset is ");Serial.println(offset, HEX);
//      Serial.print("Offset16 is ");Serial.println((int16_t)offset);


      // Disconnects SHLD from current pin
//      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1, MUX_SWITCH_OFF, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_SHLD1, MUX_SWITCH_OFF, 0);
      
      // Sets the capacitive sensor connections (row to CIN and col to GND)
//      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_ON, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_GND, MUX_SWITCH_ON, 0);
    
    
      // MEASURE
      cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEAS1, CDC_ENABLE);
      
      while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEAS1));

      value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEAS1);
//      Serial.println(value,HEX);
//      capacitance = cdc_convert_capacitance(value, capdac);
      capacitance = cdc_convert_capacitance_normalized(value);
            
      mem_store_float(mem_sensor_array, row, col, row_len, col_len, capacitance);


      // Disconnects SHLD from current pin
//      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_OFF, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_GND, MUX_SWITCH_OFF, 0);

      
      // Sets capacitive sensor connections back to shield
//      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1, MUX_SWITCH_ON, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_SHLD1, MUX_SWITCH_ON, 0);
      
      
//      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1);
//      cap_switch_column_signal(column_array[col], CAP_COL_SHLD1);
    }
    
      // Disconnects SHLD from current pin
      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_OFF, 0);
      // Sets capacitive sensor connections back to shield
      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1, MUX_SWITCH_ON, 0);
  }
  
  return mem_sensor_array;
}




// First calculates the best CAPDAC value, for position [0,0] , and then calculates and stores the offset for all positions
int cap_calibrate_sensors(float * mem_sensor_array,
                              int * mem_sensor_capdac_array, 
                              int * mem_sensor_offset_array,
                              uint8_t row_len, 
                              uint8_t col_len
                              ){
  uint32_t value;
  float capacitance;
  int capdac=0;
  int16_t offset=0;

  int row=5, col=5;
  
  uint8_t row_array[] = CAP_ROW_ARRAY;
  uint8_t column_array[] = CAP_COLUMN_ARRAY;
  
  // Initialization
  mem_clear_float(mem_sensor_array, row_len, col_len);
  mem_clear_int(mem_sensor_capdac_array, row_len, col_len, 0);
  mem_clear_int(mem_sensor_offset_array, row_len, col_len, 0);


  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
  
  // Estimates CAPDAC so that returned value is less than 1/2 LSB CAPDAC = 
  row=0;
  col=0;
  for (capdac=0; capdac<CDC_CAPDAC_MAX; capdac++){
    cdc_set_offset(I2C_ADDR_CDC, CDC_CHANNEL_CIN1, (int16_t) 0);

    capacitance = cap_get_measurement_single(row, col, capdac);
//   Serial.print("Capdac is estimated as ");Serial.println(capdac);
//   Serial.print("capacitance is ");Serial.println(capacitance);

    if (abs(capacitance)<= CDC_CAPDAC_HALF_LSB){
      break;
    }
  }
//  Serial.print("Capdac is estimated as ");Serial.println(capdac);
  // Stores common CAPDAC to memory
  mem_clear_int(mem_sensor_capdac_array, row_len, col_len, capdac);

  // Calculates and stores offset for all values

  
  // Sets all rows and columns to SHLD1
  for (row=0; row<row_len; row++){
    for (col=0; col<col_len; col++){

      // Reading the corresponding CAPDAC value 
      capdac = mem_get_int(mem_sensor_capdac_array, row, col, row_len, col_len);
      offset = mem_get_int(mem_sensor_offset_array, row, col, row_len, col_len);

      
      cdc_set_offset(I2C_ADDR_CDC, CDC_CHANNEL_CIN1, (int16_t) 0);

      cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

      // Disconnects SHLD from current pin
      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1, MUX_SWITCH_OFF, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_SHLD1, MUX_SWITCH_OFF, 0);
      
      // Sets the capacitive sensor connections (row to CIN and col to GND)
      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_ON, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_GND, MUX_SWITCH_ON, 0);
    
    
      // MEASURE
      cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEAS1, CDC_ENABLE);
      while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEAS1));
      value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEAS1);
      offset =  -value/65536;
  
//       Serial.print("[");Serial.print(row);Serial.print(", ");Serial.print(col);Serial.print("]   ");Serial.println(value, HEX);
//      Serial.print("offset/2**16 is ");Serial.println(value/65536, HEX);
//      Serial.print("Offset is ");Serial.println(offset);

      // offset is given by the read value, stores it to memory.
      mem_store_int(mem_sensor_offset_array, row, col, row_len, col_len, offset);

      capacitance = cdc_convert_capacitance_normalized(value);

      mem_store_float(mem_sensor_array, row, col, row_len, col_len, capacitance);
 

      // Disconnects SHLD from current pin
      cap_switch_row_signal(row_array[row], CAP_ROW_CIN1, MUX_SWITCH_OFF, 0);
      cap_switch_column_signal(column_array[col], CAP_COL_GND, MUX_SWITCH_OFF, 0);

      
      // Sets capacitive sensor connections back to shield
      cap_switch_row_signal(row_array[row], CAP_ROW_SHLD1);
      cap_switch_column_signal(column_array[col], CAP_COL_SHLD1);
    }
  }
  
  return 0;
}



#define SEND_DATA_LENGTH 2+2+CAP_MEM_SIZE*sizeof(float)
union dataPacket{
  struct {
    uint16_t command;
    uint16_t length;
    float data[CAP_MEM_SIZE];
    float extra[10];
    
  }packet;
  byte bytes[SEND_DATA_LENGTH];
};
  

int cap_send_sensor_array(float * mem_sensor_array,
                              uint8_t row_len, 
                              uint8_t col_len){
  
  union dataPacket send_data;
  int row=5, col=5;

  send_data.packet.command = 256*255;
  send_data.packet.length = row_len*col_len;
  for (row=0; row<row_len; row++){
    for (col=0; col<col_len; col++){
      mem_copy_float(mem_sensor_array, &send_data.packet.data[0], row_len, col_len);
    }
  }
  Serial.write(send_data.bytes, SEND_DATA_LENGTH);
}

int cap_print_sensor_array(float * mem_sensor_array,
                              uint8_t row_len, 
                              uint8_t col_len){
  
  mem_print_float(mem_sensor_array, row_len, col_len);
}
