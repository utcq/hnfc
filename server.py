import serial
from time import sleep

board = serial.Serial('/dev/ttyACM0', 9600, timeout=4)

class COMMANDS_ENUM:
  PREFIX = 0x54
  FULL_READ = 0x51
  READ = 0x52

class Interface():
  def __init__(self, board):
    self.board = board
    self.uid = None
    self.check()
    
  def check(self):
    if not self.board.isOpen():
      raise Exception('Board disconnected')
  
  def wait_for_card(self) -> int:
    while self.board.isOpen():
      r = board.read(2)
      if r and r == b"\x60\x10":
        break
    uid_size = board.read(1)
    self.uid = board.read(int.from_bytes(uid_size, byteorder='big'))
    return self.uid
  
  def read_full(self):
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.FULL_READ]))
    dump = []
    for _ in range(16*4): # Sectors * Blocks
      dump.extend(self.board.readline().decode('utf-8').strip().split(" "))
    return dump

  def stripToHex(self, data: bytes):
    # While first byte of data is not a hex character, remove it
    new = bytes()
    for i in data:
      if i.isalnum():
        new += i.encode('utf-8')
  
  def read(self, sector: int, block: int):
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.READ, sector, block]))
    line = self.board.readline()
    line = self.stripToHex(line)
    print(line)
    return line.decode('utf-8').strip().split(" ")
  
  def read_sector(self, sector: int):
    for block in range(4):
      yield self.read(sector, block)

def main():
  iface = Interface(board)
  print("UID: ", iface.wait_for_card().hex())
  print ("Reading sector 4")
  for block in iface.read_sector(4):
    print(block)

if __name__ == '__main__':
  main()