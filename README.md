# literate-trolley
The project is a smart shopping trolley system using an ESP32 microcontroller and an RFID reader. It connects to WiFi and runs a web server with WebSocket support to provide a real-time, interactive shopping cart interface accessible via a browser. Users add or remove items by scanning RFID tags mapped to specific products. The web interface displays the cart, payment summary, supermarket map, entertainment content, and settings. The system updates the cart live via WebSocket without page reloads.

To use it, wire the ESP32 with the RFID module, update WiFi credentials and RFID tag UIDs, then upload the code via Arduino IDE. Access the ESP32 IP in a browser to interact with the trolley.
