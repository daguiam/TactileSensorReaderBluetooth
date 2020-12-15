#include <Arduino.h>
#include <Wire.h>



#include "FDC1004.h"



union Package_FDC_CONF{
  struct {
    uint16_t done_4 : 1;
    uint16_t done_3 : 1;
    uint16_t done_2 : 1;
    uint16_t done_1 : 1;
    uint16_t meas_4 : 1;
    uint16_t meas_3 : 1;
    uint16_t meas_2 : 1;
    uint16_t meas_1 : 1;
    uint16_t repeat : 1;
    uint16_t reserved : 1;
    uint16_t rate : 2;
    uint16_t reserved2 : 3;
    uint16_t reset: 1;
  } elements;
  byte bytes[2];
  uint16_t packet;
};




#define CDC_REGISTER_READ_SIZE 2
#define CDC_REGISTER_WRITE_SIZE 3

uint16_t cdc_read_register(uint8_t addr, uint8_t pointer, uint8_t verbose){

  char status = 0xaa;
  uint16_t ret = 0;

  // Sends 2 frame write to set the register pointer to the slave
  Wire.beginTransmission(addr);
  Wire.write(pointer);
  status = 0xff;
  status = Wire.endTransmission(1); 
  
  if (status){
    Serial.print("Wire Transmission error: "); Serial.println(status,DEC);
  }
  

  // Reading from slave device
  Wire.requestFrom(addr, CDC_REGISTER_READ_SIZE);
  // waiting for slave to reply
  while (Wire.available()<CDC_REGISTER_READ_SIZE){
//    Serial.println(Wire.available(), HEX);delay(100);
    status = Wire.available();
  }
  status = Wire.read();
  // shifts the read byte onto ret
  ret = (status&0xFF) <<8;
  status = Wire.read(); // Second byte is the status
  ret = ret | (status&0xFF);

  if (verbose){
    Serial.print("Read packet: ");Serial.print(pointer,HEX);Serial.print(" ");Serial.println(ret,HEX);
  }
  return ret;

}




char cdc_write_register(uint8_t addr, uint8_t pointer, uint16_t data, uint8_t verbose){
  
  union Package{
    struct {
      uint16_t data;
      uint8_t pointer;
    } elements;
    char bytes[CDC_REGISTER_WRITE_SIZE];
  };

  char status = 0xaa;
  union Package package;
  package.elements.data = data;
  package.elements.pointer = pointer;


  Wire.beginTransmission(addr);
  Wire.write(package.bytes[2]);
  Wire.write(package.bytes[1]);
  Wire.write(package.bytes[0]); 
  status = Wire.endTransmission(); 


  

  if(verbose){
    Serial.print("Written packet: "); Serial.print(package.bytes[2],HEX);Serial.print(" ");
    Serial.print(package.bytes[1],HEX);Serial.print(" ");Serial.println(package.bytes[0],HEX);
  }
  return status;
}






bool cdc_test_id(uint8_t addr){
  // Reads the Manufacturrer ID and Device ID of a FDC1004 device on the i2c bus
  uint16_t manufacturer_id = cdc_read_register(addr, CDC_MANUFACTURER_ID, 1);
  uint16_t device_id = cdc_read_register(addr, CDC_DEVICE_ID, 1);

  return (manufacturer_id==CDC_MANUFACTURER_ID_RESPONSE && device_id == CDC_DEVICE_ID_RESPONSE);
  

}







union Package_FDC_CONF cdc_read_configuration(uint8_t addr){
  union Package_FDC_CONF fdc_configuration;
  fdc_configuration.packet =  cdc_read_register(addr, CDC_FDC_CONF);
  return fdc_configuration;
}
char cdc_print_configuration(uint8_t addr){
  union Package_FDC_CONF fdc_configuration = cdc_read_configuration(addr);
//  fdc_configuration.packet =  cdc_read_register(addr, CDC_FDC_CONF);
  Serial.println("-- FDC Configuration -- "); 
  Serial.print("Reset: "); Serial.println(fdc_configuration.elements.reset);
  Serial.print("Rate: "); Serial.println(fdc_configuration.elements.rate);
  Serial.print("Repeat: "); Serial.println(fdc_configuration.elements.repeat);
  Serial.print("Meas_1: "); Serial.println(fdc_configuration.elements.meas_1);
  Serial.print("Meas_2: "); Serial.println(fdc_configuration.elements.meas_2);
  Serial.print("Meas_3: "); Serial.println(fdc_configuration.elements.meas_3);
  Serial.print("Meas_4: "); Serial.println(fdc_configuration.elements.meas_4);
  Serial.print("Done_1: "); Serial.println(fdc_configuration.elements.done_1);
  Serial.print("Done_2: "); Serial.println(fdc_configuration.elements.done_2);
  Serial.print("Done_3: "); Serial.println(fdc_configuration.elements.done_3);
  Serial.print("Done_4: "); Serial.println(fdc_configuration.elements.done_4);
  Serial.print("packet: "); Serial.println(fdc_configuration.packet, HEX);
  Serial.println("-- End FDC Configuration -- "); 
}


char cdc_reset_device(uint8_t addr){
  union Package_FDC_CONF fdc_configuration;
  fdc_configuration.elements.reset = 1;
  return cdc_write_register(addr, CDC_FDC_CONF, fdc_configuration.packet);
  
}

char cdc_set_repeat_measurements(uint8_t addr, uint8_t repeat){
  union Package_FDC_CONF fdc_configuration = cdc_read_configuration(addr);
  fdc_configuration.elements.repeat = repeat;
  return cdc_write_register(addr, CDC_FDC_CONF, fdc_configuration.packet);
}


char cdc_set_rate(uint8_t addr, uint8_t rate){
  union Package_FDC_CONF fdc_configuration = cdc_read_configuration(addr);
  fdc_configuration.elements.rate = rate;
  return cdc_write_register(addr, CDC_FDC_CONF, fdc_configuration.packet);
}


char cdc_set_offset(uint8_t addr, uint8_t channel, int16_t offset){
  uint8_t offset_addr_array[] = CDC_OFFSET_CAL_ARRAY;
  uint8_t offset_addr = offset_addr_array[channel];
//  Serial.print("Offset is ");Serial.println(offset, HEX);

  return cdc_write_register(addr, offset_addr, offset);
}




char cdc_set_measurement_enable(uint8_t addr, uint8_t measurement, uint8_t enable){
  union Package_FDC_CONF fdc_configuration = cdc_read_configuration(addr);
  switch(measurement){
    case CDC_MEAS1:
        fdc_configuration.elements.meas_1 = enable;
      break;
    case CDC_MEAS2:
        fdc_configuration.elements.meas_2 = enable;
      break;
    case CDC_MEAS3:
        fdc_configuration.elements.meas_3 = enable;
      break;
    case CDC_MEAS4:
        fdc_configuration.elements.meas_4 = enable;
      break;
    default:
      Serial.println("CDC_Measurement_Enable error: non-existent measurement value");
  }
  return cdc_write_register(addr, CDC_FDC_CONF, fdc_configuration.packet);
}


int cdc_get_measurement_completed(uint8_t addr, uint8_t measurement){
  union Package_FDC_CONF fdc_configuration = cdc_read_configuration(addr);
  uint8_t completed;
  switch(measurement){
    case CDC_MEAS1:
        completed = fdc_configuration.elements.done_1;
      break;
    case CDC_MEAS2:
        completed = fdc_configuration.elements.done_2;
      break;
    case CDC_MEAS3:
        completed = fdc_configuration.elements.done_3;
      break;
    case CDC_MEAS4:
        completed = fdc_configuration.elements.done_4;
      break;
    default:
      Serial.println("CDC_Measurement_completed error: non-existent measurement");
      completed = -1;
  }
  return completed;
}


// keep individual uint32_t to larger size than whole bitfield struct
union Package_MEAS_CONF{
  struct {
    uint32_t reserved : 5;
    uint32_t capdac : 5;
    uint32_t chb : 3;
    uint32_t cha : 3;
  } elements;
  byte bytes[2];
  uint16_t packet;
};





char cdc_set_measurement_configuration(uint8_t addr, uint8_t measurement, uint8_t input_cha, uint8_t input_chb, uint8_t capdac, uint8_t verbose){
  uint8_t measurement_register = 0;
  
  union Package_MEAS_CONF meas_configuration;
  meas_configuration.elements.cha = input_cha;
  meas_configuration.elements.chb = input_chb;
  meas_configuration.elements.capdac = capdac;
  meas_configuration.elements.reserved = 0;
//  meas_configuration.packet = 0x1C00;

  if (verbose){
    Serial.print("1CHA: "); Serial.println(meas_configuration.elements.cha,DEC);
    Serial.print("1CHB: "); Serial.println(meas_configuration.elements.chb,DEC);
    Serial.print("1CAPDAC: "); Serial.println(meas_configuration.elements.capdac,DEC);
    Serial.print("reserved: "); Serial.println(meas_configuration.elements.reserved,DEC);
    Serial.print("packet: "); Serial.println(meas_configuration.packet,HEX);
  }
  switch(measurement){
    case CDC_MEAS1:
        measurement_register = CDC_CONF_MEAS1;
      break;
    case CDC_MEAS2:
        measurement_register = CDC_CONF_MEAS2;
      break;
    case CDC_MEAS3:
        measurement_register = CDC_CONF_MEAS3;
      break;
    case CDC_MEAS4:
        measurement_register = CDC_CONF_MEAS4;
      break;
    default:
      Serial.println("cdc_set_measurement_configuration error: non-existent measurement");
      
  }
  
  return cdc_write_register(addr, measurement_register, meas_configuration.packet);
}



uint16_t cdc_get_measurement_configuration(uint8_t addr, uint8_t measurement, char verbose){
  union Package_MEAS_CONF meas_configuration;
  switch(measurement){
    case CDC_MEAS1:
        meas_configuration.packet = cdc_read_register(addr, CDC_CONF_MEAS1);
      break;
    case CDC_MEAS2:
        meas_configuration.packet = cdc_read_register(addr,CDC_CONF_MEAS2);
      break;
    case CDC_MEAS3:
        meas_configuration.packet = cdc_read_register(addr,CDC_CONF_MEAS3);
      break;
    case CDC_MEAS4:
        meas_configuration.packet = cdc_read_register(addr,CDC_CONF_MEAS4);
      break;
    default:
      Serial.println("cdc_get_measurement_configuration error: non-existent measurement");
  }

  if(verbose){
    Serial.println("-- Measurement Configuration -- "); 
    Serial.print("Measurement: "); Serial.println(measurement,DEC);
    Serial.print("CHA: "); Serial.println(meas_configuration.elements.cha,DEC);
    Serial.print("CHB: "); Serial.println(meas_configuration.elements.chb,DEC);
    Serial.print("CAPDAC: "); Serial.println(meas_configuration.elements.capdac,DEC);
    Serial.print("packet: "); Serial.println(meas_configuration.packet,HEX);
    Serial.println("-- End of Measurement Configuration -- "); 
  }
  return meas_configuration.packet;
}


int32_t cdc_get_measurement(uint8_t addr, uint8_t measurement){
  // reads the correspondings measurement MSB and LSB registers and builds the measurement value
  uint32_t output = 0;
  uint16_t aux = 0;
  uint8_t msb_register = 0;
  uint8_t lsb_register = 0;
  
  switch(measurement){
    case CDC_MEAS1:
        msb_register = CDC_MEAS1_MSB;
        lsb_register = CDC_MEAS1_LSB;
      break;
    case CDC_MEAS2:
        msb_register = CDC_MEAS2_MSB;
        lsb_register = CDC_MEAS2_LSB;
      break;
    case CDC_MEAS3:
        msb_register = CDC_MEAS3_MSB;
        lsb_register = CDC_MEAS3_LSB;
      break;
    case CDC_MEAS4:
        msb_register = CDC_MEAS4_MSB;
        lsb_register = CDC_MEAS4_LSB;
    default:
      Serial.println("cdc_get_measurement error: non-existent measurement");
      
  }
  // TODO: VERIFY I2C AUTO-INCREMENT check page 16 FDC1004 datasheet. To read MSB then LSB.
  // Eliminate aux read, why is this here???
  aux = cdc_read_register(addr, msb_register); // TODO: OPTIMIZE why aux?? eliminate
  output = cdc_read_register(addr, msb_register) << 16;
  aux = cdc_read_register(addr, lsb_register);
  output = output | cdc_read_register(addr, lsb_register);
  return output;

}


float cdc_convert_capacitance(int32_t value, int32_t capdac){
  float output = (float) value/134217728.0 + capdac*3.125;
  return output;
}


float cdc_convert_capacitance_normalized(int32_t value){
  float output = (float) value/134217728.0 ;
//  output = (float) value;
  return output;
}


int cdc_measurement_saturated(int32_t value){
  if (value >= 2147483391 || value <= -2147483392){
    return 1;
  }else{
    return 0;
  }
}
