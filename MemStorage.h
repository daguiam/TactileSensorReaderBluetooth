

float * mem_init_float(int row_len, int col_len, float value=0);
int mem_clear_float(float * mem_array, int row_len, int col_len, float value=0);
int mem_store_float(float * mem_array, int row, int col, int row_len, int col_len, float value);
float mem_get_float(float * mem_array, int row, int col, int row_len, int col_len );
int mem_copy_float(float * mem_src, float * mem_dst, int row_len, int col_len );



int * mem_init_int(int row_len, int col_len, int value=0);
int mem_clear_int(int * mem_array, int row_len, int col_len, int value=0);
int mem_store_int(int * mem_array, int row, int col, int row_len, int col_len, int value);
int mem_get_int(int * mem_array, int row, int col, int row_len, int col_len );




int mem_print_float(float * mem_array, int row_len, int col_len);
int mem_print_int(int * mem_array, int row_len, int col_len);
