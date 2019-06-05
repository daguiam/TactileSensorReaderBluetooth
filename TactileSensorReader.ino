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
  // Printing switch info of MUX 1 and 2
  Serial.print("MUX1\n");
  mux_read_config_matrix(I2C_ADDR_MUX1);
  Serial.print("MUX2\n");
  mux_read_config_matrix(I2C_ADDR_MUX2);

  // Checking connectivity and operation of FDC1004
  if (!cdc_test_id(I2C_ADDR_CDC)){
    Serial.print("CDC not working properly \n");
  }



  
}

void loop() {
  int i = 0;
  int j = 0;
  char addr = 0;
  char rb_addr = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  char status = 0;
  uint16_t aux = 0;
  
  digitalWrite(led,HIGH);
  digitalWrite(led2,HIGH);
  digitalWrite(IO_MUX_RESET_N,LOW);
  delay(100);
  digitalWrite(IO_MUX_RESET_N,HIGH);

  
  delay(200);
  digitalWrite(led,LOW);
  digitalWrite(led2,LOW);
  delay(1000);
  write_text("text\n");


  aux = cdc_read_register(I2C_ADDR_CDC, CDC_MANUFACTURER_ID);
  Serial.println("FDC exists?");

  Serial.println(cdc_test_id(I2C_ADDR_CDC));



  
}
