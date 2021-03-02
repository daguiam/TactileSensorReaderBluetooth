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
    with serial.Serial(port, baud, timeout=timeout, rtscts=False, dsrdtr=True) as ser:

        # time.sleep(1)
        print("In waiting:", ser.in_waiting)
        print("In waiting:", ser.inWaiting())

        # while ser.in_waiting:
        while ser.in_waiting:
            print("In waiting:", ser.in_waiting)
        # s = ser.read(ser.in_waiting)
            s = ser.read(ser.in_waiting)
            # if len(s)==0:
            #     break
            print(time.time(), '\t',s)
        print("In waiting:", ser.in_waiting)




def read_acquisition(port, baud, timeout=1):
    with serial.Serial(port, baud, timeout=timeout, rtscts=False, dsrdtr=True) as ser:
        # ser.flush()
        # ser.reset_input_buffer()

        print("write read acq: ", READ_ACQ)
        x = ser.write(READ_ACQ)
        # x = ser.write(READ_ACQ_BIN)
        
        time.sleep(0.1)

        result = ser.read()
        print("result reading after acquisition: ",result)

        result = ser.read()
        print("result reading after acquisition: ",result)
        if result == CMD_ERROR:
            # s = ser.readline()
            print(time.time(), '\tError:',result)
        else:
            # buffer = ser.readline()
            for i in range(12):
                result = ser.readline()
                # result = ser.read(PACKET_SIZE_BIN)


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

    



def start_acquisition(port, baud, timeout=1):
    print("Starting acquisition")
    result = send_check_command(b'a', port, baud, timeout=1)
    print(result)
    
def send_check_command(command, port, baud, timeout=0):
    with serial.Serial(port, baud, timeout=timeout, rtscts=False, dsrdtr=True, write_timeout=timeout) as ser:
        print (command)
        x = ser.write(command)

        print ("wrote")

        time.sleep(0.01)
        # result = ser.read()    # readline is blocking so it always waits
        # print(result)
        # if result[0] != command:
        #     print("send_check_command error [%s] received: %s"%(command, result))
        #     return 0
        # return result
        # while(1):
        #     result = ser.readline()    # readline is blocking so it always waits
        #     print(len(result))
        #     if result[0] != command:
        #         print("send_check_command error [%s] received: %s"%(command, result))
        #         continue
        #     break
        # return result
        return 1


if __name__ == "__main__":


    port = "COM8"

    baud = 230400
    baud = 230400
    print("test2")

    # read_everything(port, baud, timeout=1)

    # start_acquisition(port, baud, timeout=1)

    with serial.Serial(port, baud, timeout=0.1, write_timeout=0.1, rtscts=False, dsrdtr=False,) as ser:
    # # with serial.Serial() as ser:

        # it will reset board..
        time.sleep(2)
        ser.flushInput()
        ser.flushOutput()


        while ser.inWaiting()>0:
            ser.read(1)

        # Start acquisition
        ser.write(b'c')
        ser.write(b's')
        ser.write(b'a')

        last_time = time.time()
        while True:
            # if ser.in_waiting>0:
            #     val = str(ser.readline().decode().strip('\r\n'))#Capture serial output as a decoded string
            #     valA = val.split("/")
            #     #print()
            #     print(ser.in_waiting)
            #     print(val)
            #     # ser.flushInput()
            #     # ser.flushOutput()
            # print("out_waiting",ser.out_waiting)

            ser.write(b'r')


            if ser.in_waiting>0:
                val = str(ser.readline().decode().strip('\r\n'))#Capture serial output as a decoded string
                valA = val.split("/")
                #print()
                # print(ser.in_waiting)
                # print(val)
                if len(val)> 0:
                    if val[0] == 'r':
                        print(val)

                        current_time = time.time()
                        time_delta = current_time-last_time

                        last_time = current_time
                        print(time_delta)
                        ser.flushInput()
                # ser.flushInput()
                # ser.flushOutput()

            time.sleep(0.01)
            # print(ord(command))
            # ser.write(command)
            # print(command)
            # print(command)
            # print(command)
            # print(command)
            # print(command)
            # print(command)
            # # data = ser.read(100)
            # # time.sleep(0.1)
            # # print(data.rstrip())
            # ser.close()



    # read_everything(port, baud, timeout=1)
    # start_acquisition(port, baud, timeout=1)

    # while 1:

    #     print("inside while")
    #     start_acquisition(port, baud, timeout=1)
    #     # read_everything(port, baud, timeout=1)

    #     time.sleep(1)
    #     # read_acquisition(port, baud, timeout=1)
    #     break


