#include <Arduino.h>
#include <Wire.h>

#include "ADG2128.h"
//void write_text(char * text){
//  Serial.write(text);
//}






char mux_get_addr_x(uint8_t x){
  // ADG2128 has reserved codes for addressing the X lines... check datasheet page 21
  if (x<6){
    return x;
  }else{
    return x+2;
  }
}

char mux_get_addr_y(uint8_t y){
  return y;
}






char mux_write_switch_config(uint8_t addr, uint8_t addr_x, uint8_t addr_y, uint8_t data, uint8_t ldsw){
  // For arduino, bitfield order must be reversed
  struct Package{
    uint8_t ldsw : 1;
    uint8_t undefined : 7;
    uint8_t addr_y : 3;
    uint8_t addr_x : 4;
    uint8_t data : 1;

  };

  char status = 0xaa;
  struct Package package;
  char * package_char = (char*)&package;

  package.addr_x = addr_x;
  package.addr_y = addr_y;
  package.data = data;
  package.ldsw = ldsw;

  Wire.beginTransmission(addr);
  Wire.write(package_char[1]);
  Wire.write(package_char[0]); 
  status = 0xff;
  status = Wire.endTransmission(); 
  
  if (status){
    Serial.print("Wire Transmission error: "); Serial.println(status,DEC);
  }
  return status;
}

char mux_read_config(uint8_t addr, uint8_t read_addr_X, uint8_t verbose){
//  verbose = 0;
  char status = 0xaa;
  // Start by writing to MUX register to request the Y lines connected to the expected X data
  Wire.beginTransmission(addr);

  Wire.write(read_addr_X);
  Wire.write(0xAA); 
  status = 0xff;

  status = Wire.endTransmission(); 
  if (status){
    Serial.print("Wire Transmission error: "); Serial.println(status,DEC);
  }
  if (verbose){
    Serial.print("status of transmission: ");
    Serial.print(status,BIN);
    Serial.print("\n");
  }
  // Reading from slave device
  Wire.requestFrom(addr, 2);

  // waiting for slave to reply
//  Serial.print("Waiting for reply from slave \n");
  while (Wire.available()<2){
    status = Wire.available();
    
  }

  if (verbose){
    Serial.print("Finished waiting for reply from slave \n");
    Serial.print(status, DEC);
    Serial.print("\n");
  }

  
  status = Wire.read(); // First byte out is 0s
  if (verbose){
    Serial.print(status,BIN);
    Serial.print("\n");
  }
  status = Wire.read(); // Second byte is the status
  
  if (verbose){
    Serial.print(status,BIN);
    Serial.print("\n");
  }
  return status;

}




bool mux_test_operation(uint8_t addr, bool verbose){
  // Writes random data to X-Y pair and reads the configuration
  // If the values read match the written, it returns True, otherwise False.
  // This overwrites the existing configuration!

  
  char switch_info[8] = {1,0,1,0,1,0,1,1};
  int i = 0;
  int j = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  char status = 0;
  char random_value = random(256);
  char random_bit = 0;

  for (j=0; j<12; j++){
    random_value = random(256);
    
    for (i=0; i<8; i++){
      random_bit = random_value>>i & 0x1;
      
      status = mux_write_switch_config(addr, mux_get_addr_x(j), mux_get_addr_y(i), random_bit, MUX_LDSW_LOAD );
    }
    status = mux_read_config(addr, rb_addr_array[j]);
    
    if(verbose){
      Serial.print("Reg data: ");
      Serial.print(random_value, BIN);
      Serial.print("\n");
      Serial.print("Status of X");
      Serial.print(j,DEC);
      Serial.print(": ");
      Serial.print(status,BIN);
      Serial.print("\n");
    }
    if (status != random_value){
      return 0;
    }
  }
  return 1;
}



bool mux_read_config_matrix(uint8_t addr){
  // Reads the switch configuration matrix to the serial port
  int i = 0;
  int j = 0;
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  char status = 0;

//  Serial.print("\tY7\tY6\tY5\tY4\tY3\tY2\tY1\tY0\n");
  Serial.print("\tY0\tY1\tY2\tY3\tY4\tY5\tY6\tY7\n");
  for (j=0; j<12; j++){


    status = mux_read_config(addr, rb_addr_array[j]);
    Serial.print("X");
    Serial.print(j,DEC);
    Serial.print(":\t");
    for (i=0; i<8; i++){
      Serial.print(status>>i & 0x1,BIN);  
      Serial.print("\t");  
    }
    Serial.print("\n");
  }
  return 1;
}


uint8_t mux_read_x_connections(uint8_t addr, uint8_t x){
  
  char rb_addr_array[] = MUX_READ_X_ARRAY;
  return mux_read_config(addr, rb_addr_array[x]);
  
}


char mux_clear_x(uint8_t addr, uint8_t x ){
  int i = 0;
  for (i=0; i<MUX_READ_Y_ARRAY_LEN; i++){
    mux_write_switch_config(addr, mux_get_addr_x(x), mux_get_addr_y(i), MUX_SWITCH_OFF, MUX_LDSW_LOAD );
  }
}


char mux_clear_all(uint8_t addr){
  int j = 0;
  for (j=0; j<MUX_READ_X_ARRAY_LEN; j++){
   mux_clear_x(addr, j);
  }
}


char mux_reset(){
  
  digitalWrite(IO_MUX_RESET_N,LOW);
  delay(10);
  digitalWrite(IO_MUX_RESET_N,HIGH);
}
