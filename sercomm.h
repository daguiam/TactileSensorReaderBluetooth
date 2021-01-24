
// Define the orders that can be sent and received
enum Order {
  START_ACQ = 'a',
  STOP_ACQ = 's',
  READ_ACQ = 'r',
  READ_ACQ_BIN = 'b',
  DONE_ACQ = 'd',
  CAL_SENSOR = 'c',
  HELP = 'h',
  HELP2 = '?',
//  CAL_DONE = 'e',
  EOL = '\n',
  ERROR = 'E',
  TRUE = 'T',
  FALSE = 'F',
 
};

typedef enum Order Order;
