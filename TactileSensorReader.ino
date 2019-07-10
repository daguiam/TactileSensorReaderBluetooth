/*
 * INL Capacitive Tactile Sensor Firmware
 * 
 * 
 */
#include <pthread.h>
#include <Arduino.h>
#include <Wire.h>
#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"
#include "MemStorage.h"
#include "parameters.h"
#include "sercomm.h"



#define led 13
#define led2 17

// Global variables

int capdac_value = 0;

// Each memory array is stored inside the FIFO

#define MEM_FIFO_SIZE 3
float * mem_sensor_fifo[MEM_FIFO_SIZE];
int mem_sensor_fifo_index = 0;
int mem_sensor_fifo_done_index = -1;

int mem_fifo_next_index(int mem_sensor_fifo_index){
  return (mem_sensor_fifo_index+1)%MEM_FIFO_SIZE;
}


float * mem_sensor_array;
int * mem_sensor_capdac_array;
int * mem_sensor_offset_array;
int row_len = CAP_ROW_ARRAY_LEN;
int col_len = CAP_COLUMN_ARRAY_LEN;

// Execution flags

//int flag_acq_start = 0;     // Flags to start the measurements
//int flag_acq_stop = 1;      // Flags to stop the measurements 
int flag_acq_running = 0;     // Flag that indicates measurements are being made

int flag_acq_done = 0;
int flag_acq_calibrate = 0; // Flags to calibrate the sensor



Order read_order()
{
  return (Order) Serial.read();
}


//TaskHandle_t Task_LoopMeasurements;



void write_text(char * text){
  Serial.write(text);
}
   


void *Thread_AcquireSensorData(void *threadid) {

  Serial.println("Started comm thread");

  for(;;){


    if(Serial.available() > 0){
        digitalWrite(led, HIGH);
  
      
      // The first byte received is the instruction
      Order order_received = read_order();
  //    Serial.println(order_received);
  
      switch(order_received){
        case START_ACQ:
        
          Serial.println("ACQ Start");
          flag_acq_running = 1;
          flag_acq_done = 0;
          
          break;
          
        case STOP_ACQ:
          Serial.println("ACQ Stop");
          flag_acq_running = 0;
          
          break;

          
        case DONE_ACQ:
          if (flag_acq_done){
            Serial.println("ACQ Done");
          }else{
            Serial.println("ACQ Not Done");
          }
          break;
  
        case READ_ACQ:
          if (!flag_acq_done){
            Serial.println("ACQ Not Done");
          }else{
            cap_print_sensor_array(mem_sensor_array, row_len, col_len);
//            cap_print_sensor_array(mem_sensor_fifo[mem_sensor_fifo_done_index], row_len, col_len);
            flag_acq_done = 0;
          }
          break;
        case READ_ACQ_BIN:
          cap_send_sensor_array(mem_sensor_array, row_len, col_len);
          flag_acq_done = 0;
          break;
  
        case CAL_SENSOR:
          flag_acq_calibrate = 1;
          
          
          break;

        case EOL:
          //ignore
          break;
        default:
          Serial.println("Unrecognized command");
          
          break;
  
      }
      digitalWrite(led, LOW);

    }
    vTaskDelay(10);
  }
   
}




  
void setup() {
  pthread_t threads[4];
  int returnValue;
  int i;
  // Serial for debugging connection
//    Serial.begin(9600);




  Serial.begin(SERIAL_BAUD);

  Serial.println("Started");
  randomSeed(analogRead(0));

  
  // Initializing wire connection
  Wire.begin();
  // set clock to 400 kHz
  Wire.setClock(I2C_CLOCK);



  // Initializing IO
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);


  // Checking connectivity and operation of ADG2128

  // Initializing ADG2128 MUX
  pinMode(IO_MUX_RESET_N, OUTPUT);

  mux_reset();
  digitalWrite(IO_MUX_RESET_N,HIGH);

  
  // Checking operation of MUX  
  if (!mux_test_operation(I2C_ADDR_MUX2)){
    Serial.print("MUX2 not working properly \n");
  }
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

  cdc_set_rate(I2C_ADDR_CDC, CDC_RATE_400SPS);
  
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_ENABLE);

  
  // Reading configurations of all measurements
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS2);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS3);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS4);


//  cdc_print_configuration(I2C_ADDR_CDC);

  cap_switch_all_rows_signal(CAP_ROW_SHLD1);

//
//  
//  // Initalize memory
  for (i=0; i<MEM_FIFO_SIZE; i++){
    mem_sensor_fifo[i] = mem_init_float(row_len, col_len);
  }
  
//  mem_sensor_array = mem_init_float(row_len, col_len);

  mem_sensor_array = mem_sensor_fifo[mem_sensor_fifo_index];
  
  mem_sensor_capdac_array = mem_init_int(row_len, col_len, 0);
  mem_sensor_offset_array = mem_init_int(row_len, col_len);




//  Serial.println("Clearing cap rows and columns");
  cap_switch_clear_all_rows();
  cap_switch_clear_all_columns();
//  Serial.println("Setting all rows and columns to shield");
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);

  cap_calibrate_sensors(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);



  returnValue = pthread_create(&threads[0], NULL, Thread_AcquireSensorData, (void *)0);
  

}







void loop() {
  int i = 0;
  int j = 0;
  int row, col;
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
  int capdac;

  Order order;


//  Serial.println("running");

  if (flag_acq_calibrate==1){
    cap_switch_all_rows_signal(CAP_ROW_SHLD1);
    cap_switch_all_columns_signal(CAP_COL_SHLD1); 
    cap_calibrate_sensors(mem_sensor_fifo[mem_sensor_fifo_index], mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
    flag_acq_calibrate = 0;
  }

// flag_acq_running=1;
  if (flag_acq_running==1){
  
    digitalWrite(led2, HIGH);
    cap_get_measurement_iteration(mem_sensor_fifo[mem_sensor_fifo_index], mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
    digitalWrite(led2, LOW);
    flag_acq_done = 1;

    mem_sensor_fifo_done_index = mem_sensor_fifo_index ;
    mem_sensor_array = mem_sensor_fifo[mem_sensor_fifo_index];
    mem_sensor_fifo_index = mem_fifo_next_index(mem_sensor_fifo_index);
//    Serial.println(mem_sensor_fifo_index);
    

  }

//
//  digitalWrite(led2, HIGH);
//  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//  digitalWrite(led2, LOW);




  // Verifies if there is a message on the serial port
  
//  
//  if(Serial.available() > 0){
//    
//    
//    // The first byte received is the instruction
//    Order order_received = read_order();
////    Serial.println(order_received);
//
//    switch(order_received){
//      case START_ACQ:
//        Serial.println("ACQ Start");
//        
//        break;
//      case STOP_ACQ:
//        Serial.println("ACQ Stop");
//        
//        break;
//
//      case READ_ACQ:
////        Serial.println("READ Acq");
////        digitalWrite(led2, HIGH);
////        cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
////        cap_send_sensor_array(mem_sensor_array, row_len, col_len);
//        cap_print_sensor_array(mem_sensor_array, row_len, col_len);
////        digitalWrite(led2, LOW);
//        break;
//      case READ_ACQ_BIN:
////        Serial.println("READ Acq");
////        digitalWrite(led2, HIGH);
////        cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//        cap_send_sensor_array(mem_sensor_array, row_len, col_len);
////        cap_print_sensor_array(mem_sensor_array, row_len, col_len);
////        digitalWrite(led2, LOW);
//        break;
//
//      case CAL_SENSOR:
////        Serial.println("CAL Sensor");
//        //  Serial.println("Setting all rows and columns to shield");
//        cap_switch_all_rows_signal(CAP_ROW_SHLD1);
//        cap_switch_all_columns_signal(CAP_COL_SHLD1);
//
//        cap_calibrate_sensors(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//
//        break;
//      default:
//        Serial.println("Unrecognized command");
//        
//        break;
//
//    }
//  }



//  digitalWrite(led,HIGH);
  
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);


  // Configure the measurements
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

//  digitalWrite(led2, HIGH);
////  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
////  cap_send_sensor_array(mem_sensor_array, row_len, col_len);
//  digitalWrite(led2, LOW);



  // Clear the memory 0    


//
//  cap_calibrate_sensors(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//
//  Serial.println("-capdac-");
//  mem_print_int(mem_sensor_capdac_array, row_len, col_len);
//  Serial.println("-offset-");
//  mem_print_int(mem_sensor_offset_array, row_len, col_len);
//  Serial.println("-sensor of cal-");
//  mem_print_float(mem_sensor_array, row_len, col_len);
//  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//  Serial.println("-sensor-");
//  mem_print_float(mem_sensor_array, row_len, col_len);
//  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
//  
//  
//  Serial.println("-sensor-");
//  mem_print_float(mem_sensor_array, row_len, col_len);
//
//    mem_clear_int(mem_sensor_offset_array, row_len, col_len, 0);

//  Serial.println("-sensor-");
//  mem_print_float(mem_sensor_array, row_len, col_len);



//  for (row=0; row<row_len; row++){
//    for (col=0; col<col_len; col++){
//      capacitance = cap_get_measurement_single(row, col, capdac);
//      mem_store_float(mem_sensor_array, row, col, row_len, col_len, capacitance);
//    }
//
//  }

//
//  for (capdac=0; capdac<32; capdac++){
//    for (row=0; row<row_len; row++){
//      for (col=0; col<col_len; col++){
//        capacitance = cap_get_measurement_single(row, col, capdac);
//  
//          mem_store_float(mem_sensor_array, row, col, row_len, col_len, capacitance);
//      }
//  //    Serial.println();
//    }
//    
//    Serial.println("-sensor-");
//    Serial.println(capdac);
//    mem_print_float(mem_sensor_array, row_len, col_len);
//  }

  

//  delay(500);
//  Serial.println("-capdac-");
//  mem_print_int(mem_sensor_capdac_array, row_len, col_len);
//  Serial.println("-offset-");
//  mem_print_int(mem_sensor_offset_array, row_len, col_len);
//  Serial.println("-sensor-");
//  mem_print_float(mem_sensor_array, row_len, col_len);


//  digitalWrite(led,LOW);

  
//
//
//  cap_set_sensor_measurement_single(5,5);
//  cap_print_connections();

  



}
