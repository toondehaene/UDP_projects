/*

  WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using a WiFi shield.

 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 Circuit:

 * WiFi shield attached

 created 30 December 2012

 by dlf (Metodo2 srl)

 */

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "SparkFunLSM6DS3.h"
#include "Wire.h"

//wifi
int status = WL_IDLE_STATUS;
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "Christophe";        // your network SSID (name)
char pass[] = "visgraat";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 20777;      // local port to listen on
char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[50];       // a string to send back
WiFiUDP Udp;

//accelero 

LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
double k_coeff = 0.9;
double theta = 0.0;
double delta_t = 0; //ms since last update to theta
double timer = micros(); //absolute time of last update to theta
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  acceleroSetup();
  //Initialize serial and wait for port to open:
  wifiSetup();
 }

void loop() {

  arduinoloop();
  // if there's data available, read a packet

  int packetSize = Udp.parsePacket();
  if (packetSize)

  {

    Serial.print("Received packet of size ");

    Serial.println(packetSize);

    Serial.print("From ");

    IPAddress remoteIp = Udp.remoteIP();

    Serial.print(remoteIp);

    Serial.print(", port ");

    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer

    int len = Udp.read(packetBuffer, 255);

    if (len > 0) packetBuffer[len] = 0;

    Serial.println("Contents:");

    Serial.println(packetBuffer);

//    // send a reply, to the IP address and port that sent us the packet we received
//    if(isDigit(Udp.remotePort()))
//    {
//      Serial.println("is a number");
//    }
//    else
//    {
//      Serial.println("is not a number");
//    }
    
//    Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
    Udp.beginPacket(Udp.remoteIP(),(uint16_t) 20666);

    String(theta,5).toCharArray(ReplyBuffer,50);
    Udp.write(ReplyBuffer);
    Serial.println(String(theta,5));
    Udp.endPacket();  

  }
}

void printWiFiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}

void acceleroSetup(){
  
    //Call .begin() to configure the IMUs
    if (myIMU.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }
}

void wifiSetup(){
   
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");

    // don't continue:

    while (true){
      Serial.println("Wifi shield not present");
    }
  }

  // attempt to connect to WiFi network:

  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(5000);
    
    

  }

  Serial.println("Connected to wifi");

  printWiFiStatus();

  Serial.println("\nStarting connection to server...");

  // if you get a connection, report back via serial:

  Udp.begin(localPort);

}


void arduinoloop(){
    // angle calculation
    
    delta_t = micros()-timer;
    timer = micros();
    theta = ComplementaryFilter(
      theta,
      myIMU.readFloatAccelX(),
      myIMU.readFloatAccelY(),
      myIMU.readFloatGyroZ()*70.0 / 1000000, //this is in degrees per millisecond, sensitivity should be 70mdps got this from library
      delta_t
      );

//    Serial.print("Currently calculated theta= ");
//    Serial.println(theta);
  
  }


double ComplementaryFilter(float theta, float ax, float ay, float gz, int delta_t) {
//  Serial.println(theta);
//  Serial.println(ax);
//  Serial.println(ay);
//  Serial.println(gz);
//  Serial.println(delta_t);
  
  double phi = atan2(ay, ax)*180/M_PI;
  double w = gz;
  return k_coeff*(theta+ w*(delta_t/1000)) + (1-k_coeff)*phi;
}
