#include "switch.h"
#include "translation_en.h"

String navBar() {
  return
    "<div class=\"navbar\">"
      "<a href=\"/p/home\">" LANG_HOME "</a> "
      "<a href=\"/p/network\">" LANG_NETWORK "</a> "
      "<a href=\"/p/pass\">" LANG_CHANGE_PASSWORD "</a> "
      "<a href=\"/p/config\">" LANG_OTHER_CONFIGURATION "</a> "
    "</div>";
}

HttpResponse pageNetwork() {
  String tmp;
  HttpResponse response = http_okHead();
  response.print("<body>");
  response.print(navBar());
  response.print("<form method=\"POST\" action=\"/c/network\"><table border=\"1\">");
  response.print("<tr><td>" LANG_WIFI_SSID ":</td><td><input name=\"ssid\" value=\"");
  tmp = cfg_ssid;
  htmlspecialchars(tmp);
  response.print(tmp);
  response.print("\"></td></tr>");
  response.print("<tr><td>" LANG_WIFI_KEY ":</td><td><input name=\"key\" value=\"");
  tmp = cfg_key;
  htmlspecialchars(tmp);
  response.print(tmp);
  response.print("\"></td></tr>");
  response.print("<tr><td>" LANG_IP "</td><td><input name=\"ip\" value=\"");
  response.print(cfg_ip);
  response.print("\"></td></tr>");
  response.print("<tr><td>" LANG_PREFIXLENGTH "</td><td><input name=\"prefixlen\" value=\"");
  response.print(cfg_prefixlen, DEC);
  response.print("\"></td></tr>");
  response.print("<tr><td>Gateway</td><td><input name=\"gateway\" value=\"");
  response.print(cfg_gateway);
  response.print("\"></td></tr>");
  response.print("<tr><td>&nbsp;</td><td><button type=\"submit\">" LANG_OK "</button></td></tr>");
  response.print("</table></form></body></html>");
  return response;
}

HttpResponse pagePass() {
  HttpResponse response = http_okHead();
  response.content += "<body>";
  response.content += navBar();
  response.content += "<form method=\"POST\" action=\"/c/pass\">" LANG_NEW_PASSWORD ": <input type=\"password\" name=\"pass\"><button type=\"submit\">" LANG_OK "</button></form></body></html>";
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
    response.content += switch_getOutState(i) ? LANG_ON : LANG_OFF;
    response.content += "</td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=on\">" LANG_SWITCH_ON "</a></td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=off\">" LANG_SWITCH_OFF "</a></td><td><a href=\"/switch?id=";
    response.content += String(i, DEC);
    response.content += "&set=toggle\">" LANG_TOGGLE "</a></td></tr>";
  }
  response.content += "</table>";
  return response;
}

HttpResponse page_apiGenericResponse(String payloadJson) {
  int i;
  HttpResponse response = http_ok();
  response.content = "{\"status\":{\"code\":0,\"message\":\"ok\"},\"payload\":";
  response.content += payloadJson;
  response.content += '}';
  return response;
}

HttpResponse page_apiStates() {
  int i;
  String data = "{\"output\":[";
  HttpResponse response = http_ok();
  for (i = 0; i < cfg_outputCount; i++) {
    data += outState[i] ? "false," : "true,";
  }
  data.setCharAt(data.length() - 1, ']');
  if (cfg_feedbackMode) {
    data += ",\"inputs\":[";
    for (i = 0; i < cfg_outputCount; i++) {
      data += digitalRead(outputs[i+4]) == HIGH ? "true," : "false,";
    }
    data.setCharAt(data.length() - 1, ']');
  }
  data += "}";
  return page_apiGenericResponse(data);
}

HttpResponse page_config() {
  HttpResponse response = http_okHead();
  response.content += "<body>";
  response.content += navBar();
  response.content += "<form method=\"POST\" action=\"/c/config\">"
                      LANG_GPIO_MODE ":"
                      "<select name=\"outputs\" id=\"outputs\">"
                      "<option value=\"8\">8 " LANG_OUTPUTS "</option>"
                      "<option value=\"12\">4 " LANG_OUTPUTS " (" LANG_FEEDBACK_MODE ")</option>"
                      "<option value=\"13\">4 " LANG_BUNDLED_OUTPUTS "</option>"
                      "<option value=\"14\">2 " LANG_BUNDLED_OUTPUTS " (" LANG_FEEDBACK_MODE ")</option>"
                      "</select><br>"
                      LANG_GPIO_MODE_DESRIPTION "<br><br>"
                      LANG_SAFETY_OFF_PREFIX " <input type=\"text\" name=\"timeout\" value=\"";
  response.content += String(cfg_safetyTimeout, DEC);
  response.content += "\"> " LANG_SAFETY_OFF_SUFFIX "<br>"
                      LANG_SAFETY_OFF_DESCRIPTION "<br><br>"
                      "<button type=\"submit\">" LANG_OK "</button>"
                      "</form><script>document.getElementById('outputs').value='";
  if (cfg_feedbackMode) {
    if (cfg_bundleMode) {
      response.content += "14";
    } else {
      response.content += String(cfg_outputCount + 8, DEC);
    }
  } else {
    if (cfg_bundleMode) {
      response.content += "13";
    } else {
      response.content += String(cfg_outputCount, DEC);
    }
  }
  response.content += "';</script></body></html>";
  return response;
}

HttpResponse page_css() {
  HttpResponse response = 200;
  response.addHeader("Content-Type", "text/css; charset=us-ascii");
  response.addHeader("Cache-Control", "max-age=86400");
  response.content = "body { margin: 10px; }"
                     ".navbar { background-color: black; padding-left: 10px; margin-top: -10px; margin-left: -10px; margin-right: -10px; margin-bottom: 10px; }"
                     ".navbar > a { display: inline-block; padding: 10px; color: white; text-decoration: none; }"
                     ".navbar > a:hover { background-color: #444444; }";
  return response;
}

HttpResponse page_redirect(String path) {
  HttpResponse response = http_okHead();
  response.content += "<body>";
  response.content += navBar();
  response.content += "<script>setTimeout(() => location.href = '";
  response.content += path;
  response.content += "', 3000);</script></body></html>";
  return response;
}
