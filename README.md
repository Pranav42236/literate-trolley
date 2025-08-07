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
Step-by-Step Instructions for Uploading and Running the System
Install Arduino IDE
Download and install the Arduino IDE from arduino.cc.

Configure ESP32 Board Support
Follow the official guide to add ESP32 board support to Arduino:

Go to File > Preferences, add this URL to "Additional Boards Manager URLs":
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Open Tools > Board > Board Manager, search for "ESP32" and install.

Install Required Libraries
Use Arduino Library Manager (Sketch > Include Library > Manage Libraries) to install:

MFRC522 by Miguel Balboa

WebSocketsServer library (arduinoWebSockets)

Wire the Hardware
Connect the MFRC522 RFID module to ESP32 according to the wiring table above.

Configure Code

Open smart_trolley.ino in Arduino IDE.

Modify WiFi SSID and password in the code (ssid and password variables).

Update validUIDs array with your RFID tag UIDs if different.

Select Board and Port
In Arduino IDE, go to Tools > Board and select your ESP32 model.
Select the correct COM port for your device under Tools > Port.

Upload the Code
Click the upload button. Monitor serial output (115200 baud) to see WiFi connection status and IP address.

Access the Interface
From a browser on the same WiFi network, navigate to the ESP32’s IP address (shown in serial monitor).

Guide for Scanning RFID Tags and Using the Web Interface
Scanning RFID Tags:
Place RFID tags near the MFRC522 reader antenna. If the tag UID matches an entry in validUIDs, the corresponding product is added to the cart; scanning the same tag again removes that product from the cart.

Web Interface Pages:

Home ("/"): View your current cart with product names, quantities, and prices. Uses WebSocket to show live updates.

Payment Summary ("/pay"): Shows itemized bill and total amount. Button to simulate payment.

Payment Success ("/success"): Confirmation page after payment; clears the cart.

Supermarket Map ("/map"): Visual grid showing different store sections.

Entertainment ("/entertainment"): Links to curated articles, music, and videos related to shopping.

Settings ("/settings"): Simulated controls for WiFi, RFID scanner status, system reset, brightness, alerts, and language preferences.

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

How to Extend or Customize the Project
Add More Products: Extend products array and add matching RFID UIDs.

Real Payment Integration: Connect to payment APIs or services.

Persistent Storage: Save cart state in EEPROM or SPIFFS during power cycles.

Settings Functionality: Implement real configuration changes (e.g., WiFi update, brightness control).

Security: Implement authentication on web interface, enable HTTPS and secure WebSocket (WSS).

Dynamic Catalog: Load products and prices from external JSON or database.

Additional Sensors: Add weight sensors or barcode scanners for enhanced cart verification.

**********************

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
