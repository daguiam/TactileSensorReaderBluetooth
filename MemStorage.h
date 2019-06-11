#include "CapArray.h"

float * mem_init_float(int row_len, int col_len, float value=0);
int mem_clear_float(float * mem_array, int row_len, int col_len, float value);
int mem_store_float(float * mem_array, int row, int col, int row_len, int col_len, float value);
float mem_get_float(float * mem_array, int row, int col, int row_len, int col_len );




//int mem_store_int(int ** mem_array, int row, int column, int value);
//int mem_clear_int(float ** mem_array, int row_len, int column_len, int value=0);
//int mem_get_position_int(int ** mem_array, int row, int column);
//int * mem_get_row_int(int ** mem_array, int row);
//
//
//float ** mem_init_float(int row_len, int column_len, float value=0);
//int mem_store_float(float ** mem_array, int row, int column, float value);
//int mem_clear_float(float ** mem_array, int row_len, int column_len, float value=0);
//float mem_get_position_float(float ** mem_array, int row, int column);
//float * mem_get_row_float(float ** mem_array, int row);
