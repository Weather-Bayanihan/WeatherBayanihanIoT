//// Simplified take on Arduino ESP8266 WiFi Management
//// https://github.com/johndelizo/ESP8266-SimpleWifiManager
//// by me[@]johndelizo.com

#include "SimpleWifiManager.h"

DNSServer dnsServer;
ESP8266WebServer webServer(80);
ConfigurableSerial cSerial;

void SimpleWifiManager::Begin(boolean enabled) {
  _enabled = enabled;

  if (!cSerial.Initialized) {
    cSerial.Begin(global_projectName, global_serial_baud, global_serial_loggingEnabled);
  }

  cSerial.Out("-- Start");

  cSerial.Out("--- Start - Configuration");
  cSerial.Out("---- Start - Configuration global_standardDelay", global_standardDelay);
  cSerial.Out("---- Start - Configuration global_processConnections_loop_interval", global_processConnections_loop_interval);
  cSerial.Out("---- Start - Configuration global_wifi_ReconfigurationEnabled", String(global_wifi_ReconfigurationEnabled));

  cSerial.Out("--- Start - SPIFFS Started");
  SPIFFS.begin();
  cSerial.Out("---- Start - SPIFFS Done");

  cSerial.Out("--- Start - Act as an access point (AP)");
  wifiResetSoftAp(false);

  cSerial.Out("--- Start - Name Server start (DNS)");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", WiFi.softAPIP());
  delay(global_standardDelay);

  cSerial.Out("--- Start - Web Server start");

  //c-unresolved-overloaded-function-type
  //https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type
  webServer.on("/", [this]() {
    onWebServerMainPage();
  });
  webServer.on("/refresh", [this]() {
    onWebServerMainPage();
  });
  webServer.on("/connect", [this]() {
    onWebServerConnectPage();
  });
  webServer.onNotFound([this]() {
    onWebServerNotFoundPage();
  });
  webServer.on("/change", [this]() {
    onWebServerChangePage();
  });
  webServer.on("/station", [this]() {
    onWebServerStationPage();
  });
  webServer.begin();
  delay(global_standardDelay);

  cSerial.Out("-- Start Complete, Server ready");
}

//// https://www.arduino.cc/reference/en/language/functions/time/delay/
//// https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
long processConnections_loop_previousTime = 0;

void SimpleWifiManager::ProcessConnections() {
  if (!_enabled) {
    cSerial.Out("-- SimpleWifiManager Not Enabled");
    return;
  }

  unsigned long processConnections_loop_currentTime = millis();
  if (processConnections_loop_currentTime - processConnections_loop_previousTime > global_processConnections_loop_interval)
  {
    cSerial.Out("-- ProcessConnections");
    processConnections_loop_previousTime = processConnections_loop_currentTime;

    cSerial.Out("--- ProcessConnections - dnsServer");
    dnsServer.processNextRequest();

    cSerial.Out("--- ProcessConnections - webServer");
    webServer.handleClient();

    cSerial.Out("--- ProcessConnections - WiFi SSID", wiFi_ap_ssid);
  }
}

void SimpleWifiManager::ConfigureProjectName(String projectName) {
  cSerial.Out("-- ConfigureProjectName");
  cSerial.Out("--- ConfigureProjectName - ", projectName);
  global_projectName = projectName;
}

void SimpleWifiManager::ConfigureWiFiReconfiguration(boolean wifiReconfigurationEnabled) {
  cSerial.Out("-- ConfigureWiFiReconfiguration");
  cSerial.Out("--- ConfigureWiFiReconfiguration - ", String(wifiReconfigurationEnabled));
  global_wifi_ReconfigurationEnabled = wifiReconfigurationEnabled;
}

void SimpleWifiManager::ConfigureDelays(int blockingDelays, long processingInterval) {
  cSerial.Out("-- ConfigureDelays");
  cSerial.Out("--- ConfigureDelays - blockingDelays", blockingDelays);
  cSerial.Out("--- ConfigureDelays - processingInterval", processingInterval);

  global_standardDelay = blockingDelays;
  global_processConnections_loop_interval = processingInterval;
}

void SimpleWifiManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  cSerial = configurableSerial;
  cSerial.Out("-- ConfigureLogging");
}

boolean SimpleWifiManager::CheckConnection() {
  cSerial.Out("-- CheckConnection");
  if (WiFi.status() == WL_CONNECTED) {
    cSerial.Out("-- CheckConnection - Connected");
    return true;
  }
  else {
    cSerial.Out("-- CheckConnection - Disconnected");
    return false;
  }
}

String SimpleWifiManager::CheckConnectionName() {
  cSerial.Out("-- CheckConnectionName");
  cSerial.Out("-- CheckConnectionName - Expected", wiFi_ap_ssid);
  return WiFi.SSID();
}

void SimpleWifiManager::onWebServerMainPage() {
  cSerial.Out("-- onWebServerMainPage");

  String webServer_hostHeader = webServer.hostHeader();
  cSerial.Out("--- onWebServerMainPage - HostHeader: " + webServer_hostHeader);

  //https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/handleHttp.ino
  if (webServer_hostHeader != ipAddressToString(wiFi_ap_initial_IPAddress)) {
    cSerial.Out("--- onWebServerMainPage - User redirect back home");
    webServerRedirectToHome();
    return;
  }

  String html_part4_body = webServerCurrentConnection();
  String html_part6_content = wifiNetworkScan();

  String htmlPage = html_part1_header + wiFi_ap_ssid + html_part2_title  + html_part3_style + html_part4_body + html_part6_content + html_Part7_password + html_Part8_footer;

  webServerSend(htmlPage);
}

void SimpleWifiManager::onWebServerNotFoundPage() {
  cSerial.Out("-- onWebServerNotFoundPage");

  String webServer_hostHeader = webServer.hostHeader();
  cSerial.Out("--- onWebServerNotFoundPage - HostHeader: " + webServer_hostHeader);

  if (webServer_hostHeader != ipAddressToString(wiFi_ap_initial_IPAddress)) {
    cSerial.Out("--- onWebServerNotFoundPage - User redirect back home");
    webServerRedirectToHome();
    return;
  }
  String html_part6_content = "<h1>Content not found!<h1>";
  String htmlPage = "";

  if (global_wifi_ReconfigurationEnabled) {
    htmlPage = html_part1_header + wiFi_ap_ssid + html_part2_title  + html_part4_body_template1 + wiFi_ap_ssid + html_part4_body_template2 + html_part6_content +  html_Part8_footer;
  } else {
    htmlPage = html_part1_header + wiFi_ap_ssid + html_part2_title  + html_part4_body_template3 + html_part6_content +  html_Part8_footer;
  }

  webServerSend(htmlPage, 404);
}

void SimpleWifiManager::onWebServerConnectPage() {
  cSerial.Out("-- onWebServerConnectPage");

  String wifi_new_ssId = webServer.arg("wifichoice");
  String wifi_new_password = webServer.arg("password");

  cSerial.Out("--- onWebServerConnectPage - wifi_new_ssId: " + wifi_new_ssId);
  cSerial.Out("--- onWebServerConnectPage - wifi_new_password: " + wifi_new_password);

  boolean wiFiConnectWithRetry_Success = wiFiConnectWithRetry(wifi_new_ssId, wifi_new_password, 5, 6000);

  if (wiFiConnectWithRetry_Success) {
    cSerial.Out("--- onWebServerConnectPage - wiFiConnectWithRetry Success - webServerRedirectToHome");
  }
  else {
    cSerial.Out("--- onWebServerConnectPage - wiFiConnectWithRetry Fail - responding with HTML Page");
  }

  webServerRedirectToHome();
}

void SimpleWifiManager::onWebServerChangePage() {
  cSerial.Out("-- onWebServerChangePage");
  cSerial.Out("-- onWebServerChangePage - global_wifi_ReconfigurationEnabled", global_wifi_ReconfigurationEnabled);
  if (global_wifi_ReconfigurationEnabled)
  {
    String htmlPage = html_part1_header + wiFi_ap_ssid + html_part2_title  + html_part3_style +
                      html_part6_body_stationName_templatepart1 + wiFi_ap_ssid + html_part6_body_stationName_templatepart2 +
                      html_Part8_footer;

    webServerSend(htmlPage);
  }
  else {
    onWebServerNotFoundPage();
  }
}

void SimpleWifiManager::onWebServerStationPage() {
  cSerial.Out("-- onWebServerStationPage");
  cSerial.Out("-- onWebServerStationPage - global_wifi_ReconfigurationEnabled", global_wifi_ReconfigurationEnabled);
  if (global_wifi_ReconfigurationEnabled)
  {
    wiFi_ap_ssid = webServer.arg("stationName");
    wiFi_ap_passkey = webServer.arg("password");
    wiFi_station_id = webServer.arg("stationId");
    
    cSerial.Out("-- onWebServerStationPage - wifiConfigurationSave");
    cSerial.Out("-- onWebServerStationPage - new wiFi_ap_ssid: ", wiFi_ap_ssid);
    cSerial.Out("-- onWebServerStationPage - new wiFi_station_id: ", wiFi_station_id);
    
    wifiConfigurationSave();
    webServerRedirectToHome(); //<EXPERIMENTAL> REDIRECT FIRST THEN SAVE. 

    cSerial.Out("-- onWebServerStationPage - wifiResetSoftAp");
    wifiResetSoftAp(true);
    cSerial.Out("-- onWebServerStationPage - webServerRedirectToHome");

    //webServerRedirectToHome();
    
  }
  else {
    onWebServerNotFoundPage();
  }
}

void SimpleWifiManager::webServerRedirectToHome() {
  cSerial.Out("-- webServerRedirectToHome");
  String ap_IpAddress = ipAddressToString(wiFi_ap_initial_IPAddress);
  cSerial.Out("-- webServerRedirectToHome - 302 Redirect to " + ap_IpAddress);
  webServer.sendHeader("Location", "http://" + ap_IpAddress, true);
  webServer.send(302, "text/plain", "");
  webServer.client().stop();
}

void SimpleWifiManager::webServerSend(String htmlPage) {
  webServerSend(htmlPage, 200);
}

void SimpleWifiManager::webServerSend(String htmlPage, int statusCode) {
  cSerial.Out("-- webServerSend");
  cSerial.Out("--- webServerSend Status Code: " + String(statusCode));
  ////<EXPERIMENTAL> FOR DEBUGGING PURPOSES////
  //cSerial.Out("---- webServerSend HTML CONTENT (Comment this for debugging): ");
  //cSerial.Out(htmlPage);  //
  /////////////////////////////
  String webServer_hostHeader = webServer.hostHeader();
  cSerial.Out("--- webServerSend - Client requesting for: " + webServer_hostHeader);

  int htmlPage_length = htmlPage.length();

  cSerial.Out("--- webServerSend - Started Sending: " + String(htmlPage_length) + " bytes");
  //Webpage Caching
  //https://stackoverflow.com/questions/49547/how-do-we-control-web-page-caching-across-all-browsers
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "0");
  webServer.sendHeader("Content-Length", String(htmlPage_length));
  webServer.send(statusCode, "text/html", htmlPage);

  cSerial.Out("--- webServerSend - Sent! ");

  if (htmlPage_length == 0) {
    cSerial.Out("--- webServerSend - content legth is zero, must stop web server client");
    webServer.client().stop();
  }
}

String SimpleWifiManager::wifiNetworkScan() {
  cSerial.Out("-- wifiNetworkScan");
  int networkCount = WiFi.scanNetworks();

  cSerial.Out("--- wifiNetworkScan - Found Number of broadcasting wifi: " + String(networkCount));

  String wifiChoices = "";

  for (int i = 0; i < networkCount; i++) {
    String wifiSSID = WiFi.SSID(i);
    wifiChoices = wifiChoices + "<div class='inputGroup'><input id='" + wifiSSID + "' value='" + wifiSSID + "' name='wifichoice' type='radio'/><label for='" + wifiSSID + "'>" + wifiSSID + "</label></div>";
  }
  return wifiChoices;
}

String SimpleWifiManager::webServerCurrentConnection() {
  cSerial.Out("-- webServerCurrentConnection");

  if (CheckConnection()) {
    cSerial.Out("--- webServerCurrentConnection - WiFi Connected");
    cSerial.Out("--- webServerCurrentConnection - WiFi Connected to", WiFi.SSID());
    if (global_wifi_ReconfigurationEnabled) {
      cSerial.Out("---- webServerCurrentConnection - global_wifi_ReconfigurationEnabled - true");
      return html_part5_body_connected_templatepart1 + WiFi.SSID() + html_part5_body_connected_templatepart2 + wiFi_ap_ssid + html_part5_body_connected_templatepart3;
    } else {
      cSerial.Out("--- webServerCurrentConnection - global_wifi_ReconfigurationEnabled - false");
      return html_part5_body_connected_templatepart1 + WiFi.SSID() + html_part5_body_connected_templatepart4;
    }
  }
  else {
    cSerial.Out("--- webServerCurrentConnection - WiFi Not Connected");
    if (global_wifi_ReconfigurationEnabled) {
      cSerial.Out("---- webServerCurrentConnection - global_wifi_ReconfigurationEnabled - true");
      return html_part4_body_template1 + wiFi_ap_ssid + html_part4_body_template2;
    } else {
      cSerial.Out("---- webServerCurrentConnection - global_wifi_ReconfigurationEnabled - false");
      return html_part4_body_template3;
    }
  }
}

void SimpleWifiManager::wifiResetSoftAp(boolean reconfigure) {
  cSerial.Out("-- wifiResetSoftAp");

  if (reconfigure) {
    cSerial.Out("--- wifiResetSoftAp - reconfigure - softAPdisconnect");
    WiFi.softAPdisconnect(true);
  }
  else  {
    cSerial.Out("--- wifiResetSoftAp - normal - persistent WIFI_AP_STA");
    WiFi.persistent(true);
    WiFi.mode(WIFI_AP_STA);
  }

  cSerial.Out("--- wifiResetSoftAp - wifiConfigurationRead");
  wifiConfigurationRead();

  cSerial.Out("--- wifiResetSoftAp - softAPConfig");
  WiFi.softAPConfig(wiFi_ap_initial_IPAddress, wiFi_ap_initial_Gateway, wiFi_ap_initial_Subnet);

  cSerial.Out("--- wifiResetSoftAp - softAP");
  cSerial.Out("---- wifiResetSoftAp - softAP - wiFi_ap_ssid", wiFi_ap_ssid);
  cSerial.Out("---- wifiResetSoftAp - softAP - wiFi_ap_passkey", wiFi_ap_passkey);
  WiFi.softAP(wiFi_ap_ssid, wiFi_ap_passkey);

  cSerial.Out("--- Start - WiFi-Begin - TurnOn Wireless using last config from flash memory (WiFi)");
  //https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html?highlight=WiFi.begin#begin
  WiFi.begin();
  delay(global_standardDelay);
}

boolean SimpleWifiManager::wiFiConnectWithRetry(String wifiSsid, String wifiPassword, int retryCount, int retryDelay) {
  cSerial.Out("-- wiFiConnectWithRetry");

  if (CheckConnection()) {
    cSerial.Out("--- wiFiConnectWithRetry - WL_CONNECTED - Disconnect to current network");
    WiFi.disconnect();
  }

  while (retryCount >= 1) {
    cSerial.Out("--- wiFiConnectWithRetry - WiFi.begin");
    int wiFi_begin_status = WiFi.begin(wifiSsid, wifiPassword);
    delay(retryDelay);
    //Use WiFi.status instead of Wifi.begin response.
    //https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
    if (CheckConnection()) {
      cSerial.Out("--- wiFiConnectWithRetry - WiFi.begin - Connected!");
      return true;
    }
    else {
      retryCount--;
      cSerial.Out("--- wiFiConnectWithRetry - WiFi.begin - Not Connected, Retry left: " + String(retryCount));
      delay(retryDelay);
    }
  }
  cSerial.Out("--- wiFiConnectWithRetry - WiFi.begin - Retry exhausted, will exit.");
  return false;
}

void SimpleWifiManager::wifiConfigurationSave() {
  cSerial.Out("-- wifiConfigurationSave");
  spiffsConfigurationWrite(spiffs_Configuration_ap_ssid, wiFi_ap_ssid);
  spiffsConfigurationWrite(spiffs_Configuration_ap_passkey, wiFi_ap_passkey);
  spiffsConfigurationWrite(spiffs_Configuration_station_id, wiFi_station_id);
}

void SimpleWifiManager::wifiConfigurationRead() {
  cSerial.Out("-- wifiConfigurationRead");
  wiFi_ap_ssid = spiffsConfigurationRead(spiffs_Configuration_ap_ssid, wiFi_ap_ssid);
  wiFi_ap_passkey = spiffsConfigurationRead(spiffs_Configuration_ap_passkey, wiFi_ap_passkey);
  wiFi_station_id = spiffsConfigurationRead(spiffs_Configuration_station_id, wiFi_station_id);
  
}

String SimpleWifiManager::spiffsConfigurationRead(String source, String defaultvalue) {
  cSerial.Out("-- spiffsConfigurationRead");
  cSerial.Out("--- spiffsConfigurationRead -- source", source);
  String outPut = "";
  File sourcefile = SPIFFS.open("/" + source, "r");
  if (sourcefile) {
    outPut = sourcefile.readString();
    sourcefile.close();
    cSerial.Out("--- spiffsConfigurationRead -- sourcefile readString close");
  }

  if (outPut.length() == 0) {
    outPut = defaultvalue;
  }

  cSerial.Out("--- spiffsConfigurationReadionRead -- outPut", outPut);
  return outPut;
}

String SimpleWifiManager::GetStationId(){
  cSerial.Out("-- GetStationId");
  wiFi_station_id = spiffsConfigurationRead(spiffs_Configuration_station_id, wiFi_station_id);
  cSerial.Out("-- GetStationId wiFi_station_id : ", wiFi_station_id);
  return wiFi_station_id;
}

void SimpleWifiManager::spiffsConfigurationWrite(String source, String value) {
  cSerial.Out("-- spiffsConfigurationWrite");
  cSerial.Out("-- spiffsConfigurationWrite -- source", source);
  cSerial.Out("-- spiffsConfigurationWrite -- value", value);
  File sourcefile = SPIFFS.open("/" + source, "w");
  if (sourcefile) {
    sourcefile.print(value);
    sourcefile.close();
    cSerial.Out("-- spiffsConfigurationWrite -- sourcefile print close");
  }
}

String SimpleWifiManager::ipAddressToString(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
