import serial
import time

def log_uart_messages(port, baudrate):
    ser = serial.Serial(port, baudrate, timeout=1)
    log_file = open('bitflip_log.txt', 'a')

    try:
        while True:
            if ser.in_waiting:
                message = ser.readline().decode('utf-8').strip()
                timestamp = time.strftime('%d-%m-%Y %H:%M:%S')
                formatted_message = f"{timestamp} - {message}"
                print(formatted_message)
                log_file.write(f"{formatted_message}\n")
                log_file.flush()
    except KeyboardInterrupt:
        print("Logging stopped.")
    finally:
        ser.close()
        log_file.close()

if __name__ == "__main__":
    port = 'COM5'
    baudrate = 9600
    log_uart_messages(port, baudrate)
