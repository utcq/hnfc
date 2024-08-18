import serial
from time import sleep

board = serial.Serial('/dev/ttyACM0', 9600, timeout=4)

class COMMANDS_ENUM:
  PREFIX = 0x54
  FULL_READ = 0x51
  READ = 0x52
  READ_SEC = 0x53
  WRITE_SEC = 0x54
  WRITE = 0x55

class Interface():
  def __init__(self, board):
    self.board = board
    self.uid = None
    self.check()
    
  def check(self):
    if not self.board.isOpen():
      raise Exception('Board disconnected')
  
  def wait_for_card(self) -> int:
    print("Waiting for card...")
    while self.board.isOpen():
      r = board.read(2)
      if r and r == b"\x60\x10":
        break
    uid_size = board.read(1)
    self.uid = board.read(int.from_bytes(uid_size, byteorder='big'))
    return self.uid

  def hextobin(self, hexdump: list) -> bytes:
    hexdump = list(filter(lambda x: x != "", hexdump))
    if "--" in hexdump:
      print("[Warning] Block key not found! Returning None")
      return None
    return bytes([int(i, 16) for i in hexdump])
  
  def read_full(self) -> list[list[bytes]]:
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.FULL_READ]))
    self.board.read(8) # Skip first 8 bytes of sent data
    dump = []
    for _ in range(16): # Sectors * Blocks
      sector = []
      for _ in range(4):
        line = self.board.readline()
        hexd = line.decode('utf-8').strip().split(" ")
        sector.append( self.hextobin(hexd) )
      dump.append(sector)
    return dump
  
  def read(self, sector: int, block: int) -> bytes:
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.READ, sector, block]))
    self.board.read(8)
    line = self.board.readline()
    hexd = line.decode('utf-8').strip().split(" ")
    return self.hextobin(hexd)
  
  def read_sector(self, sector: int) -> list[bytes]:
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.READ_SEC, sector]))
    self.board.read(8)
    dump = []
    for _ in range(4):
      line = self.board.readline()
      hexd = line.decode('utf-8').strip().split(" ")
      dump.append( self.hextobin(hexd) )
    return dump

  def write_sector(self, sector: int, data: list[bytes]):
    assert len(data) == 4, "Data must have 4 blocks"
    assert all([len(i) == 16 for i in data]), "All blocks must have 16 bytes"
    self.check()
    self.board.write(bytes([COMMANDS_ENUM.PREFIX, COMMANDS_ENUM.WRITE_SEC, sector]))
    self.board.read(8)
    self.board.write(b''.join(data))


def main():
  iface = Interface(board)
  print("UID: ", iface.wait_for_card().hex())
  print ("Dumping all sectors")
  for i, sector in enumerate(iface.read_full()):
    print("Sector {}".format(i))
    for j,block in enumerate(sector):
      if block:
        print(" Block {}: {}".format(j, block.hex(sep=' ')))

if __name__ == '__main__':
  main()