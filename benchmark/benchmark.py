import matplotlib.pyplot as plt
import numpy as np
import serial
import time


START_ACQ = b'a'
STOP_ACQ = b's'
READ_ACQ = b'r'
READ_ACQ_BIN = b'b'
DONE_ACQ = b'd'
CAL_SENSOR = b'c'
HELP = b'h'
CMD_ERROR = b'E'
PACKET_SIZE_BIN = 12*12*4


def read_everything(port, baud, timeout=1):
    with serial.Serial(port, baud, timeout=timeout) as ser:

        time.sleep(1)
        while ser.in_waiting:
        
        # s = ser.read(ser.in_waiting)
            s = ser.readline()
            # if len(s)==0:
            #     break
            print(time.time(), '\t',s)


def start_acquisition(port, baud, timeout=1):
    with serial.Serial(port, baud, timeout=timeout) as ser:
        # ser.flush()
        print("Starting acquisition")
        x = ser.write(START_ACQ)
        time.sleep(0.1)
        s = ser.readline()
        print(time.time(), '\t',s)
    
def read_acquisition(port, baud, timeout=1):
    with serial.Serial(port, baud, timeout=timeout) as ser:
        # ser.flush()
        # ser.reset_input_buffer()
        x = ser.write(READ_ACQ)
        
        result = ser.read()
        print(result)
        if result == CMD_ERROR:
            # s = ser.readline()
            print(time.time(), '\tError:',result)
        else:
            # buffer = ser.readline()
            for i in range(12):
                result = ser.readline()

                print(time.time(), '\t', result)


# def read_acquisition_binary(port, baud, timeout=1):
#     with serial.Serial(port, baud, timeout=timeout) as ser:
#         ser.flush()
#         ser.reset_input_buffer()
#         x = ser.write(READ_ACQ_BIN)
        
#         result = ser.read()
#         if result == CMD_ERROR:
#             s = ser.readline()
#             print(time.time(), '\tError:',s)
#         else:
#             buffer = ser.read(PACKET_SIZE_BIN)
#             print(time.time(), '\t', len(buffer))

    


if __name__ == "__main__":


    port = "COM8"

    baud = 230400
    print("test")

    read_everything(port, baud, timeout=1)

    # start_acquisition(port, baud, timeout=1)



    read_everything(port, baud, timeout=1)

    while 0:

        read_acquisition(port, baud, timeout=1)



