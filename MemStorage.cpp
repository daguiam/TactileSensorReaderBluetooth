#include <Arduino.h>
#include "MemStorage.h"



// Internal memory storage is handled as 1D array, using the arguments row_len and column_len to define where each row/columns are placed


float * mem_init_float(int row_len, int col_len, float value){
  float * mem_array = (float*) malloc(row_len*col_len*sizeof(float));
  mem_clear_float(mem_array, row_len, col_len, value);
  return mem_array;
}

int mem_clear_float(float * mem_array, int row_len, int col_len, float value){
  for (int i=0; i<row_len*col_len; i++){
    mem_array[i] = value;
  }
}

int mem_store_float(float * mem_array, int row, int col, int row_len, int col_len, float value){
  mem_array[row*col_len + col] = value;
}

float mem_get_float(float * mem_array, int row, int col, int row_len, int col_len ){
  return mem_array[row*col_len + col];
}

//////////////// INT

int * mem_init_int(int row_len, int col_len, int value){
  int * mem_array = (int*) malloc(row_len*col_len*sizeof(int));
  mem_clear_int(mem_array, row_len, col_len, value);
  return mem_array;
}

int mem_clear_int(int * mem_array, int row_len, int col_len, int value){
  for (int i=0; i<row_len*col_len; i++){
    mem_array[i] = value;
  }
}

int mem_store_int(int * mem_array, int row, int col, int row_len, int col_len, int value){
  mem_array[row*col_len + col] = value;
}

int mem_get_int(int * mem_array, int row, int col, int row_len, int col_len ){
  return mem_array[row*col_len + col];
}



int mem_print_float(float * mem_array, int row_len, int col_len){
  int row,col;
  for (row=0; row<row_len; row++){
    for (col=0; col<col_len; col++){
      Serial.print(mem_get_float(mem_array, row, col, row_len, col_len),2);
      Serial.print(" ");
    }
    Serial.println();
  }
}



int mem_print_int(int * mem_array, int row_len, int col_len){
  int row,col;
  for (row=0; row<row_len; row++){
    for (col=0; col<col_len; col++){
      Serial.print(mem_get_int(mem_array, row, col, row_len, col_len), HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
