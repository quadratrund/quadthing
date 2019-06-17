/*
max 32 SSID
8 - 63 Key
0 Mode: Static/DHCP
4 IP
4 gateway
1 prefixlen (0 - 32)
1 anzahl outputs
20 pwhash
1 extra
-----
126


  0      'Q'
  1      anzahl outputs
  2.. 21 pwd hash
 22.. 53 wifi ssid
 54..116 wifi key
117..120 ip
121      prefixlen
122..125 gateway
126      safetyTimeout

 */
