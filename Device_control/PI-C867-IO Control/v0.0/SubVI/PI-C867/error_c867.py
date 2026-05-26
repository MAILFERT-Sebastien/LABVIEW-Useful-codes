import os
import serial
import time

PORT     = "COM15"
BAUDRATE = 115200

with serial.Serial(PORT, BAUDRATE, timeout=2) as ser:
    time.sleep(0.5)
    ser.reset_input_buffer()
    
    for cmd in ["ERR?", "RMC?", "MAC?"]:
        ser.write((cmd + "\n").encode())
        time.sleep(0.5)
        rep = ser.read_all().decode().strip()
        print(f"{cmd} = '{rep}'")