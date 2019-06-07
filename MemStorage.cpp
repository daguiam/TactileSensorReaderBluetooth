#include <Arduino.h>
#include "MemStorage.h"



float ** mem_init_float(int row_len, int column_len, float value){
  float ** mem_array = (float**) malloc(row_len*sizeof(float*));
  for (int i=0; i<row_len; i++){
    mem_array[i] = (float*)malloc(column_len*sizeof(float));
      for (int j=0; i<row_len; i++){
        mem_array[i][j] = value;
      }
  }
  return mem_array;
}
//
//int ** mem_init_float(int row_len, int column_len, float value){
//  float ** mem_array = (float**) malloc(row_len*sizeof(float*));
//  for (int i=0; i<row_len; i++){
//    mem_array[i] = (float*)malloc(column_len*sizeof(float));
//      for (int j=0; i<row_len; i++){
//        mem_array[i][j] = value;
//      }
//  }
//  return mem_array;
//}


int mem_store_float(float ** mem_array, int row, int column, float value){
  mem_array[row][column] = value;
}


int mem_clear_float(float ** mem_array, int row_len, int column_len, float value){
  int row, column;
  for (row=0; row<row_len; row++){
   for (column=0; column<column_len; column++){
      mem_array[row][column] = value;  
    } 
  } 
}

float mem_get_position_float(float ** mem_array, int row, int column){
  return mem_array[row][column];
}


float * mem_get_row_float(float ** mem_array, int row){
  return mem_array[row];
}



int mem_store_int(int ** mem_array, int row, int column, int value){
  mem_array[row][column] = value;
}


int mem_clear_int(float ** mem_array, int row_len, int column_len, int value){
  int row, column;
  for (row=0; row<row_len; row++){
   for (column=0; column<column_len; column++){
      mem_array[row][column] = value;  
    } 
  } 
}

int mem_get_position_int(int ** mem_array, int row, int column){
  return mem_array[row][column];
}

int * mem_get_row_int(int ** mem_array, int row){
  return mem_array[row];
}
