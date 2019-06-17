#include <base64.hpp>

String getParamValue(String params, String paramName) {
  int i, j;
  i = -1;
  const char* paramBuffer = params.c_str();
  do {
    i++;
    for (j = i; paramBuffer[j] && paramBuffer[j] != '&' && paramBuffer[j] != '='; j++);
    if (paramBuffer[j] == '&' || !paramBuffer[j]) {
      i = j;
      continue;
    }
    String foundName = params.substring(i, j);
    for (i = j; paramBuffer[i] && paramBuffer[i] != '&'; i++);
    if (foundName.equals(paramName)) {
      j++;
      return urldecode(params.substring(j, i));
    }
  } while(paramBuffer[i]);
  return String();
}

boolean checkPass(String pass) {
  int i;
  uint8_t hash[20];
  sha1(pass, hash);
  for (i = 0; i < 20 && hash[i] == cfg_pwdhash[i]; i++);
  return i == 20;
}

void htmlspecialchars(String s) {
  s.replace("&",  "&amp;");
  s.replace("\"", "&quot;");
  s.replace("'",  "&apos;");
  s.replace("<",  "&lt;");
  s.replace(">",  "&gt;");
}

String urlencode(const char* msg) {
    const char *hex = "0123456789abcdef";
    String encodedMsg = "";

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urldecode(String str)
{
    
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

String getPassFromHeaders(String text) {
  text += "\r\n";
  int index = text.indexOf("Authorization: ");
  if (index == -1) {
    Serial.println(text);
    Serial.println("Authorization header not found");
    return "";
  }
  text = text.substring(index);
  index = text.indexOf("\r\n");
  text.remove(index);
  index = text.lastIndexOf(" ");
  text = text.substring(index + 1);
  Serial.println(text);
  unsigned int bufferSize = decode_base64_length((unsigned char*) text.c_str()) + 1;
  Serial.print("bufferSize=");
  Serial.println(bufferSize, DEC);
  unsigned char* buffer = (unsigned char*)malloc(bufferSize);
  if (buffer == NULL) {
    Serial.println("malloc did not work");
    return "";
  }
  buffer[bufferSize - 1] = '\0';
  Serial.println((size_t)buffer, HEX);
  decode_base64((unsigned char*) text.c_str(), buffer);
  buffer[bufferSize - 1] = '\0';
  Serial.println((char*)buffer);
  for (index = 0; buffer[index] && buffer[index] != ':'; index++);
  if (buffer[index]) {
    text = (char*)(buffer + index + 1);
    Serial.print("Password: ");
    Serial.println(text);
  } else {
    Serial.println(text);
    Serial.println(((char*)buffer));
    Serial.println("Password not found");
    text = "";
  }
  free(buffer);
  return text;
}

String ipToString(IPAddress ip) {
  String retval(ip[0], DEC);
  for (int i =1; i < 4; i++)
  {
      retval += ".";
      retval += String(ip[i], DEC);
  }
  return retval;
}
