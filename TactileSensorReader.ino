/*
 * INL Capacitive Tactile Sensor Firmware
 * 
 * 
 */

#include <Wire.h>
#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"


#define led 13
#define led2 17


void write_text(char * text){
  Serial.write(text);
}

  
void setup() {



  // Serial for debugging connection
  Serial.begin(9600);
  randomSeed(analogRead(0));

  
  // Initializing wire connection
  Wire.begin();

  // Initializing IO
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);


  // Checking connectivity and operation of ADG2128

  // Initializing ADG2128 MUX
  pinMode(IO_MUX_RESET_N, OUTPUT);
  digitalWrite(IO_MUX_RESET_N,HIGH);
  
  // Checking operation of MUX  
  if (!mux_test_operation(I2C_ADDR_MUX1)){
    Serial.print("MUX1 not working properly \n");
  }
  if (!mux_test_operation(I2C_ADDR_MUX2)){
    Serial.print("MUX2 not working properly \n");
  }


  
  mux_reset();
  //  Connect CIN1 on MUX1 Y5 to X0
  mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(0), mux_get_addr_y(5), MUX_SWITCH_ON, MUX_LDSW_LOAD );

  // Shortcircuit Y5 to Y4?
  mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(0), mux_get_addr_y(4), MUX_SWITCH_OFF, MUX_LDSW_LOAD );
//  mux_read_config_matrix(I2C_ADDR_MUX1);

  // Printing switch info of MUX 1 and 2
  Serial.print("MUX1\n");
  mux_read_config_matrix(I2C_ADDR_MUX1);
  Serial.print("MUX2\n");
  mux_read_config_matrix(I2C_ADDR_MUX2);


  // Resetting FDC1004

  cdc_reset_device(I2C_ADDR_CDC);
  // Checking connectivity and operation of FDC1004
  if (!cdc_test_id(I2C_ADDR_CDC)){
    Serial.print("CDC not working properly \n");
  }

 #define CDC_MEASUREMENT CDC_MEAS1

  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, 0);
  cdc_set_repeat_measurements(I2C_ADDR_CDC, CDC_ENABLE);
  cdc_set_repeat_measurements(I2C_ADDR_CDC, CDC_DISABLE);
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_ENABLE);

  
  // Reading configurations of all measurements
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS2, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS3, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS4, 1);



  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  
}
int capdac_value = 0;

void loop() {
  int i = 0;
  int j = 0;
  char addr = 0;
  char rb_addr = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;

  uint8_t row_array[] = CAP_ROW_ARRAY;
  uint8_t column_array[] = CAP_COLUMN_ARRAY;

  
  char status = 0;
  uint16_t aux = 0;
  int measurement = 0;
  int value = 0;
  float capacitance = 0;
  int capdac = 3;

  
  digitalWrite(led,HIGH);

  digitalWrite(led,LOW);
  digitalWrite(led2,LOW);





//  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, 1);
//  cdc_print_configuration(I2C_ADDR_CDC);

 

//CDC_CHANNEL_DISABLED
//CDC_CHANNEL_CAPDAC

//
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac_value);
//
//  // Wait for complete measurement
//  while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEASUREMENT));
//  value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEASUREMENT);
//  capacitance = cdc_convert_capacitance(value, capdac_value);
//
//  // Check for saturation of capacitance
//
//    
//  if (!cdc_measurement_saturated(value)){
//    Serial.print("Measurement ");Serial.print(CDC_MEASUREMENT); Serial.print(" CAP ");Serial.print(capdac_value);
//    Serial.print(" : ");Serial.print(value);Serial.print("   \t= ");Serial.print(capacitance,4);Serial.println(" pF");
//  }else{
//    Serial.print("Measurement ");Serial.print(CDC_MEASUREMENT); Serial.print(" CAP Saturated");Serial.print(capdac_value); Serial.println(" Saturated ");
//  }
//
//  capdac_value = (capdac_value+1)%12;
//  capdac_value = 4;



  Serial.println("Clearing cap rows and columns");
  cap_switch_clear_all_rows();
  cap_switch_clear_all_columns();
//  Serial.println("MUX1");
//  mux_read_config_matrix(I2C_ADDR_MUX1);
//  Serial.println("MUX2");
//  mux_read_config_matrix(I2C_ADDR_MUX2);
//
//

  Serial.println("Setting all rows and columns to shield");
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
  Serial.println("Settings row R01 to CIN1 and columns to GND");
  cap_switch_row_signal(CAP_R01, CAP_ROW_CIN1);
  cap_switch_column_signal(CAP_C01, CAP_COL_GND);

  
  digitalWrite(led,HIGH);
  capdac = 10;
  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

  for (j=0; j<CAP_COLUMN_ARRAY_LEN; j++){
    for (i=0; i<CAP_ROW_ARRAY_LEN; i++){
//      cap_set_sensor_measurement_single(i, j);
      digitalWrite(led2,HIGH);
      capacitance = cap_get_measurement_single(i, j, capdac);
      digitalWrite(led2,LOW);
      Serial.print("C_");Serial.print(i); Serial.print("_");Serial.print(j);
       Serial.print("   \t:\t");Serial.print(capacitance,4);Serial.println(" pF");
    }
  }
  digitalWrite(led,LOW);
  
//
//
//  cap_set_sensor_measurement_single(5,5);
//  cap_print_connections();

  Serial.println("|||");
  Serial.println("-+-");
  Serial.println("|||");







}
