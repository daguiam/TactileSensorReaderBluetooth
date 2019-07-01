
// Define the orders that can be sent and received
enum Order {
  START_ACQ = 'a',
  STOP_ACQ = 's',
  READ_ACQ = 'r',
  READ_ACQ_BIN = 'b',
  DONE_ACQ = 'd',
  CAL_SENSOR = 'c',
  EOL = '\n',
  ERROR = 'E',
 
};

typedef enum Order Order;
