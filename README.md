# XBee Applications
This is a small collection of useful applications for programmable XBee modules (ZigBee variants). Most of these were created for an IoT project (not available here in its entirety) that allowed a person to monitor their energy consumption, their water usage, and the amount of trash they produced.

As their names imply, these applications were created with a purpose in mind. Nevertheless, they can be re-purposed for other tasks.

### Electricity Meter
This project allows you to add some wireless capability to a Kill-A-Watt electricity usage monitor. In order for this to be of any use, an XBee must be connected to the [internal circuitry](http://www.mikesmicromania.com/2013/03/kill-watt-circuit-analysis.html) of the usage monitor. The XBee will sample the voltage and current waveforms and compute the average energy used by any device plugged into the Kill-A-Watt.

### Water Sensor
Many residential water meters use rotating and magnetized components to measure the volume of water passing through them. This project allows an XBee module to work in conjunction with a hall effect sensor to count the number of rotations. Depending on the sensor you use, you may need to filter out any noise and amplify its output.

### Trash Sensor
This project allows an XBee to interface with an [HX711](https://www.sparkfun.com/products/13879) amplifier and load cell. It's original purpose was to track the amount of trash produced in a household. More generally, it can be used to build a simple weight scale with wireless capability.

### Serial Bypass
Example project provided by Digi in their programmable XBee SDK. By default, serial data is blocked by the secondary processor on programmable XBee modules. This application allows an XBee module to share its data with any device that has a serial port (e.g Raspberry Pi). 




## Handling Sensor Data
Sensor data can be stored on a computer by connecting to the serial port on an XBee module. The python script `xbee_receive.py` provides a basic solution for storing sensor data in a CSV file on a Raspberry Pi.

The script also serves as a complement to the XBee applications described above. For certain projects, the HCS08 processor on the programmable XBee modules may not have the capability to process floating point numbers and large integers efficiently. In those cases, the receiver module that collects sensor data can be connected to a device which can easily handle those operations.




## Usage
### Importing and Compiling Projects
The source files are organized so they can easily be [imported](https://xbee-sdk-doc.readthedocs.io/en/latest/doc/exporting_importing/#72-importing-a-project) and compiled in CodeWarrior, provided you have the [XBee SDK](https://www.digi.com/resources/documentation/Digidocs/90002126/concepts/c_programmable_xbee_sdk.htm?TocPath=Hardware%7C_____5) installed as well. For the chosen project, you will need to create a zip file containing the `data` directory and the `project.metadata` file.

### Flashing Firmware
The output files are called `[project-name].abs.bin` and can be found in the `/build/[project-name]` folder.

You can upload the file via local XMODEM with the aid of an [XBee Explorer board](https://www.sparkfun.com/products/11812). If you plan on doing basic configuration through the XCTU program, it is advantageous to use the legacy version of XCTU (version 5.2.8.6) as it has XMODEM included in the program.

### Storing Sensor Data
Depending on what version of python you want to use, you can install either the unofficial [XBee](https://pypi.org/project/XBee/) library used in `xbee_receive.py`, or the official [XBee Python](https://github.com/digidotcom/xbee-python) library (requires revising the python script).

Dependencies:

* xbee or digi-xbee
* pyserial
* pandas
* numpy




## License
This project does not include the full Digi XBee SDK sources. Unless otherwise noted in the source files, the code is BSD licensed.