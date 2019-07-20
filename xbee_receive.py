import serial, time, os.path
from xbee import XBee, ZigBee
import pandas as pd
import numpy as np

filename = "sensors.csv"

# Scaling factors found experimentally.
e_scaling = 3.9096E-7
w_scaling = 0.32041
t_scaling = 1

label_frame = {'Sensor':['Electric', 'Water', 'Trash'], 'Value':[0, 0, 0],
               'Units':['W*h', 'mL', 'lb']}
if (not os.path.exists(filename)):
    # Create a CSV file with the proper row and column labels.
    pd.DataFrame(data=label_frame).to_csv(filename, index=False)

def print_data(data):
    # Process incoming data and store into proper location in CSV file.
    msg = data['rf_data'].decode("utf-8")
    data_frame = pd.read_csv(filename, index_col=0)
    
    if (msg[0] == 'E'):
        energy = int(msg[2:-1], base=10) * e_scaling
        data_frame.loc['Electric', 'Value'] += energy
        
    elif (msg[0] == 'W'):
        volume = int(msg[2:-1], base=10) * w_scaling
        data_frame.loc['Water', 'Value'] += volume

    elif (msg[0] == 'T'):
        weight = float(msg[2:-1]) * t_scaling
        data_frame.loc['Trash', 'Value'] += weight

    data_frame.to_csv(filename)
    print(data_frame, '\n')

# Allow Raspberry Pi to communicate with XBee module and
# call print_data when sensor data is received.
serial_port = serial.Serial('/dev/serial0', 115200)
xbee = ZigBee(serial_port, callback=print_data)

while True:
    try:
        time.sleep(0.001)
    except KeyboardInterrupt:
        break

# Write zeros to CSV file and close connections properly.
pd.DataFrame(data=label_frame).to_csv(filename, index=False)
xbee.halt()
serial_port.close()
