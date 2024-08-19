# HNFC 
## RFID bridge between hardware and high level operations

## Setup
In my case I'm using a RFID-RC522 connected directly to an Arduino UNO
![Setup](https://github.com/utcq/hnfc/blob/main/setup.jpg?raw=true)

| 3.3V 	| 3.3V 	|
|------	|------	|
| RST  	| D5   	|
| GND  	| GND  	|
| IRQ  	|  --  	|
| MISO 	| 12   	|
| MOSI 	| 11   	|
| SCK  	| 13   	|
| SDA  	| 10   	|


## How

It all works through Serial communication.


Server example:
```py
iface = Interface(board)
print("UID: ", iface.wait_for_card().hex())
print ("Dumping all sectors")
for i, sector in enumerate(iface.read_full()):
  print("Sector {}".format(i))
  for j,block in enumerate(sector):
    if block:
      print(" Block {}: {}".format(j, block.hex(sep=' ')))
```

## Interface

```py
read_full()                ## Will read the every sector and return list[list[bytes]] == sectors[blocks[bytes]
read_sector(sector)        ## Will read fully a specified sector and return list[bytes] == blocks[bytes]
read_block(sector, block)  ## Will read just a specified block and return its bytes 
```
