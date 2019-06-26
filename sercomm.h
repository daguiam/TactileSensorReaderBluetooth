
// Define the orders that can be sent and received
enum Order {
  START_ACQ = 0x01,
  STOP_ACQ = 0x03,
  READ_ACQ = 0x02,
  CAL_SENSOR = 0x04,
  
  ERROR = 0xAA,
 
};

typedef enum Order Order;
