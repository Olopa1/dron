#192.168.0.204

import socket
import struct

HOST = '192.168.0.103'  # <-- wpisz IP ESP32
PORT = 6969             # <-- port z twojego kodu na ESP32


def send_int32(value):
    # Zamień liczbę na 4 bajty (int32, little-endian)
    data = struct.pack('<i', value)

    with socket.socket() as s:
        s.connect((HOST, PORT))
        s.sendall(data)

        # Odbierz 4 bajty (odpowiedź)
        #response = s.recv(4)
        #returned_value = struct.unpack('<i', response)[0]
        #print("Odebrano z ESP32:", returned_value)

def main():
    print("Połączono z ESP32. Wpisz liczbę (lub 'exit'):")
    while True:
        user_input = input(">>> ")
        if user_input.lower() in ('exit', 'quit'):
            break
        try:
            number = int(user_input)
            send_int32(number)
        except ValueError:
            print("To nie jest prawidłowa liczba całkowita!")

if __name__ == "__main__":
    main()