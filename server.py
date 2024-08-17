import serial
from time import sleep

board = serial.Serial('/dev/ttyACM0', 9600, timeout=0)

def main():
  if not board.isOpen():
    print('Board not connected')
    return

  
  while board.isOpen():
    r = board.read(2)
    if r and r == b"\x60\x10":
      break
  
  print("Card connected")
  uid_size = board.read(1)
  uid = board.read(int.from_bytes(uid_size, byteorder='big'))
  print("UID: ", uid.hex())

if __name__ == '__main__':
  main()