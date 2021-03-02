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


//
//#define led 13
//#define led2 17


#define D1 13
#define D2 12
#define D3 27
#define D5 4
#define D6 25
#define D7 26

#define led1 D7
#define led2 D6
#define led3 D1
#define led4 D2
#define led5 D3
#define led6 D5


#define led led1

#define VERBOSE 0

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
        digitalWrite(led1, HIGH);
  
      
      // The first byte received is the instruction
      Order order_received = read_order();
//      Serial.print("Order: ");Serial.println(order_received);
  
      switch(order_received){
        case START_ACQ:
          Serial.print((char)START_ACQ);
          Serial.println("ACQ Start");
          flag_acq_running = 1;
          flag_acq_done = 0;

          digitalWrite(led6, HIGH);

          break;
          
        case STOP_ACQ:
          Serial.print((char)STOP_ACQ);
          Serial.println("ACQ Stop");
          flag_acq_running = 0;
          
          break;

          
        case DONE_ACQ:
          Serial.print((char)DONE_ACQ);
          if (flag_acq_done){
            Serial.print((char)TRUE);
            Serial.println("ACQ Done");
          }else{
            Serial.print((char)FALSE);
            Serial.println("ACQ Not Done");
          }
          break;
  
        case READ_ACQ:
          if (!flag_acq_running){
            Serial.print((char)ERROR);
            Serial.println("ACQ Not Started");
          }else if (!flag_acq_done){
            Serial.print((char)ERROR);
            Serial.println("ACQ Not Done");
          }else{
            Serial.print((char)READ_ACQ);

            cap_print_sensor_array(mem_sensor_array, row_len, col_len);
//            cap_print_sensor_array(mem_sensor_fifo[mem_sensor_fifo_done_index], row_len, col_len);
            flag_acq_done = 0;
          }
          break;
        case READ_ACQ_BIN:

          if (!flag_acq_done){
            Serial.print((char)ERROR);
            Serial.println("ACQ Not Done");
          }else{
            Serial.print((char)READ_ACQ_BIN);
            cap_send_sensor_array(mem_sensor_array, row_len, col_len);
            flag_acq_done = 0;
          }
          break;
  
        case CAL_SENSOR:
          Serial.print((char)CAL_SENSOR);
          Serial.println("Calibration");

          flag_acq_calibrate = 1;
          
          
          break;

        case HELP:
          Serial.println((char)HELP);
          Serial.println("\'a\' - Start Acquisition");
          Serial.println("\'s\' - Stop Acquisition");
          Serial.println("\'r\' - Read data text");
          Serial.println("\'b\' - Read data binary");
          Serial.println("\'d\' - Return \'T\' or \'F\' if acquisition is done");
          Serial.println("\'c\' - Run calibration");
          
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
  int status;
  // Serial for debugging connection
//    Serial.begin(9600);




  Serial.begin(SERIAL_BAUD);

  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("Started");
  randomSeed(analogRead(0));

//  
  // Initializing wire connection
  Wire.begin();
  // set clock to 400 kHz
  Wire.setClock(I2C_CLOCK_MUX);

  Serial.println("Started Wire");


//
//
  // Initializing IO
//  pinMode(led,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(led4,OUTPUT);
  pinMode(led5,OUTPUT);
  pinMode(led6,OUTPUT);

  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
  digitalWrite(led4,LOW);
  digitalWrite(led5,LOW);
  digitalWrite(led6,LOW);

  for (i=0; i<5; i++){
    #define delayled 10
      delay(delayled);
      
      digitalWrite(led1,HIGH);
      delay(delayled);
      digitalWrite(led2,HIGH);
      delay(delayled);
      digitalWrite(led3,HIGH);
      delay(delayled);
      digitalWrite(led4,HIGH);
      delay(delayled);
      digitalWrite(led5,HIGH);
      delay(delayled);
      digitalWrite(led6,HIGH);
      
      delay(delayled);

      digitalWrite(led1,LOW);
      delay(delayled);
      digitalWrite(led2,LOW);
      delay(delayled);
      digitalWrite(led3,LOW);
      delay(delayled);
      digitalWrite(led4,LOW);
      delay(delayled);
      digitalWrite(led5,LOW);
      delay(delayled);
      digitalWrite(led6,LOW);

  }
//
//

  // Checking connectivity and operation of ADG2128

  // Initializing ADG2128 MUX
  pinMode(IO_MUX_RESET_N, OUTPUT);

  mux_reset();
  digitalWrite(IO_MUX_RESET_N,HIGH);

  Serial.println("Mux reset");

  
  // Checking operation of MUX  
  if (!mux_test_operation(I2C_ADDR_MUX2, VERBOSE)){
    Serial.print("MUX2 not working properly \n");
    digitalWrite(led5,HIGH);

  }
  if (!mux_test_operation(I2C_ADDR_MUX1, VERBOSE)){
    Serial.print("MUX1 not working properly \n");
    digitalWrite(led4,HIGH);
  }
  if (!mux_test_operation(I2C_ADDR_MUX2, VERBOSE)){
    Serial.print("MUX2 not working properly \n");
    digitalWrite(led5,HIGH);
  }

  mux_reset();

  Serial.println("Mux confirmed");


  //  Connect CIN1 on MUX1 Y5 to X0
  status = mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(0), mux_get_addr_y(5), MUX_SWITCH_ON, MUX_LDSW_LOAD );

  if (status){
    Serial.println("Mux not switched");
  }
  // Shortcircuit Y5 to Y4?
  status = mux_write_switch_config(I2C_ADDR_MUX1, mux_get_addr_x(0), mux_get_addr_y(4), MUX_SWITCH_OFF, MUX_LDSW_LOAD );
//  mux_read_config_matrix(I2C_ADDR_MUX1);

  if (status){
    Serial.println("Mux not switched");
  }

  Serial.println("Mux22 confirmed");


  // Resetting FDC1004

  cdc_reset_device(I2C_ADDR_CDC); 
  // Cost: 2 (R+W)

  Serial.println("cdc reset confirmed");


  // Checking connectivity and operation of FDC1004
  if (!cdc_test_id(I2C_ADDR_CDC)){
    // Cost: 2
    Serial.print("CDC not working properly \n");
  }
  Serial.println("cdc test ");

 #define CDC_MEASUREMENT CDC_MEAS1
  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, 0); // Cost: 1
  cdc_set_repeat_measurements(I2C_ADDR_CDC, CDC_ENABLE); // Cost: 2 R+W
  cdc_set_repeat_measurements(I2C_ADDR_CDC, CDC_DISABLE); // Cost: 2 R+W


  Serial.println("cdc repeat measurement");

  cdc_set_rate(I2C_ADDR_CDC, CDC_RATE_400SPS);
  
  cdc_set_measurement_enable(I2C_ADDR_CDC, CDC_MEASUREMENT, CDC_ENABLE);
  Serial.println("Mux333confirmed");

  
  // Reading configurations of all measurements
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1); // Cost: 1
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS2);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS3);
  cdc_get_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS4);

  Serial.println("before cap switch all");

//  cdc_print_configuration(I2C_ADDR_CDC);

  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  Serial.println("cap switch all");

//
//  
//  // Initalize memory
  for (i=0; i<MEM_FIFO_SIZE; i++){
    mem_sensor_fifo[i] = mem_init_float(row_len, col_len);
  }
  Serial.println("CDC22");

//  mem_sensor_array = mem_init_float(row_len, col_len);

  mem_sensor_array = mem_sensor_fifo[mem_sensor_fifo_index];
  
  mem_sensor_capdac_array = mem_init_int(row_len, col_len, 0);
  mem_sensor_offset_array = mem_init_int(row_len, col_len);




  Serial.println("Clearing cap rows and columns");
  cap_switch_clear_all_rows();
  cap_switch_clear_all_columns();
  Serial.println("Setting all rows and columns to shield");
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
  Serial.println("Calibrate sensors initialize");

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

//  return;


//  Serial.println("running");

  if (flag_acq_calibrate==1){

    digitalWrite(led3, HIGH);

    cap_switch_all_rows_signal(CAP_ROW_SHLD1);
    cap_switch_all_columns_signal(CAP_COL_SHLD1); 
    cap_calibrate_sensors(mem_sensor_fifo[mem_sensor_fifo_index], mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
    flag_acq_calibrate = 0;
    digitalWrite(led3, LOW);
  }

// flag_acq_running=1;
  if (flag_acq_running==1){
  
    digitalWrite(led2, HIGH);
    cap_get_measurement_iteration(mem_sensor_fifo[mem_sensor_fifo_index], mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
    flag_acq_done = 1;

    mem_sensor_fifo_done_index = mem_sensor_fifo_index ;
    mem_sensor_array = mem_sensor_fifo[mem_sensor_fifo_index];
    mem_sensor_fifo_index = mem_fifo_next_index(mem_sensor_fifo_index);
//    Serial.println(mem_sensor_fifo_index);
    digitalWrite(led2, LOW);

    

  }
//

  



}
