import os.path
import time
import serial
import pandas as pd
import numpy as np
from xbee import XBee, ZigBee

FILENAME = "sensors.csv"

# Scaling factors found experimentally. To change the units
# (.e.g. from pounds to kilograms), multiply these numbers by
# the conversion ratio.
E_SCALING_FACTOR = 3.9096E-7 # W*h
W_SCALING_FACTOR = 0.32041 # mL
T_SCALING_FACTOR = 1 # lb

LABEL_FRAME = {'Sensor':['Electric', 'Water', 'Trash'], 'Value':[0, 0, 0],
               'Units':['W*h', 'mL', 'lb']}
if not os.path.exists(FILENAME):
    # Create a CSV file with the proper row and column labels.
    pd.DataFrame(data=LABEL_FRAME).to_csv(FILENAME, index=False)

def print_data(data):
    # Process incoming data and store into proper location in CSV file.
    msg = data['rf_data'].decode("utf-8")
    data_frame = pd.read_csv(FILENAME, index_col=0)

    if msg[0] == 'E':
        energy = int(msg[2:-1], base=10) * E_SCALING_FACTOR
        data_frame.loc['Electric', 'Value'] += energy

    elif msg[0] == 'W':
        volume = int(msg[2:-1], base=10) * W_SCALING_FACTOR
        data_frame.loc['Water', 'Value'] += volume

    elif msg[0] == 'T':
        weight = float(msg[2:-1]) * T_SCALING_FACTOR
        data_frame.loc['Trash', 'Value'] += weight

    data_frame.to_csv(FILENAME)
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
pd.DataFrame(data=LABEL_FRAME).to_csv(FILENAME, index=False)
xbee.halt()
serial_port.close()
