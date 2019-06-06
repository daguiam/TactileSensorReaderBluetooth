

#define MUX_PACKAGE_LENGTH 3

#define IO_MUX_RESET_N 16


#define MUX_RW_READ 1
#define MUX_SWITCH_ON 1
#define MUX_SWITCH_OFF 0
#define MUX_LDSW_LOAD 1
#define MUX_LDSW_LATCH 0


#define MUX_READ_X0 0b00110100
#define MUX_READ_X1 0b00111100
#define MUX_READ_X2 0b01110100
#define MUX_READ_X3 0b01111100
#define MUX_READ_X4 0b00110101
#define MUX_READ_X5 0b00111101
#define MUX_READ_X6 0b01110101
#define MUX_READ_X7 0b01111101
#define MUX_READ_X8 0b00110110
#define MUX_READ_X9 0b00111110
#define MUX_READ_X10 0b01110110
#define MUX_READ_X11 0b01111110
#define MUX_READ_X_ARRAY {MUX_READ_X0, MUX_READ_X1, MUX_READ_X2, MUX_READ_X3, MUX_READ_X4, MUX_READ_X5, MUX_READ_X6, MUX_READ_X7, MUX_READ_X8, MUX_READ_X9, MUX_READ_X10, MUX_READ_X11}


char mux_reset();
char mux_get_addr_x(char x);
char mux_get_addr_y(char y);

char mux_read_config(uint8_t addr, uint8_t read_addr_X, uint8_t verbose=0);
char mux_write_switch_config(uint8_t addr, uint8_t addr_x, uint8_t addr_y, uint8_t data, uint8_t ldsw);


bool mux_read_config_matrix(uint8_t addr);
bool mux_test_operation(uint8_t addr, bool verbose=0);
