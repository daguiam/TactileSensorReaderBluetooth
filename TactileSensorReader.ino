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



  
}

void loop() {
  int i = 0;
  int j = 0;
  char addr = 0;
  char rb_addr = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  char status = 0;
  
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


  char switch_info[8] = {1,0,1,0,1,0,1,1};


  
}
