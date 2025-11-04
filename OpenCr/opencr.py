#!/usr/bin/env python3
import serial
import time

# Pas dit eventueel aan:
PORT = "/dev/ttyACM0"   # of /dev/ttyACM0, afhankelijk van jouw Arduino
BAUD = 57600

# Seriële poort openen
ser = serial.Serial(PORT, BAUD, timeout=0.1)
time.sleep(2)  # even wachten zodat Arduino kan resetten

print("Verbonden met", PORT)
print("Typ je commando's (bijv. 'V 50 20' of 'D 30 20 2'). Ctrl+C om te stoppen.\n")

try:
    while True:
        cmd = input("> ").strip()
        if not cmd:
            continue

        # Stuur commando + newline
        ser.write((cmd + "\n").encode())

        # Lees eventuele respons
        time.sleep(0.1)
        while ser.in_waiting:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                print("<", line)
except KeyboardInterrupt:
    print("\nStoppen...")
finally:
    ser.close()
