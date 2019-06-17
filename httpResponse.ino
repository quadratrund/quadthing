#include "httpResponse.h"

HttpResponse::HttpResponse(): statusCode(200) {
}

HttpResponse::HttpResponse(unsigned int statusCode): statusCode(statusCode) {
}

void HttpResponse::addHeader(String name, String value) {
  this->headers += name;
  this->headers += ": ";
  this->headers += value;
  this->headers += "\r\n";
}

String HttpResponse::toString() {
  String retval = "HTTP/1.1 ";
  retval += String(this->statusCode, DEC);
  switch (this->statusCode) {
    case 200:
      retval += " OK\r\n";
      break;
    case 303:
      retval += " See Other\r\n";
      break;
    case 401:
      retval += " Unauthorized\r\n";
      break;
    case 404:
      retval += " Not Found\r\n";
      break;
    default:
      retval += " Unknown Code\r\n";
      break;
  }
  retval += this->headers;
  retval += "\r\n";
  retval += this->content;
  return retval;
}

size_t HttpResponse::write(uint8_t c) {
  this->content += (char)c;
  return 1;
}
