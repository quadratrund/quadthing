extern uint8_t outState[8];

String navBar() {
  return
    "<a href=\"/p/home\">Home</a> "
    "<a href=\"/p/network\">Netzwerk</a> "
    "<a href=\"/p/pass\">Kennwort &auml;ndern</a> "
    "<a href=\"/p/config\">Sonstige Konfiguration</a> "
    "<br>";
}

HttpResponse pageNetwork() {
  String tmp;
  HttpResponse response = http_okHead();
  response.print("<body>");
  response.print(navBar());
  response.print("<form method=\"POST\" action=\"/c/network\"><table border=\"1\">");
  response.print("<tr><td>Wifi SSID:</td><td><input name=\"ssid\" value=\"");
  tmp = cfg_ssid;
  htmlspecialchars(tmp);
  response.print(tmp);
  response.print("\"></td></tr>");
  response.print("<tr><td>Wifi Key:</td><td><input name=\"key\" value=\"");
  tmp = cfg_key;
  htmlspecialchars(tmp);
  response.print(tmp);
  response.print("\"></td></tr>");
  response.print("<tr><td>IP</td><td><input name=\"ip\" value=\"");
  response.print(cfg_ip);
  response.print("\"></td></tr>");
  response.print("<tr><td>Prefixl&auml;nge</td><td><input name=\"prefixlen\" value=\"");
  response.print(cfg_prefixlen, DEC);
  response.print("\"></td></tr>");
  response.print("<tr><td>Gateway</td><td><input name=\"gateway\" value=\"");
  response.print(cfg_gateway);
  response.print("\"></td></tr>");
  response.print("<tr><td>&nbsp;</td><td><button type=\"submit\">OK</button></td></tr>");
  response.print("</table></form></body></html>");
  return response;
}

HttpResponse pagePass() {
  HttpResponse response = http_okHead();
  response.content += "<body>";
  response.content += navBar();
  response.content += "<form method=\"POST\" action=\"/c/pass\">neues Kennwort: <input type=\"password\" name=\"pass\"><button type=\"submit\">OK</button></form></body></html>";
  return response;
}

HttpResponse pageHome() {
  HttpResponse response = http_okHead();
  response.content += navBar();

  response.content += "<table border=\"1\">";
  int i;
  for (i = 0; i < cfg_outputCount; i++) {
    response.content += "<tr><td>";
    response.content += String(i + 1, DEC);
    response.content += "</td><td>";
    response.content += switch_getOutState(i) ? "AN" : "AUS";
    response.content += "</td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=on\">einschalten</a></td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=off\">ausschalten</a></td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=toggle\">umschalten</a></td></tr>";
  }
  response.content += "</table>";
  return response;
}

HttpResponse page_apiStates() {
  int i;
  String data = "[";
  HttpResponse response = http_ok();
  for (i = 0; i < cfg_outputCount; i++) {
    data += outState[i] ? "true," : "false,";
  }
  data.setCharAt(data.length() - 1, ']');
  response.content = data;
  return response;
}

HttpResponse page_config() {
  HttpResponse response = http_okHead();
  response.content += "<body>";
  response.content += navBar();
  response.content += "<form method=\"POST\" action=\"/c/config\">"
                      "Betriebsmodus:"
                      "<select name=\"outputs\" id=\"outputs\">"
                      "<option value=\"1\">1 Ausgang</option>"
                      "<option value=\"2\">2 Ausg&auml;nge</option>"
                      "<option value=\"3\">3 Ausg&auml;nge</option>"
                      "<option value=\"4\">4 Ausg&auml;nge</option>"
                      "<option value=\"5\">5 Ausg&auml;nge</option>"
                      "<option value=\"6\">6 Ausg&auml;nge</option>"
                      "<option value=\"7\">7 Ausg&auml;nge</option>"
                      "<option value=\"8\">8 Ausg&auml;nge</option>"
                      "<option value=\"9\">1 Ausgang (Feedback-Modus)</option>"
                      "<option value=\"10\">2 Ausg&auml;nge (Feedback-Modus)</option>"
                      "<option value=\"11\">3 Ausg&auml;nge (Feedback-Modus)</option>"
                      "<option value=\"12\">4 Ausg&auml;nge (Feedback-Modus)</option>"
                      "</select><br>"
                      "Die Pins D0 bis D3 sind die Ausg&auml;nge 1-4.<br>"
                      "Die Pins D5 bis D8 sind die Ausg&auml;nge 5-8 oder die Feedback-Eing&auml;nge f&uuml;r die Ausg&auml;nge 1-4.<br><br>"
                      "Sicherheitsabschaltung nach <input type=\"text\" name=\"timeout\" value=\"";
  response.content += String(cfg_safetyTimeout, DEC);
  response.content += "\"> Minuten (Maximum: 255)<br>"
                      "Die Sicherheitsabschaltung schaltet alle Ausg&auml;nge aus, wenn die angegebene Anzahl Minuten seit dem letzten Schaltvorgang vergangen ist.<br>"
                      "Sie wird deaktiviert, wenn hier 0 Minuten eigestellt sind.<br><br>"
                      "<button type=\"submit\">OK</button>"
                      "</form><script>document.getElementById('outputs').value='";
  if (cfg_feedbackMode) {
    response.content += String(cfg_outputCount + 8, DEC);
  } else {
    response.content += String(cfg_outputCount, DEC);
  }
  response.content += "';</script></body></html>";
  return response;
}

/*
HttpResponse page_config_js() {
  HttpResponse response = http_ok();
  response.addHeader("Content-Type", "text/javascript; charset=us-ascii");
  response.content += "document.getElementById('outputs').value='";
  if (cfg_feedbackMode) {
    response.content += String(cfg_outputCount + 8, DEC);
  } else {
    response.content += String(cfg_outputCount, DEC);
  }
  response.content += "';";
  return response;
}
*/
