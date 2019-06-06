

#define CDC_MEAS1_MSB 0x00   // 0x0000 // MSB portion of Measurement 1
#define CDC_MEAS1_LSB 0x01  // 0x0000 // LSB portion of Measurement 1
#define CDC_MEAS2_MSB 0x02  // 0x0000 // MSB portion of Measurement 2
#define CDC_MEAS2_LSB 0x03  // 0x0000 // LSB portion of Measurement 2
#define CDC_MEAS3_MSB 0x04  // 0x0000 // MSB portion of Measurement 3
#define CDC_MEAS3_LSB 0x05  // 0x0000 // LSB portion of Measurement 3
#define CDC_MEAS4_MSB 0x06  // 0x0000 // MSB portion of Measurement 4
#define CDC_MEAS4_LSB 0x07  // 0x0000 // LSB portion of Measurement 4
#define CDC_CONF_MEAS1 0x08 // 0x1C00 // Measurement 1 Configuration
#define CDC_CONF_MEAS2 0x09 // 0x1C00 // Measurement 2 Configuration
#define CDC_CONF_MEAS3 0x0A // 0x1C00 // Measurement 3 Configuration
#define CDC_CONF_MEAS4 0x0B // 0x1C00 // Measurement 4 Configuration
#define CDC_FDC_CONF 0x0C // 0x0000 // Capacitance to Digital Configuration
#define CDC_OFFSET_CAL_CIN1 0x0D  // 0x0000 // CIN1 Offset Calibration
#define CDC_OFFSET_CAL_CIN2 0x0E  // 0x0000 // CIN2 Offset Calibration
#define CDC_OFFSET_CAL_CIN3 0x0F  // 0x0000 // CIN3 Offset Calibration
#define CDC_OFFSET_CAL_CIN4 0x10  // 0x0000 // CIN4 Offset Calibration
#define CDC_GAIN_CAL_CIN1 0x11  // 0x4000 // CIN1 Gain Calibration
#define CDC_GAIN_CAL_CIN2 0x12  // 0x4000 // CIN2 Gain Calibration
#define CDC_GAIN_CAL_CIN3 0x13  // 0x4000 // CIN3 Gain Calibration
#define CDC_GAIN_CAL_CIN4 0x14  // 0x4000 // CIN4 Gain Calibration
#define CDC_MANUFACTURER_ID 0xFE  // 0x5449 // ID of Texas Instruments
#define CDC_DEVICE_ID 0xFF // 0x1004 ID of FDC1004 device
#define CDC_MANUFACTURER_ID_RESPONSE 0x5449 // ID of Texas Instruments
#define CDC_DEVICE_ID_RESPONSE 0x1004 //ID of FDC1004 device


#define CDC_CHANNEL_CIN1 0b000
#define CDC_CHANNEL_CIN2 0b001
#define CDC_CHANNEL_CIN3 0b010
#define CDC_CHANNEL_CIN4 0b011
#define CDC_CHANNEL_CAPDAC 0b100
#define CDC_CHANNEL_DISABLED 0b111

#define CDC_MEAS1 1
#define CDC_MEAS2 2
#define CDC_MEAS3 3
#define CDC_MEAS4 4
//
//#define CDC_REPEAT_ENABLE 1
//#define CDC_REPEAT_DISABLE 0

#define CDC_ENABLE 1
#define CDC_DISABLE 0



uint16_t cdc_read_register(uint8_t addr, uint8_t register, uint8_t verbose=0);
char cdc_write_register(uint8_t addr, uint8_t pointer, uint16_t data, uint8_t verbose=0);


bool cdc_test_id(uint8_t addr);
char cdc_reset_device(uint8_t addr);

char cdc_print_configuration(uint8_t addr);

char cdc_set_repeat_measurements(uint8_t addr, uint8_t repeat);
char cdc_set_measurement_enable(uint8_t addr, uint8_t channel, uint8_t enable);
int cdc_get_measurement_completed(uint8_t addr, uint8_t channel);
char cdc_set_measurement_configuration(uint8_t addr, uint8_t measurement, uint8_t input_cha, uint8_t input_chb, uint8_t capdac, uint8_t verbose=0);
uint16_t cdc_get_measurement_configuration(uint8_t addr, uint8_t measurement, char verbose=0);

int32_t cdc_get_measurement(uint8_t addr, uint8_t measurement);
float cdc_convert_capacitance(int32_t value, int32_t capdac);
int cdc_measurement_saturated(int32_t value);
