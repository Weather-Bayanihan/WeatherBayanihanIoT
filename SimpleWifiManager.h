//// Simplified take on Arduino ESP8266 WiFi Management
//// https://github.com/johndelizo/ESP8266-SimpleWifiManager
//// by me[@]johndelizo.com 

#ifndef SimpleWifiManager_h
#define SimpleWifiManager_h
#include "Arduino.h"
#include "ConfigurableSerial.h"
#include <ESP8266WiFi.h>              //https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
#include <DNSServer.h>                //https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer
#include <ESP8266WebServer.h>         //https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
#include <FS.h>                       //https://github.com/esp8266/Arduino/blob/master/cores/esp8266/FS.h

class SimpleWifiManager {
  public:
    void ConfigureLogging(ConfigurableSerial configurableSerial);
    void ConfigureProjectName(String projectName);
    void ConfigureDelays(int blockingDelays, long processingInterval);
    void ConfigureWiFiReconfiguration(boolean wifiReconfigurationEnabled);
    
    void Begin(boolean enabled = true);
    void ProcessConnections();
    boolean CheckConnection();
    String CheckConnectionName();
    String GetStationId();
  private:
  
    boolean _enabled;
    
    String global_projectName = "ESP8266-SimpleWiFiManager";
    int global_standardDelay = 3000;
    long global_processConnections_loop_interval = 3000;
    
    boolean global_serial_loggingEnabled = true;
    int global_serial_baud = 115200;
    boolean global_wifi_ReconfigurationEnabled = true;

    String wiFi_station_id = "";
    
    String wiFi_ap_ssid = "IoT Device 1";
    String wiFi_ap_passkey = "hello123";
    IPAddress wiFi_ap_initial_IPAddress = IPAddress(11, 1, 1, 1);
    IPAddress wiFi_ap_initial_Gateway = IPAddress(11, 1, 1, 1);
    IPAddress wiFi_ap_initial_Subnet = IPAddress(255, 255, 255, 0);

    String spiffs_Configuration_ap_ssid = "wifissid.txt";
    String spiffs_Configuration_ap_passkey = "wifiKey.txt";
    String spiffs_Configuration_station_id = "stationid.txt";

    const String html_part1_header = "<!DOCTYPE HTML><html lang='en'><head><title>";
    const String html_part2_title = "</title><meta charset='utf-8' /><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
    const String html_part3_style = "<style>body{background-color:#d1d7dc;font-family:sans-serif}*,::after,::before{box-sizing:inherit}html{box-sizing:border-box}.container{padding:0 10px;max-width:550px;margin:20px auto;font-size:18px;font-weight:600;line-height:36px}.inputGroup{background-color:#fff;display:block;margin:10px 0;position:relative}.inputGroup label{padding:12px 30px;width:100%;display:block;text-align:left;color:#3c454c;cursor:pointer;position:relative;z-index:2;-webkit-transition:color .2s ease-in;transition:color .2s ease-in;overflow:hidden}.inputGroup label:before{width:10px;height:10px;border-radius:50%;content:\"\";background-color:#5562eb;position:absolute;left:50%;top:50%;-webkit-transform:translate(-50%,-50%) scale3d(1,1,1);transform:translate(-50%,-50%) scale3d(1,1,1);-webkit-transition:all .3s cubic-bezier(.4,0,.2,1);transition:all .3s cubic-bezier(.4,0,.2,1);opacity:0;z-index:-1}.inputGroup label:after{width:32px;height:32px;content:\"\";border:2px solid #d1d7dc;background-color:#fff;background-image:url(\"data:image/svg+xml,%3Csvg width='32' height='32' viewBox='0 0 32 32' xmlns='http://www.w3.org/2000/svg'%3E%3Cpath d='M5.414 11L4 12.414l5.414 5.414L20.828 6.414 19.414 5l-10 10z' fill='%23fff' fill-rule='nonzero'/%3E%3C/svg%3E\");background-repeat:no-repeat;background-position:2px 3px;border-radius:50%;z-index:2;position:absolute;right:30px;top:50%;-webkit-transform:translateY(-50%);transform:translateY(-50%);cursor:pointer;-webkit-transition:all .2s ease-in;transition:all .2s ease-in}.inputGroup input:checked~label{color:#fff}.inputGroup input:checked~label:before{-webkit-transform:translate(-50%,-50%) scale3d(56,56,1);transform:translate(-50%,-50%) scale3d(56,56,1);opacity:1}.inputGroup input:checked~label:after{background-color:#54e0c7;border-color:#54e0c7}.inputGroup input{width:32px;height:32px;-webkit-box-ordinal-group:2;order:1;z-index:2;position:absolute;right:30px;top:50%;-webkit-transform:translateY(-50%);transform:translateY(-50%);cursor:pointer;visibility:hidden}.button{background-color:#5562eb;display:block;position:relative;text-align:center;text-decoration:none;padding:15px 32px;color:#fff;border:none;font-size:12px}.header{display:flex;justify-content:space-between;background-color:#fff;padding:0 15px;margin:15px 0}.header-vert{background-color:#fff;padding:15px;margin:15px 0}.header-vert *{color:#54e0c7;letter-spacing:3px;font-size:18px;text-align:center;margin:15px 0;height:40px;width:100%}.button,.header a{background-color:#5562eb;display:block;position:relative;text-align:center;text-decoration:none;padding:0 15px;margin:15px 0;color:#fff;border:none;font-size:12px;height:40px}.header div{font-size:22px;padding:10px}.header .password{color:#54e0c7;letter-spacing:3px;font-size:18px;text-align:center;margin:15px 0;height:40px}.connected{background-color:#5562eb;text-decoration:none;padding:5px 15px;color:#fff;border:none;margin:10px 0}</style></head>";
    
    const String html_part4_body_template1 = "<body><div class='container'><form action=\"/connect\"><div class='header'><div>Device Name: ";
    const String html_part4_body_template2 = "</div><a href='/change'>Change</a></div><div class='header'><div>WiFi Connections</div> <a href='/refresh'>Refresh</a></div>";
    const String html_part4_body_template3 = "<body><div class='container'><form action=\"/connect\"><div class='header'><div>WiFi Connections</div> <a href='/refresh'>Refresh</a></div>";

    const String html_part5_body_connected_templatepart1 = "<body><div class='container'><form action=\"/connect\"><div class=\"connected\"><p>Connected to:</p><h2>";
    const String html_part5_body_connected_templatepart2 = "</h2></div><div class='header'><div>Device Name: ";
    const String html_part5_body_connected_templatepart3 = "</div><a href='/change'>Change</a></div><div class='header'><div>WiFi Connections</div> <a href='/refresh'>Refresh</a></div>";
    const String html_part5_body_connected_templatepart4 = "</h2></div><div class='header'><div>WiFi Connections</div> <a href='/refresh'>Refresh</a></div>";

    const String html_part6_body_stationName_templatepart1 = "<body><div class='container'><form action=\"/station\"><div class=\"connected\"><p>Device Name:</p><h2>";
    const String html_part6_body_stationName_templatepart2 = "</h2></div><div class=\"header-vert\"><input id=\"stationName\" class=\"password\" name=\"stationName\" type=\"text\" placeholder=\"New Device Name\" /><input id=\"stationName\" class=\"password\" name=\"stationId\" type=\"text\" placeholder=\"New Device Id\" /><input id=\"password\" class=\"password\" name=\"password\" type=\"password\" placeholder=\"Enter Password\" /><button type=\"submit\" class=\"button\" id=\"connect\">Change</button></div></form></div>";
    
    const String html_Part7_password = "<div class=\"header\"> <input id=\"password\"  class=\"password\" name=\"password\" type=\"password\" placeholder=\"Enter Password\" /> <button type=\"submit\"  class=\"button\" id=\"connect\">Connect</button></div></form></div>";
    const String html_Part8_footer = "</div></body></html>";

    void onWebServerMainPage();
    void onWebServerConnectPage();
    void onWebServerNotFoundPage();
    void onWebServerChangePage();
    void onWebServerStationPage();
    void webServerRedirectToHome();
    void webServerSend(String htmlPage);
    void webServerSend(String htmlPage, int statusCode);
    String webServerCurrentConnection();
    String wifiNetworkScan();
    boolean wiFiConnectWithRetry(String wifiSsid, String wifiPassword, int retryCount, int retryDelay);
    void wifiResetSoftAp(boolean reconfigure);
    void wifiConfigurationSave();
    void wifiConfigurationRead();
    String spiffsConfigurationRead(String source, String defaultvalue);
    void spiffsConfigurationWrite(String source, String value);
    String ipAddressToString(IPAddress ip);
};

#endif
