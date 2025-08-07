  #include <WiFi.h>
  #include <WebServer.h>
  #include <SPI.h>
  #include <MFRC522.h>
  #include <vector>
  #include <WebSocketsServer.h>

  #define SS_PIN 15
  #define RST_PIN 5

  const char* ssid = "Hey";
  const char* password = "hey12345";

  WebServer server(80);
  WebSocketsServer webSocket = WebSocketsServer(81); 
  MFRC522 mfrc522(SS_PIN, RST_PIN);

  struct Product {
      String name;
      float price;
  };

  Product products[] = {
      {"Bottle", 50.0}, {"Diary", 80.0}, {"Bread", 120.0},
      {"Box", 180.0}, {"Cake", 60.0}, {"Pouch", 45.0}
  };

  String validUIDs[] = {"636C87E2", "03BD4EE2", "05CFFA00", "76534102", "230957E2", "03F6F7E1"};

  std::vector<int> cart;
  float totalAmount = 0.0;

  // Function Prototypes
  void handleRoot();
  void handlePay();
  void handleSuccess();
  void handleMap();
  void handleEntertainment();
  void handleSettings();
  void scanRFID();
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length); // WebSocket event handler
  void sendCartUpdate(); // Function to send cart updates to clients

  void setup() {
      Serial.begin(115200);
      Serial.println("Initializing...");
      SPI.begin();
      mfrc522.PCD_Init();
      Serial.println("RFID Scanner Ready");
      WiFi.begin(ssid, password);
      Serial.print("Connecting to WiFi...");
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      }
      Serial.println("\nConnected to WiFi");
      Serial.println(WiFi.localIP());
      
      // Initialize WebSocket server
      webSocket.begin();
      webSocket.onEvent(webSocketEvent);
      Serial.println("WebSocket server started on port 81");
      
      server.on("/", handleRoot);
      server.on("/pay", handlePay);
      server.on("/success", handleSuccess);
      server.on("/map", handleMap);
      server.on("/entertainment", handleEntertainment);
      server.on("/settings", handleSettings);
      server.begin();
      Serial.println("Web Server Started");
  }

  void loop() {
      server.handleClient();
      webSocket.loop(); // Handle WebSocket events
      scanRFID();
  }

  // WebSocket event handler
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
      switch(type) {
          case WStype_DISCONNECTED:
              Serial.printf("[%u] Disconnected!\n", num);
              break;
          case WStype_CONNECTED:
              {
                  IPAddress ip = webSocket.remoteIP(num);
                  Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
                  
                  // Send current cart data to newly connected client
                  sendCartUpdate();
              }
              break;
          case WStype_TEXT:
              Serial.printf("[%u] Received text: %s\n", num, payload);
              // Handle incoming WebSocket messages if needed
              break;
      }
  }

  // Function to send cart updates to all connected WebSocket clients
  void sendCartUpdate() {
      String json = "{\"cart\":[";
      std::vector<int> productCount(7, 0);
      
      for (int i : cart) {
          productCount[i]++;
      }
      
      bool first = true;
      for (int i = 0; i < 7; i++) {
          if (productCount[i] > 0) {
              if (!first) json += ",";
              json += "{\"name\":\"" + products[i].name + "\",\"quantity\":" + String(productCount[i]) + ",\"price\":" + String(products[i].price) + "}";
              first = false;
          }
      }
      
      json += "],\"total\":" + String(totalAmount, 2) + "}";
      webSocket.broadcastTXT(json);
  }

  void scanRFID() {
      if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;
      String tagID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
          tagID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");  
          tagID += String(mfrc522.uid.uidByte[i], HEX);
      }
      tagID.toUpperCase();
      Serial.println("Scanned RFID: " + tagID);
      for (int i = 0; i < 7; i++) {
          if (tagID == validUIDs[i]) {
              if (!cart.empty() && cart.back() == i) {
                  Serial.println("Confirm removal of: " + products[i].name);
                  cart.pop_back();
                  totalAmount -= products[i].price;
                  Serial.println(products[i].name + " removed. New total: Rs. " + String(totalAmount));
              } else {
                  cart.push_back(i);
                  totalAmount += products[i].price;
                  Serial.println(products[i].name + " added. New total: Rs. " + String(totalAmount));
              }
              // Send cart update via WebSocket after item is added/removed
              sendCartUpdate();
              break;
          }
      }
      delay(1000);
  }

  void handleEntertainment() {
      Serial.println("Serving Entertainment Page...");
      String html = getHeaderAndMenu();
      html += "<h2>Entertainment</h2>";
      html += "<p>Explore some interesting articles, music, and videos while you shop:</p>";
      html += "<table style='width: 100%; border-collapse: collapse; margin: auto;'>"
              "<tr><th style='padding: 12px; text-align: center; border: 1px solid #555; background-color: #333;'>Title</th>"
              "<th style='padding: 12px; text-align: center; border: 1px solid #555; background-color: #333;'>Category</th>"
              "<th style='padding: 12px; text-align: center; border: 1px solid #555; background-color: #333;'>Link</th></tr>";

      // Articles
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>How Smart Shopping Carts Are Changing Retail</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Article</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.theguardian.com/technology/2023/jul/10/how-smart-shopping-carts-are-changing-the-way-we-shop' target='_blank'>Read More</a></td></tr>";
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>The Future of Retail: Smart Carts</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Article</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.forbes.com/sites/johnkoetsier/2023/06/22/the-future-of-retail-is-here-smart-carts-in-1000-stores/' target='_blank'>Read More</a></td></tr>";

      // Music
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>Relaxing Music for Shopping</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Music</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.youtube.com/watch?v=5qap5aO4i9A' target='_blank'>Listen Now</a></td></tr>";
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>Upbeat Shopping Playlist</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Music</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.youtube.com/watch?v=J2X5mJ3HDYE' target='_blank'>Listen Now</a></td></tr>";

      // Videos
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>Smart Cart Technology Explained</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Video</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.youtube.com/watch?v=dQw4w9WgXcQ' target='_blank'>Watch Now</a></td></tr>";
      html += "<tr><td style='padding: 12px; text-align: center; border: 1px solid #555;'>The Future of AI in Retail</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'>Video</td>"
              "<td style='padding: 12px; text-align: center; border: 1px solid #555;'><a href='https://www.youtube.com/watch?v=Ly8F2mTJd8M' target='_blank'>Watch Now</a></td></tr>";

      html += "</table>";
      html += "<button onclick='location.href=\"/\"' style='margin-top: 20px;'>Back to Home</button>";
      html += "</div></body></html>";
      server.send(200, "text/html", html);
  }

  void handleSettings() {
      Serial.println("Serving Settings Page...");
      String html = getHeaderAndMenu();
      html += "<h2>Settings</h2>";
      html += "<p>Configure your smart trolley settings:</p>";

      // List Format for Settings
      html += "<div style='max-width: 600px; margin: auto; background: #333; padding: 20px; border-radius: 10px;'>";

      // Wi-Fi Configuration
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>Wi-Fi Configuration</h3>";
      html += "<p>Change Wi-Fi network settings.</p>";
      html += "<button onclick='alert(\"Wi-Fi settings updated!\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Update</button>";
      html += "</div>";

      // RFID Scanner Status
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>RFID Scanner</h3>";
      html += "<p>Check the status of the RFID scanner.</p>";
      html += "<button onclick='alert(\"RFID Scanner is active.\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Check Status</button>";
      html += "</div>";

      // System Reset
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>System Reset</h3>";
      html += "<p>Reset the trolley system to default settings.</p>";
      html += "<button onclick='alert(\"System reset complete!\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Reset</button>";
      html += "</div>";

      // Display Brightness
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>Display Brightness</h3>";
      html += "<p>Adjust the brightness of the display.</p>";
      html += "<button onclick='alert(\"Brightness set to 75%.\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Set Brightness</button>";
      html += "</div>";

      // Audio Alerts
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>Audio Alerts</h3>";
      html += "<p>Enable or disable audio alerts for notifications.</p>";
      html += "<button onclick='alert(\"Audio alerts enabled.\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Enable Alerts</button>";
      html += "</div>";

      // Language Preferences
      html += "<div style='margin-bottom: 15px;'>";
      html += "<h3>Language Preferences</h3>";
      html += "<p>Change the language of the interface.</p>";
      html += "<button onclick='alert(\"Language set to English.\")' style='padding: 10px 20px; background-color: #FF5722; color: white; border: none; border-radius: 5px; cursor: pointer;'>Set Language</button>";
      html += "</div>";

      html += "</div>"; // Close the container div
      html += "<button onclick='location.href=\"/\"' style='margin-top: 20px;'>Back to Home</button>";
      html += "</div></body></html>";
      server.send(200, "text/html", html);
  }

  String getHeaderAndMenu() {
      return "<html><head><title>Smart Trolley</title>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<style>body { font-family: Arial, sans-serif; background-color: #181818; color: #FFF; text-align: center; }"
            "nav { background-color: #222; padding: 15px; display: flex; justify-content: center; gap: 20px; }"
            "nav a { color: white; text-decoration: none; font-size: 18px; padding: 10px; }"
            "nav a:hover { background-color: #FF5722; border-radius: 5px; padding: 10px; }"
            ".container { max-width: 90%; margin: auto; padding: 20px; background: #222; border-radius: 10px; }"
            "table { width: 100%; border-collapse: collapse; margin: auto; }"
            "th, td { padding: 12px; text-align: center; border: 1px solid #555; font-size: 18px; }"
            "th { background-color: #333; color: white; }"
            "button { padding: 15px 30px; font-size: 18px; background-color: #FF5722; color: white; border: none; cursor: pointer; border-radius: 5px; margin-top: 20px; }"
            "@media screen and (max-width: 1084px) { table { font-size: 16px; } th, td { padding: 10px; } }"
            "</style>"
            // Add WebSocket JavaScript
            "<script>"
            "let socket;"
            "function initWebSocket() {"
            "  socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
            "  socket.onopen = function(e) { console.log('WebSocket connected'); };"
            "  socket.onclose = function(e) { console.log('WebSocket disconnected'); setTimeout(initWebSocket, 2000); };"
            "  socket.onmessage = function(e) { handleWebSocketMessage(e); };"
            "}"
            "function handleWebSocketMessage(event) {"
            "  const data = JSON.parse(event.data);"
            "  if (window.location.pathname === '/') {"
            "    updateCart(data);"
            "  }"
            "}"
            "function updateCart(data) {"
            "  const cartTable = document.getElementById('cartTable');"
            "  if (cartTable) {"
            "    let tableHTML = '<tr><th>Product</th><th>Quantity</th><th>Price (Rs.)</th></tr>';"
            "    data.cart.forEach(item => {"
            "      tableHTML += `<tr><td>${item.name}</td><td>${item.quantity}</td><td>Rs. ${item.price}</td></tr>`;"
            "    });"
            "    cartTable.innerHTML = tableHTML;"
            "    document.getElementById('totalAmount').innerText = `Total: Rs. ${data.total}`;"
            "  }"
            "}"
            "window.onload = function() {"
            "  initWebSocket();"
            "};"
            "</script>"
            "</head><body>"
            "<h2>Welcome to Smart Shopping Trolley</h2>"
            "<nav><a href='/'>Shopping List</a> | <a href='/map'>Supermarket Map</a> | <a href='/entertainment'>Entertainment</a> | <a href='/settings'>Settings</a></nav>"
            "<div class='container'>";
  }

  void handleRoot() {
      Serial.println("Serving Root Page...");
      String html = getHeaderAndMenu();
      // Remove auto-reload since WebSocket will update the cart
      html += "<h2>Your Cart</h2><table id='cartTable'><tr><th>Product</th><th>Quantity</th><th>Price (Rs.)</th></tr>";
      std::vector<int> productCount(7, 0);
      for (int i : cart) {
          productCount[i]++;
      }
      for (int i = 0; i < 7; i++) {
          if (productCount[i] > 0) {
              html += "<tr><td>" + products[i].name + "</td><td>" + String(productCount[i]) + "</td><td>Rs. " + String(products[i].price) + "</td></tr>";
          }
      }
      html += "</table><h3 id='totalAmount'>Total: Rs. " + String(totalAmount, 2) + "</h3>";
      html += "<button onclick='location.href=\"/pay\"'>Pay Now</button>";
      html += "</div></body></html>";
      server.send(200, "text/html", html);
  }

  void handlePay() {
      Serial.println("Serving Payment Page...");
      String html = getHeaderAndMenu();
      html += "<h2>Bill Summary</h2><div class='container' style='max-width: 400px; margin: auto;'>"
              "<table><tr><th>Item</th><th>Qty</th><th>Price</th></tr>";
      std::vector<int> productCount(7, 0);
      for (int i : cart) {
          productCount[i]++;
      }
      for (int i = 0; i < 7; i++) {
          if (productCount[i] > 0) {
              html += "<tr><td>" + products[i].name + "</td><td>" + String(productCount[i]) + "</td><td>Rs. " + String(products[i].price * productCount[i]) + "</td></tr>";
          }
      }
      html += "<tr><td colspan='2'>Total:</td><td>Rs. " + String(totalAmount, 2) + "</td></tr>";
      html += "</table><button onclick='location.href=\"/success\"'>Proceed to Payment</button></div></body></html>";
      server.send(200, "text/html", html);
  }

  void handleSuccess() {
      Serial.println("Serving Payment Success Page...");
      cart.clear();
      totalAmount = 0.0;
      // Send empty cart update via WebSocket
      sendCartUpdate();
      String html = getHeaderAndMenu();
      html += "<h2 style='color: green;'>Payment Successful!</h2>"
              "<p>Thank you for shopping with us.</p>"
              "<button onclick='location.href=\"/\"'>Return to Home</button>"
              "</div></body></html>";
      server.send(200, "text/html", html);
  }

  void handleMap() {
      Serial.println("Serving Supermarket Map Page...");
      String html = getHeaderAndMenu();
      html += "<!DOCTYPE html><html lang='en'><head>"
              "<meta charset='UTF-8'>"
              "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
              "<style>"
              "  body { font-family: Arial, sans-serif; text-align: center; background: #1a1a1a; color: white; }"
              "  .container {"
              "    display: grid;"
              "    grid-template-columns: repeat(3, 1fr);"
              "    gap: 15px;"
              "    max-width: 600px;"
              "    margin: auto;"
              "  }"
              "  .box {"
              "    background: #333; padding: 15px; border-radius: 8px; color: white;"
              "    font-size: 18px; font-weight: bold;"
              "  }"
              "  .arrow { font-size: 24px; color: #ffcc00; padding-top: 10px; }"
              "  .btn {"
              "    padding: 10px 20px; background: #ff5722; color: white;"
              "    border: none; border-radius: 8px; cursor: pointer;"
              "    font-size: 16px; display: inline-block; width: auto; margin-top: 20px;"
              "  }" 
              "</style></head><body>";
      html += "<h2>Supermarket Map</h2>"
              "<div class='container'>"
              "<div class='box'> Entrance</div> <div class='box'> Fruits & Vegetables</div> <div class='box'>Bakery</div>"
              "<div class='box'> Cereal & Snacks</div> <div class='box'> Drinks & Coffee</div> <div class='box'>Frozen Foods & Meat</div>"
              "<div class='box'> Dairy & Eggs</div> <div class='box'>Meat & Seafood</div> <div class='box'>Pantry & Canned Foods</div>"
              "<div class='box'> Cleaning Supplies</div> <div class='box'>Personal Care & Beauty</div> <div class='box'>Checkout Area</div>"
              "</div>"
              "<br><button class='btn' onclick='location.href=\"/\"'> Back to Home</button>"
              "</body></html>";
      server.send(200, "text/html", html);
  }
