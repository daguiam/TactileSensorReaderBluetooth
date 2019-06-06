

#define I2C_ADDR_MUX1 0b01110001
#define I2C_ADDR_MUX2 0b01110011
#define I2C_ADDR_CDC  0b01010000


// Definition of the MUX1 X inputs connected to the Capacitor array rows
#define CAP_R01  0
#define CAP_R02  6
#define CAP_R03  3
#define CAP_R04  9
#define CAP_R05  1
#define CAP_R06  7
#define CAP_R07  4
#define CAP_R08  10
#define CAP_R09  2
#define CAP_R10  8
#define CAP_R11  5
#define CAP_R12  11
#define CAP_ROW_ARRAY {CAP_R01,CAP_R02,CAP_R03,CAP_R04,CAP_R05,CAP_R06,CAP_R07,CAP_R08,CAP_R09,CAP_R10,CAP_R11,CAP_R12}
#define CAP_ROW_ARRAY_LEN 12

// Definition of the MUX2 X inputs connected to the Capacitor array columns
#define CAP_C01  11
#define CAP_C02  5
#define CAP_C03  10
#define CAP_C04  4
#define CAP_C05  9
#define CAP_C06  3
#define CAP_C07  8
#define CAP_C08  2
#define CAP_C09  1
#define CAP_C10  7
#define CAP_C11  0
#define CAP_C12  6
#define CAP_COLUMN_ARRAY {CAP_C01,CAP_C02,CAP_C03,CAP_C04,CAP_C05,CAP_C06,CAP_C07,CAP_C08,CAP_C09,CAP_C10,CAP_C11,CAP_C12}
#define CAP_COLUMN_ARRAY_LEN 12


#define CAP_ROW_SHLD1 1
#define CAP_ROW_SHLD2 0
#define CAP_ROW_CIN1 5
#define CAP_ROW_CIN2 4
#define CAP_ROW_CIN3 3
#define CAP_ROW_CIN4 2
#define CAP_ROW_CIN4 2
#define CAP_ROW_GND 7
#define CAP_ROW_Y_CONNECTIONS {"SHLD2", "SHLD1", "CIN4", "CIN3", "CIN2", "CIN1", "SHLD1", "GND"}


#define CAP_COL_SHLD1 1
#define CAP_COL_SHLD2 7
#define CAP_COL_GND 2
#define CAP_COLUMN_Y_CONNECTIONS {"SHLD1", "SHLD1", "GND", "SHLD1", "SHLD1", "SHLD1", "SHLD1", "SHLD2"}



#define CAP_SWITCH_ON 1
#define CAP_SWITCH_OFF 0



int cap_switch_row_signal(uint8_t row, uint8_t signal);
int cap_switch_all_rows_signal(uint8_t signal);
int cap_switch_clear_row(uint8_t row);
int cap_switch_clear_all_rows();

int cap_switch_column_signal(uint8_t column, uint8_t signal);
int cap_switch_all_columns_signal(uint8_t signal);
int cap_switch_clear_column(uint8_t column);
int cap_switch_clear_all_columns();


int cap_print_row_connections(uint8_t row);
int cap_print_column_connections(uint8_t column);
int cap_print_connections();

int cap_set_sensor_measurement_single(uint8_t position_row, uint8_t position_column);
