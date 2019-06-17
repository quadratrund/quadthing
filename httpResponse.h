#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <Arduino.h>

class HttpResponse : public Print {
  private:
    String headers;
  public:
    HttpResponse();
    HttpResponse(unsigned int statusCode);
    unsigned int statusCode;
    String content;

    void addHeader(String name, String value);
    String toString();

    size_t write(uint8_t);
};

#endif
