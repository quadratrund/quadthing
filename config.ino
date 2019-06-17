#include <EEPROM.h>
#include <Hash.h>

uint8_t cfg_outputCount;
bool cfg_feedbackMode;
uint8_t cfg_pwdhash[20];
char cfg_ssid[33];
char cfg_key[64];
IPAddress cfg_ip;
uint8_t cfg_prefixlen;
IPAddress cfg_netmask;
IPAddress cfg_gateway;
uint8_t cfg_safetyTimeout;
uint32_t cfg_safetyTimeout_ms;


void EepromUpdate(int address, uint8_t value) {
  uint8_t oldValue = EEPROM.read(address);
  Serial.print("EEPROM ");
  Serial.print(address, DEC);
  Serial.print("\t");
  Serial.print(oldValue, HEX);
  Serial.print("\t");
  Serial.println(value, HEX);
  if (value != oldValue) {
    EEPROM.write(address, value);
  }
}

void config_reset() {
  uint8_t hash[20];
  int i;
  int address = 0;

  Serial.println();
  Serial.println("RESET CONFIG");
  Serial.println();

  // extra byte
  EepromUpdate(address++, 'Q');
  // anzahl outputs
  EepromUpdate(address++, 8);
  // pwdhash
  sha1("", 0, hash);
  for (i = 0; i < 20; i++) {
    EepromUpdate(address++, hash[i]);
  }
  // wifi ssid
  EepromUpdate(address, 0);
  address += 32;
  // wifi key
  EepromUpdate(address, 0);
  address += 63;
  // ip
  EepromUpdate(address++, 0);
  EepromUpdate(address++, 0);
  EepromUpdate(address++, 0);
  EepromUpdate(address++, 0);
  // prefixlen
  EepromUpdate(address++, 24);
  //gateway
  EepromUpdate(address++, 192);
  EepromUpdate(address++, 168);
  EepromUpdate(address++, 0);
  EepromUpdate(address++, 1);
  // safetyTimeout
  EepromUpdate(address++, 0);
  EEPROM.commit();
}

void config_read() {
  int i;
  uint8_t ipBuf[4];
  int address = 0;
  char c = EEPROM.read(address++);
  if (c != 'Q') {
    config_reset();
  }

  cfg_outputCount = min((int)EEPROM.read(address++), 12);
  cfg_feedbackMode = cfg_outputCount > 8;
  if (cfg_feedbackMode) {
    cfg_outputCount -= 8;
  }

  for (i = 0; i < 20; i++) {
    cfg_pwdhash[i] = EEPROM.read(address++);
  }

  for (i = 0; i < 32; i++) {
    cfg_ssid[i] = EEPROM.read(address++);
  }
  cfg_ssid[32] = '\0';

  for (i = 0; i < 63; i++) {
    cfg_key[i] = EEPROM.read(address++);
  }
  cfg_key[63] = '\0';

  for (i = 0; i < 4; i++) {
    ipBuf[i] = EEPROM.read(address++);
  }
  cfg_ip = ipBuf;

  cfg_prefixlen = EEPROM.read(address++);
  updateSubnetmask();

  for (i = 0; i < 4; i++) {
    ipBuf[i] = EEPROM.read(address++);
  }
  cfg_gateway = ipBuf;

  cfg_safetyTimeout = EEPROM.read(address++);
  cfg_safetyTimeout_ms = cfg_safetyTimeout;
  cfg_safetyTimeout_ms *= 60000;
}

void updateSubnetmask() {
  union {
    uint32_t temp1;
    uint8_t temp2[4];
  };
  uint8_t temp3[4];
  temp1 = -1;
  temp1 <<= (32 - cfg_prefixlen);
  temp3[0] = temp2[3];
  temp3[1] = temp2[2];
  temp3[2] = temp2[1];
  temp3[3] = temp2[0];
  cfg_netmask = temp3;
}

void config_setNewPass(String postVars) {
  String pass = getParamValue(postVars, "pass");
  Serial.println(pass);
  int address = 2;
  sha1(pass, cfg_pwdhash);
  for (int i = 0; i < 20; i++) {
    EepromUpdate(address++, cfg_pwdhash[i]);
  }
  EEPROM.commit();
}

void config_setNetwork(String postVars) {
  String newSsid = getParamValue(postVars, "ssid");
  newSsid.remove(32);
  String newKey = getParamValue(postVars, "key");
  newKey.remove(63);
  IPAddress newIp;
  if (!newIp.fromString(getParamValue(postVars, "ip"))) {
    newIp = (uint32_t)0;
  }
  uint8_t newPrefixlen = getParamValue(postVars, "prefixlen").toInt();
  if (newPrefixlen > 32) {
    newPrefixlen = 32;
  }
  IPAddress newGateway;
  if (!newGateway.fromString(getParamValue(postVars, "gateway"))) {
    newGateway = newIp;
    newGateway[3] = 1;
  }
  Serial.println(newSsid);
  Serial.println(newKey);
  Serial.println(newIp);
  Serial.println(newPrefixlen);
  Serial.println(newGateway);
  

  int address = 22;
  int i;
  const char* s;
  // wifi ssid
  s = newSsid.c_str();
  for (i = 0; s[i]; i++) {
    EepromUpdate(address + i, s[i]);
  }
  if (i < 32) {
    EepromUpdate(address + i, '\0');
  }
  address += 32;
  // wifi key
  s = newKey.c_str();
  for (i = 0; s[i]; i++) {
    EepromUpdate(address + i, s[i]);
  }
  if (i < 63) {
    EepromUpdate(address + i, '\0');
  }
  address += 63;
  // ip
  EepromUpdate(address++, newIp[0]);
  EepromUpdate(address++, newIp[1]);
  EepromUpdate(address++, newIp[2]);
  EepromUpdate(address++, newIp[3]);
  // prefixlen
  EepromUpdate(address++, newPrefixlen);
  //gateway
  EepromUpdate(address++, newGateway[0]);
  EepromUpdate(address++, newGateway[1]);
  EepromUpdate(address++, newGateway[2]);
  EepromUpdate(address++, newGateway[3]);
  EEPROM.commit();
}

void config_outputCount(String postVars) {
  int count = getParamValue(postVars, "outputs").toInt();
  Serial.println(count);
  if (count < 1 || count > 12) {
    count = 8;
  }
  EepromUpdate(1, count);

  int timeout = getParamValue(postVars, "timeout").toInt();
  Serial.println(timeout);
  if (timeout < 0 || timeout > 255) {
    timeout = 0;
  }
  EepromUpdate(126, timeout);

  EEPROM.commit();
}
