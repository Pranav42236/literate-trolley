# literate-trolley
The project is a smart shopping trolley system using an ESP32 microcontroller and an RFID reader. It connects to WiFi and runs a web server with WebSocket support to provide a real-time, interactive shopping cart interface accessible via a browser. Users add or remove items by scanning RFID tags mapped to specific products. The web interface displays the cart, payment summary, supermarket map, entertainment content, and settings. The system updates the cart live via WebSocket without page reloads.

To use it, wire the ESP32 with the RFID module, update WiFi credentials and RFID tag UIDs, then upload the code via Arduino IDE. Access the ESP32 IP in a browser to interact with the trolley.


*****
Hardware Components Required

* ESP32 wroom development board 
* MFRC522 RFID module
* RFID tags or cards
* Jumper wires
* USB cable to program ESP32
* batteries
* display
* Access to a WiFi network

********
Wiring Diagram and Pin Configuration

MFRC522 Pin	   ESP32 Pin (GPIO)  	Description
SDA (SS)	     15	                Slave Select (SS)
SCK            18	                SPI Clock
MOSI	         23               	SPI MOSI
MISO	         19	                SPI MISO
RST	           5	                Reset
3.3V	         3.3V	              Power Supply
GND            GND	              Ground

(Note: ESP32 SPI pins - SCK=18, MOSI=23, MISO=19 are commonly used defaults but can be changed if needed)

************


Common Issues and Troubleshooting
ESP32 Not Connecting to WiFi
Check SSID and password. Ensure WiFi network is operational and in range.

RFID Reader Not Detecting Tags
Verify wiring; check antenna orientation. Use serial monitor to see scan attempts.

Web Pages Not Loading
Confirm ESP32 IP from serial monitor is correct and device is connected to the same WiFi network as your browser.

WebSocket Updates Not Working
Ensure WebSocket port (81) is open and accessible. Check browser console for errors.

Payment Not Processing
Current code simulates payment; no real transaction occurs.
