#include <Arduino.h>
#include <ESP8266WiFi.h>

/* global variables for server connection */
const char *ssid = "TP-Link_5C54_asd";     // Enter SSID
const char *password = "$$$ 6#@ h4d [\\]"; // Enter Password

/* *********************************************************************************
  - To see what IP address the ESP has, run the command: nmap -sn 192.168.0.168/24
************************************************************************************ */

// int gpio4Value; 
// int gpio5Value;

// WiFiServer espServer(80); /* Instance of WiFiServer with port number 80 */
// /* 80 is the Port Number for HTTP Web Server */

// void setup() {

//   Serial.begin(115200); /* Begin Serial Communication with 115200 Baud Rate */
//   Serial.print("\n");
//   Serial.print("Connecting to: ");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA); /* Configure ESP8266 in STA Mode */
//   WiFi.begin(ssid, password); /* Connect to Wi-Fi based on above SSID and Password */

//   while(WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print("*");
//     delay(500);
//   }

//   Serial.print("\n");
//   Serial.print("Connected to Wi-Fi: ");
//   Serial.println(WiFi.SSID());
//   delay(100);

//   Serial.print("\n");
//   Serial.println("Starting ESP8266 Web Server...");

//   espServer.begin(); /* Start the HTTP web Server */

//   Serial.println("ESP8266 Web Server Started");
//   Serial.print("\n");
//   Serial.print("The URL of ESP8266 Web Server is: ");
//   Serial.print("http://");
//   Serial.println(WiFi.localIP());
//   Serial.print("\n");
//   Serial.println("Use the above URL in your Browser to access ESP8266 Web Server\n");
// }

// void loop() {
  
//   WiFiClient client = espServer.available(); /* Check if a client is available */
//   if(!client)
//   {
//     return;
//   }

//   Serial.println("New Client!!!");

//   String request = client.readStringUntil('\r'); /* Read the first line of the request from client */
//   Serial.println(request); /* Print the request on the Serial monitor */
//   /* The request is in the form of HTTP GET Method */ 
//   client.flush();

//   /* Based on the URL from the request, "turn the LEDs ON or OFF" */
//   if (request.indexOf("/GPIO4ON") != -1) 
//   {
//     Serial.println("GPIO4 LED is ON");
//     gpio4Value = HIGH;
//   } 
//   if (request.indexOf("/GPIO4OFF") != -1)
//   {
//     Serial.println("GPIO4 LED is OFF");
//     gpio4Value = LOW;
//   }
//   if (request.indexOf("/GPIO5ON") != -1) 
//   {
//     Serial.println("GPIO5 LED is ON");
//     gpio5Value = HIGH;
//   } 
//   if (request.indexOf("/GPIO5OFF") != -1)
//   {
//     Serial.println("GPIO5 LED is OFF");
//     gpio5Value = LOW;
//   }

//   /* HTTP Response in the form of HTML Web Page */
//   client.println("HTTP/1.1 200 OK");
//   client.println("Content-Type: text/html");
//   client.println(); //  IMPORTANT
//   client.println("<!DOCTYPE HTML>");
//   client.println("<html>");
//   client.println("<head>");
//   client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
//   client.println("<link rel=\"icon\" href=\"data:,\">");
//   /* CSS Styling for Buttons and Web Page */
//   client.println("<style>");
//   client.println("html { font-family: Courier New; display: inline-block; margin: 0px auto; text-align: center;}");
//   client.println(".button {border: none; color: white; padding: 10px 20px; text-align: center;");
//   client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
//   client.println(".button1 {background-color: #13B3F0;}");
//   client.println(".button2 {background-color: #3342FF;}");
//   client.println("</style>");
//   client.println("</head>");
  
//   /* The main body of the Web Page */
//   client.println("<body>");
//   client.println("<h2>ESP8266 Web Server</h2>");
  
//   if(gpio4Value == LOW) 
//   {
//     client.println("<p>GPIO4 LED Status: OFF</p>");
//     client.print("<p><a href=\"/GPIO4ON\"><button class=\"button button1\">Click to turn ON</button></a></p>");  
//   } 
//   else 
//   {
//     client.println("<p>GPIO4 LED Status: ON</p>");
//     client.print("<p><a href=\"/GPIO4OFF\"><button class=\"button button2\">Click to turn OFF</button></a></p>"); 
//   }
  
//   if(gpio5Value == LOW) 
//   {
//     client.println("<p>GPIO5 LED Status: OFF</p>");
//     client.print("<p><a href=\"/GPIO5ON\"><button class=\"button button1\">Click to turn ON</button></a></p>");  
//   } 
//   else 
//   {
//     client.println("<p>GPIO5 LED Status: ON</p>");
//     client.print("<p><a href=\"/GPIO5OFF\"><button class=\"button button2\">Click to turn OFF</button></a></p>");  
//   }
  
//   client.println("</body>");
//   client.println("</html>");
//   client.print("\n");
  
//   delay(1);
//   /* Close the connection */
//   client.stop();
//   Serial.println("Client disconnected");
//   Serial.print("\n");
// }

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}