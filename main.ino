/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <Hash.h>
#include <EEPROM.h>
#include "Pins.h"
#include "config.h"
#include "httpStatus.h"
#include "switch.h"

// The certificate is stored in PMEM
static const uint8_t x509[] PROGMEM = {
  0x30, 0x82, 0x01, 0x33, 0x30, 0x81, 0xde, 0x02, 0x09, 0x00, 0xcb, 0xab,
  0xa5, 0x1d, 0xbf, 0xe3, 0x5a, 0xc4, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x20, 0x31,
  0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x51, 0x75,
  0x61, 0x64, 0x54, 0x68, 0x69, 0x6e, 0x67, 0x31, 0x0a, 0x30, 0x08, 0x06,
  0x03, 0x55, 0x04, 0x03, 0x0c, 0x01, 0x2a, 0x30, 0x20, 0x17, 0x0d, 0x31,
  0x39, 0x30, 0x36, 0x31, 0x31, 0x31, 0x38, 0x31, 0x37, 0x32, 0x32, 0x5a,
  0x18, 0x0f, 0x32, 0x31, 0x31, 0x39, 0x30, 0x38, 0x32, 0x36, 0x31, 0x38,
  0x31, 0x37, 0x32, 0x32, 0x5a, 0x30, 0x20, 0x31, 0x12, 0x30, 0x10, 0x06,
  0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x51, 0x75, 0x61, 0x64, 0x54, 0x68,
  0x69, 0x6e, 0x67, 0x31, 0x0a, 0x30, 0x08, 0x06, 0x03, 0x55, 0x04, 0x03,
  0x0c, 0x01, 0x2a, 0x30, 0x5c, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
  0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x4b, 0x00, 0x30,
  0x48, 0x02, 0x41, 0x00, 0xc0, 0x48, 0x0e, 0xd7, 0xbc, 0x8b, 0x54, 0x09,
  0x99, 0xe9, 0xb3, 0x92, 0x60, 0x7d, 0x97, 0x7f, 0x52, 0x94, 0x87, 0x6d,
  0xd4, 0x75, 0x9f, 0x15, 0x5e, 0x15, 0xe1, 0xb7, 0xfc, 0x0c, 0xdf, 0x84,
  0x3d, 0x18, 0xba, 0x7e, 0xfc, 0xe4, 0x4f, 0x47, 0x3e, 0x20, 0x6d, 0x1e,
  0x06, 0x07, 0xdc, 0xef, 0x35, 0x8e, 0xba, 0xd9, 0xae, 0xe0, 0xe1, 0x34,
  0xb8, 0xbf, 0x09, 0xe1, 0x5a, 0x88, 0xc8, 0x05, 0x02, 0x03, 0x01, 0x00,
  0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
  0x01, 0x0b, 0x05, 0x00, 0x03, 0x41, 0x00, 0x70, 0x34, 0x1f, 0xa3, 0x18,
  0x53, 0x35, 0x81, 0x88, 0x2b, 0xc9, 0xd6, 0x77, 0x37, 0xb1, 0x68, 0xdf,
  0x2d, 0x90, 0x47, 0xc6, 0x9b, 0xc4, 0x94, 0xcd, 0x52, 0x4e, 0xeb, 0x99,
  0x1f, 0x71, 0x31, 0xf1, 0x81, 0x6e, 0xb0, 0xf4, 0x8a, 0x0c, 0xd7, 0xe5,
  0xee, 0xd6, 0xd1, 0x12, 0xdd, 0xef, 0xaf, 0x51, 0x0e, 0x09, 0x13, 0xc3,
  0x41, 0x88, 0xfe, 0x1f, 0x8f, 0x13, 0xdf, 0xf0, 0xe5, 0x3c, 0xbc
};

// And so is the key.  These could also be in DRAM
static const uint8_t rsakey[] PROGMEM = {
  0x30, 0x82, 0x01, 0x3a, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xc0, 0x48,
  0x0e, 0xd7, 0xbc, 0x8b, 0x54, 0x09, 0x99, 0xe9, 0xb3, 0x92, 0x60, 0x7d,
  0x97, 0x7f, 0x52, 0x94, 0x87, 0x6d, 0xd4, 0x75, 0x9f, 0x15, 0x5e, 0x15,
  0xe1, 0xb7, 0xfc, 0x0c, 0xdf, 0x84, 0x3d, 0x18, 0xba, 0x7e, 0xfc, 0xe4,
  0x4f, 0x47, 0x3e, 0x20, 0x6d, 0x1e, 0x06, 0x07, 0xdc, 0xef, 0x35, 0x8e,
  0xba, 0xd9, 0xae, 0xe0, 0xe1, 0x34, 0xb8, 0xbf, 0x09, 0xe1, 0x5a, 0x88,
  0xc8, 0x05, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x40, 0x58, 0xc8, 0xac,
  0xf9, 0xd6, 0x98, 0x6a, 0xfc, 0x67, 0xf2, 0x2e, 0x35, 0x77, 0x92, 0x66,
  0x42, 0xa7, 0x29, 0xe8, 0x02, 0xc1, 0xa8, 0x72, 0x05, 0x56, 0xc5, 0x39,
  0xcd, 0x4f, 0x77, 0xce, 0x38, 0x9f, 0x09, 0x0a, 0xad, 0x17, 0x14, 0xe9,
  0x96, 0x3f, 0x0b, 0x35, 0xc2, 0xd5, 0xe4, 0x31, 0xe0, 0x16, 0xe4, 0xba,
  0xba, 0x75, 0x0c, 0x9a, 0xb0, 0xbf, 0xb8, 0x7f, 0x97, 0xfa, 0x84, 0x50,
  0x61, 0x02, 0x21, 0x00, 0xec, 0x05, 0xd9, 0x3e, 0x17, 0x04, 0x95, 0x15,
  0x9c, 0xd2, 0x8b, 0xd7, 0x07, 0x53, 0x4b, 0xb0, 0x61, 0x04, 0x59, 0x8f,
  0x94, 0x20, 0x44, 0x4e, 0x09, 0x18, 0x45, 0xd0, 0xa6, 0x7e, 0x3a, 0x9d,
  0x02, 0x21, 0x00, 0xd0, 0x8e, 0x6b, 0x79, 0xd9, 0xd5, 0xf1, 0x34, 0xc7,
  0xbd, 0xcb, 0xa3, 0x0e, 0x84, 0x2d, 0xfe, 0x51, 0x79, 0xb5, 0x87, 0xa8,
  0xce, 0xc6, 0xd5, 0x41, 0x6f, 0x98, 0xe8, 0x79, 0xef, 0xf2, 0x89, 0x02,
  0x20, 0x0a, 0xc4, 0xa1, 0x8a, 0xb1, 0xa6, 0x25, 0xdf, 0x1c, 0x8e, 0x86,
  0x8e, 0xc2, 0xe8, 0xcb, 0xf5, 0x3a, 0x17, 0xe6, 0xb6, 0x24, 0x3d, 0xec,
  0x9f, 0xda, 0xce, 0x5e, 0xb1, 0x0c, 0x47, 0x8a, 0x09, 0x02, 0x20, 0x23,
  0x0d, 0x7c, 0xe4, 0xeb, 0x06, 0xf9, 0x56, 0x7c, 0xa7, 0xda, 0xbd, 0x1a,
  0x28, 0x7f, 0x2e, 0x7a, 0x75, 0x31, 0x5c, 0x8e, 0xb3, 0xca, 0x59, 0x77,
  0x79, 0x11, 0x43, 0x29, 0x93, 0x79, 0xb9, 0x02, 0x21, 0x00, 0xdd, 0x3b,
  0x89, 0x40, 0xab, 0xfe, 0x7b, 0x11, 0x9b, 0xad, 0xb3, 0xa6, 0xa2, 0xb8,
  0x09, 0x2c, 0xad, 0x29, 0xfd, 0xd5, 0xd4, 0x7e, 0xe0, 0x29, 0xdd, 0xc7,
  0xbf, 0x4c, 0x49, 0xa1, 0x7f, 0xe4
};

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer http_server(80);
WiFiServerSecure https_server(443);


boolean restartFlag = false;
IPAddress myIP;

void setup() {
  int i;

  switch_setupInitOutputs();

  Serial.begin(9600);
  EEPROM.begin(512);

  Serial.println();
  Serial.println();

  if (analogRead(A0) > 1000) {
    config_reset();
  }

  config_read();

  switch_setupOutputsFromCfg();

  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);

  if (cfg_ssid[0]) {
    // Connect to WiFi network
    Serial.print(F("Connecting to "));
    Serial.println(cfg_ssid);
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(cfg_ssid, cfg_key);
    if ((uint32_t)cfg_ip) {
      WiFi.config(cfg_ip, cfg_gateway, cfg_netmask);
    }
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
      serial_loop();
    }
    Serial.println();
    Serial.println(F("WiFi connected"));
    myIP = WiFi.localIP();
  } else {
    Serial.println("Configuring access point...");
    WiFi.softAP("QuadThing");
    //WiFi.softAPConfig(ip, gateway, subnet);
    myIP = WiFi.softAPIP();
  }

  // Start the server
  http_server.begin();
  https_server.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
  https_server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.print("Server IP address: ");
  Serial.println(myIP);
}

void loop() {
  digitalWrite(D4, HIGH);
  if (restartFlag) {
    ESP.restart();
  }
  serial_loop();
  http_loop();
  https_loop();
  switch_loop();
}

void http_loop() {
  HttpResponse response;
  // Check if a client has connected
  WiFiClient client = http_server.available();
  if (!client) {
    return;
  }
  digitalWrite(D4, LOW);
  Serial.println(F("------------------------------HTTP"));
  Serial.println(client.remoteIP());

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  if (req.length() == 0) {
    Serial.println("empty request");
    return;
  }
  Serial.print(F("request: "));
  Serial.println(req);
  String path = req.substring(req.indexOf(' '), req.lastIndexOf(' '));
  path.trim();
  Serial.println(path);

  client.setTimeout(100);

  client.readString();

  response = http_redirect(myIP, path.c_str());
  client.print(response.toString());
}

void https_loop() {
  HttpResponse response;

  // Check if a client has connected
  WiFiClientSecure client = https_server.available();
  if (!client) {
    return;
  }
  digitalWrite(D4, LOW);
  Serial.println(F("------------------------------HTTPS"));
  Serial.println(client.remoteIP());

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  if (req.length() == 0) {
    Serial.println("empty request");
    return;
  }
  Serial.print(F("request: "));
  Serial.println(req);
  String path = req.substring(req.indexOf(' '), req.lastIndexOf(' '));
  String params;
  int i = path.indexOf("?");
  if (i != -1) {
    params = path.substring(i + 1);
    path.remove(i);
  }
  path.trim();
  Serial.println(path);
  Serial.println(params);

  client.setTimeout(100);

  String httpHeaders = client.readString();
  String postVars;
  i = httpHeaders.indexOf("\r\n\r\n");
  if (i != -1) {
    postVars = httpHeaders.substring(i+4);
    httpHeaders.remove(i);
  }
  Serial.println(postVars);
  String pass = getPassFromHeaders(httpHeaders);
  Serial.println(pass);
  Serial.println(sha1(pass));

  if (!checkPass(pass)) {
    response = http_askForLogin();
  } else if (path.equals("/")) {
    response = http_redirect(myIP, "/p/home");
  } else if (path.equals("/c/pass")) {
    config_setNewPass(postVars);
    response = http_redirect(myIP, "/p/pass");
  } else if (path.equals("/c/network")) {
    config_setNetwork(postVars);
    response = http_redirect(myIP, "/p/network");
    restartFlag = true;
  } else if (path.equals("/c/config")) {
    config_outputCount(postVars);
    response = http_redirect(myIP, "/p/config");
    restartFlag = true;
  } else if (path.equals("/p/network")) {
    response = pageNetwork();
  } else if (path.equals("/p/config")) {
    response = page_config();
  } else if (path.equals("/p/home")) {
    response = pageHome();
  } else if (path.equals("/p/pass")) {
    response = pagePass();
  } else if (path.equals("/restart")) {
    response = http_redirect(myIP, "/p/home");
    restartFlag = true;
  } else if (path.equals("/api/states")) {
    response = page_apiStates();
  } else if (path.equals("/api/switch")) {
    i = switch_multiOutputs(params);
    response = page_apiGenericResponse(String(i, DEC));
  } else if (path.equals("/css/global")) {
    response = page_css();
  } else if (path.equals("/switch")) {
    switch_outputs(params);
    response = http_redirect(myIP, "/p/home");
  } else {
    response = http_notFound();
  }
  client.print(response.toString());
  client.flush();
}
