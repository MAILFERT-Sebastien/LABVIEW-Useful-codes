import serial
import time

PORT     = "COM15"
BAUDRATE = 115200

with serial.Serial(PORT, BAUDRATE, timeout=2) as ser:
    time.sleep(0.5)
    ser.reset_input_buffer()
    
    # lire position en boucle
    try:
        while True:
            ser.write(b"POS?\n")
            time.sleep(0.3)
            rep = ser.read_all().decode().strip()
            print(f"POS? = {rep}")
            time.sleep(0.5)
    except KeyboardInterrupt:
        print("Arret.")