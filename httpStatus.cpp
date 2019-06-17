#include <ESP8266WiFi.h>
#include "httpResponse.h"
#include "utils.h"

HttpResponse http_redirect(IPAddress ip, const char* url) {
  String location = "https://";
  location += ipToString(ip);
  location += String(url);
  HttpResponse response(303);
  response.addHeader("Location", location);
  Serial.print("Redirect to ");
  Serial.println(url);
  Serial.println(location);
  return response;
}

HttpResponse http_notFound() {
  return 404;
}

HttpResponse http_okHead() {
  HttpResponse response(200);
  response.addHeader("Content-Type", "text/html; charset=us-ascii");
  response.content = "<!DOCTYPE HTML>\r\n<html>\r\n";
  return response;
}

HttpResponse http_askForLogin() {
  HttpResponse response(401);
  response.addHeader("WWW-Authenticate", "Basic realm=\"Please login\"");
  return response;
}

HttpResponse http_ok() {
  return 200;
}
