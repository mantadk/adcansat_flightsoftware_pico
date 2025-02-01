import serial
import serial.tools.list_ports

def list_serial_ports():
    """Lists available serial ports."""
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("No serial devices found.")
        return []
    
    print("Available serial devices:")
    for i, port in enumerate(ports):
        print(f"{i}: {port.device}")
    
    return ports

def select_serial_port(ports):
    """Prompts the user to select a serial port."""
    while True:
        try:
            index = int(input("Select a serial port (number): "))
            if 0 <= index < len(ports):
                return ports[index].device
            else:
                print("Invalid selection. Try again.")
        except ValueError:
            print("Please enter a valid number.")

def read_serial(port, baudrate=115200):
    """Reads data from the selected serial port."""
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"Reading from {port} at {baudrate} baud...\n")
            while True:
                line = ser.readline().decode("utf-8").strip()
                if line:
                    print(f"Received: {line}")
    except serial.SerialException as e:
        print(f"Serial error: {e}")

if __name__ == "__main__":
    ports = list_serial_ports()
    if not ports:
        exit(1)

    selected_port = select_serial_port(ports)
    read_serial(selected_port)
