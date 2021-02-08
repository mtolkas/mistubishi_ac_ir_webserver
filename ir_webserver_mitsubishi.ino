// WiFi and server stuff
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// IR stuff
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>

const uint16_t kIrLed = 4; // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRMitsubishiAC ac(kIrLed); // Set the GPIO used for sending messages.

ESP8266WebServer server(80);

void printState()
{
  // Display the settings.
  Serial.println("Mitsubishi A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  
  // Display the encoded IR sequence.
  unsigned char *ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kMitsubishiACStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}

const char *ssid = "<SSID>";       //Enter Wi-Fi SSID
const char *password = "<PASS>"; //Enter Wi-Fi Password

void setup()
{
  // Set up hardware to be able to send a message.
  ac.begin();
  Serial.begin(115200);
  delay(200);
  Serial.println("Default state of the remote.");
  printState();

  // Connect
  WiFi.begin(ssid, password); //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED)
  { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect...");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Print the local IP

  server.on("/", handle_index);

  server.on("/handle_ac", handle_ac);

  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop()
{
  server.handleClient(); //Handling of incoming client requests
}

void handle_ac()
{
  String power_state = server.arg("power_state");
  String mode = server.arg("mode");
  String temperature = server.arg("temperature");
  String fan = server.arg("fan");
  String swing_v = server.arg("swing_v"); // https://github.com/crankyoldgit/IRremoteESP8266/issues/1399#issuecomment-771450305

  Serial.print("power_state: ");
  Serial.println(power_state);
  Serial.print("mode: ");
  Serial.println(mode);
  Serial.print("temperature: ");
  Serial.println(temperature);
  Serial.print("fan: ");
  Serial.println(fan);
  Serial.print("swing_v: ");
  Serial.println(swing_v);

  // Set up what we want to send. See ir_Mitsubishi.cpp for all the options.
  printState();
  Serial.println("Setting desired state for A/C.");

  // POWER STATE
  if (power_state == "on")
  {
    ac.on();
  }
  else if (power_state == "off")
  {
    ac.off();
  }
  else
  {
    ac.off();
  }

  // MODE
  if (mode == "dry")
  {
    ac.setMode(kMitsubishiAcDry);
  }
  else if (mode == "cool")
  {
    ac.setMode(kMitsubishiAcCool);
  }
  else
  {
    ac.setMode(kMitsubishiAcDry);
  }
  
  // TEMPERATURE
  if (temperature.isEmpty())
  {
    Serial.print("No temp, setting 27");
    ac.setTemp(27);
  } else
  {
    ac.setTemp(temperature.toInt());
  }
  
  // FAN
  if (fan == "silent")
  {
    ac.setFan(kMitsubishiAcFanSilent);
  }
  else if (fan == "quiet")
  {
    ac.setFan(kMitsubishiAcFanQuiet);
  }
  else if (fan == "max")
  {
    ac.setFan(kMitsubishiAcFanMax);
  }
  else if (fan == "real_max")
  {
    ac.setFan(kMitsubishiAcFanRealMax);
  }
  else
  {
    ac.setFan(kMitsubishiAcFanSilent);
  }  

  // VANE
  if (swing_v == "lowest")
  {
    ac.setVane(kMitsubishi112SwingVLowest);
  }
  else if (swing_v == "highest")
  {
    ac.setVane(kMitsubishi112SwingVHighest);
  }
  else if (swing_v == "auto")
  {
    ac.setVane(kMitsubishi112SwingVAuto);
  }
  else
  {
    ac.setVane(kMitsubishi112SwingVLowest);
  }  

  Serial.println("Setting State");
  printState();

  ac.send();
  server.send(200, "text/plain", "Done.");
}

void handle_index()
{
  //Print Hello at opening homepage
  server.send(200, "text/plain", "Welcome to the IR server!");
}
