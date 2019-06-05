#include <Arduino.h>
#include <Wire.h>


#include "FDC1004.h"



#define CDC_REGISTER_READ_SIZE 2
uint16_t cdc_read_register(uint8_t addr, uint8_t pointer, uint8_t verbose){

  char status = 0xaa;
  uint16_t ret = 0;

  // Sends 2 frame write to set the register pointer to the slave
  Wire.beginTransmission(addr);
  Wire.write(pointer);
  status = Wire.endTransmission(); 

  // Reading from slave device
  Wire.requestFrom(addr, CDC_REGISTER_READ_SIZE);
  // waiting for slave to reply
  while (Wire.available()<CDC_REGISTER_READ_SIZE){
    status = Wire.available();
  }
  status = Wire.read();
  // shifts the read byte onto ret
  ret = (status&0xFF) <<8;
  status = Wire.read(); // Second byte is the status
  ret = ret | (status&0xFF);

  if (verbose){
    Serial.print(ret,HEX);
    Serial.print("\n");
  }
  return ret;

}




char cdc_write_register(uint8_t addr, uint8_t addr_x, uint16_t data){
  
  union Package{
    uint16_t data;
    char bytes[3];
  };

  char status = 0xaa;
  union Package package;
  package.data = data;


  Wire.beginTransmission(addr);
  Wire.write(package.bytes[1]);
  Wire.write(package.bytes[0]); 
  status = Wire.endTransmission(); 
  return status;
}






bool cdc_test_id(uint8_t addr){
  // Reads the Manufacturrer ID and Device ID of a FDC1004 device on the i2c bus
  uint16_t manufacturer_id = cdc_read_register(addr, CDC_MANUFACTURER_ID);
  uint16_t device_id = cdc_read_register(addr, CDC_DEVICE_ID);

  return (manufacturer_id==CDC_MANUFACTURER_ID_RESPONSE && device_id == CDC_DEVICE_ID_RESPONSE);
  

}
