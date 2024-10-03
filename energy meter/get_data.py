import requests
import json
import struct
from pymodbus.client import ModbusSerialClient as ModbusClient
import time
def read_modbus_values():
    client = ModbusClient(
        port="/dev/ttyUSB0",  # Adjust to your serial port
        baudrate=9600,
        parity='E',
        stopbits=1,
        bytesize=8,
        timeout=1
    )
    try:
        client.connect()

        # Read voltage from register 40141 (address 140)
        voltage_result = client.read_holding_registers(address=140, count=2, slave=1)
        if not voltage_result.isError():
            voltage_registers = voltage_result.registers
            voltage = struct.unpack('>f', struct.pack('>HH', voltage_registers[1], voltage_registers[0]))[0]
        else:
            voltage = None

        # Read current from register 40149 (address 148)
        current_result = client.read_holding_registers(address=148, count=2, slave=1)
        if not current_result.isError():
            current_registers = current_result.registers
            current = struct.unpack('>f', struct.pack('>HH', current_registers[1], current_registers[0]))[0]
        else:
            current = None

        # Read frequency from register 40157 (address 156)
        frequency_result = client.read_holding_registers(address=156, count=2, slave=1)
        if not frequency_result.isError():
            frequency_registers = frequency_result.registers
            frequency = struct.unpack('>f', struct.pack('>HH', frequency_registers[1], frequency_registers[0]))[0]
        else:
            frequency = None

        # Read total power from register 40101 (address 100)
        power_result = client.read_holding_registers(address=100, count=2, slave=1)
        if not power_result.isError():
            power_registers = power_result.registers
            power = struct.unpack('>f', struct.pack('>HH', power_registers[1], power_registers[0]))[0]
        else:
            power = None

        
        return voltage, current, frequency, power

    finally:
        client.close()

def send_data_to_server(voltage, current, frequency, power):
    url = "https://ctop.iiit.ac.in/api/cin/create/36"
    data = {
        "Voltage": round(voltage, 3),
        "Current": round(current, 3),
        "Frequency": round(frequency, 3),
        "Total Power": round(power, 3)
    }
    headers = {"Content-Type": "application/json", "Authorization": "Bearer eb15729b605ebd4982b6e9e49fef5150"}

    response = requests.post(url, data=json.dumps(data), headers=headers)

    if response.status_code == 200:
        print("Success:", response.text)
    else:
        print("Error:", response.status_code, response.text)

if __name__ == "__main__":
    while True:
        voltage, current, frequency, power = read_modbus_values()
        if voltage is not None and current is not None and frequency is not None and power is not None:
            send_data_to_server(voltage, current, frequency, power)
        else:
            print("Error reading Modbus values")
        time.sleep(30)    
