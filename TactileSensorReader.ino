/*
 * INL Capacitive Tactile Sensor Firmware
 * 
 * 
 */

#include <Wire.h>
#include "ADG2128.h"
#include "FDC1004.h"


#define led 13
#define led2 17


#define I2C_ADDR_MUX1 0b01110001
#define I2C_ADDR_MUX2 0b01110011
#define I2C_ADDR_CDC  0b01010000


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
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_ENABLE);

  
  // Reading configurations of all measurements
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS2, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS3, 1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS4, 1);
  
}
int capdac_value = 0;

void loop() {
  int i = 0;
  int j = 0;
  char addr = 0;
  char rb_addr = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  char status = 0;
  uint16_t aux = 0;
  int measurement = 0;
  int value = 0;
  float capacitance = 0;

  
  digitalWrite(led,HIGH);

  digitalWrite(led,LOW);
  digitalWrite(led2,LOW);





//  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, 1);
//  cdc_print_configuration(I2C_ADDR_CDC);

 

//CDC_CHANNEL_DISABLED
//CDC_CHANNEL_CAPDAC


  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac_value);

  // Wait for complete measurement
  while(!cdc_get_measurement_completed(I2C_ADDR_CDC,CDC_MEASUREMENT));
  value = cdc_get_measurement(I2C_ADDR_CDC, CDC_MEASUREMENT);
  capacitance = cdc_convert_capacitance(value, capdac_value);

  // Check for saturation of capacitance

    
  if (!cdc_measurement_saturated(value)){
    Serial.print("Measurement ");Serial.print(CDC_MEASUREMENT); Serial.print(" CAP ");Serial.print(capdac_value);
    Serial.print(" : ");Serial.print(value);Serial.print("   \t= ");Serial.print(capacitance,4);Serial.println(" pF");
  }else{
    Serial.print("Measurement ");Serial.print(CDC_MEASUREMENT); Serial.print(" CAP Saturated");Serial.print(capdac_value); Serial.println(" Saturated ");
  }

  capdac_value = (capdac_value+1)%12;
  capdac_value = 4;


}
