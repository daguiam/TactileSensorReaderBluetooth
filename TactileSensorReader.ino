/*
 * INL Capacitive Tactile Sensor Firmware
 * 
 * 
 */
#include <pthread.h>

#include <Wire.h>
#include "ADG2128.h"
#include "FDC1004.h"
#include "CapArray.h"
#include "MemStorage.h"
#include "parameters.h"
#include "sercomm.h"



#define led 13
#define led2 17


int capdac_value = 0;



float * mem_sensor_array;
int * mem_sensor_capdac_array;
int * mem_sensor_offset_array;
int row_len = CAP_ROW_ARRAY_LEN;
int col_len = CAP_COLUMN_ARRAY_LEN;



TaskHandle_t Task_LoopMeasurements;



void write_text(char * text){
  Serial.write(text);
}




void Task2Code( void * pvParameters ){

  int row, col;
  int capdac=7;
  float capacitance;
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());


  // Configure the measurements
  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

  for(;;){
    digitalWrite(led2, HIGH);

    // Clear the memory 0    

    mem_clear_float(mem_sensor_array, row_len, col_len);

    for (row=0; row<row_len; row++){
      for (col=0; col<col_len; col++){
        capacitance = cap_get_measurement_single(row, col, capdac);
        mem_store_float(mem_sensor_array, row, col, row_len, col_len, capacitance);
      }

    }
    digitalWrite(led2, LOW);
    delay(100);
    
  }
}     


void *Thread_AcquireSensorData(void *threadid) {

  digitalWrite(led2, HIGH);

  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);

//  cap_send_sensor_array(mem_sensor_array, row_len, col_len);


  digitalWrite(led2, LOW);
  
   
}




  
void setup() {
  pthread_t threads[4];
  int returnValue;
  // Serial for debugging connection
//    Serial.begin(9600);




  Serial.begin(SERIAL_BAUD);

  
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
//  Serial.print("MUX1\n");
//  mux_read_config_matrix(I2C_ADDR_MUX1);
//  Serial.print("MUX2\n");
//  mux_read_config_matrix(I2C_ADDR_MUX2);


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
//  mem_sensors_values_1 = mem_init_float(CAP_ROW_ARRAY_LEN,CAP_COLUMN_ARRAY_LEN);
//  mem_sensors_values = mem_sensors_values_1;

  mem_sensor_array = mem_init_float(row_len, col_len);
  mem_sensor_capdac_array = mem_init_int(row_len, col_len, 5);
  mem_sensor_offset_array = mem_init_int(row_len, col_len);




//  Serial.println("Clearing cap rows and columns");
  cap_switch_clear_all_rows();
  cap_switch_clear_all_columns();
//  Serial.println("MUX1");
//  mux_read_config_matrix(I2C_ADDR_MUX1);
//  Serial.println("MUX2");
//  mux_read_config_matrix(I2C_ADDR_MUX2);
//
//

//  Serial.println("Setting all rows and columns to shield");
  cap_switch_all_rows_signal(CAP_ROW_SHLD1);
  cap_switch_all_columns_signal(CAP_COL_SHLD1);
//  Serial.println("Settings row R01 to CIN1 and columns to GND");
  cap_switch_row_signal(CAP_R01, CAP_ROW_CIN1);
  cap_switch_column_signal(CAP_C01, CAP_COL_GND);


  cap_calibrate_sensors(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);


  
//
//  xTaskCreatePinnedToCore(
//                    Task2Code,   /* Task function. */
//                    "Task_LoopMeasurements",     /* name of task. */
//                    10000,       /* Stack size of task */
//                    NULL,        /* parameter of the task */
//                    1,           /* priority of the task */
//                    &Task_LoopMeasurements,      /* Task handle to keep track of created task */
//                    0);          /* pin task to core 1 */

//  returnValue = pthread_create(&threads[0], NULL, Thread_AcquireSensorData, (void *)0);

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
  digitalWrite(led,HIGH);

  digitalWrite(led,LOW);



  // Verifies if there is a message on the serial port



  digitalWrite(led,HIGH);
  
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);


  // Configure the measurements
//  cdc_set_measurement_configuration(I2C_ADDR_CDC, CDC_MEAS1, CDC_CHANNEL_CIN1, CDC_CHANNEL_CAPDAC, capdac);

  digitalWrite(led2, HIGH);
  cap_get_measurement_iteration(mem_sensor_array, mem_sensor_capdac_array, mem_sensor_offset_array, row_len, col_len);
  cap_send_sensor_array(mem_sensor_array, row_len, col_len);
  digitalWrite(led2, LOW);





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


  digitalWrite(led,LOW);

  
//
//
//  cap_set_sensor_measurement_single(5,5);
//  cap_print_connections();

  



}
