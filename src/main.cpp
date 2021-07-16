// #include <Arduino.h>

// #ifdef ESP32
#include <WiFi.h>
// #else
//   #include <ESP8266WiFi.h>
// #endif

int16_t networksFound;

const char *wl_status_to_string(wl_status_t status)
{
  switch (status)
  {
  case WL_NO_SHIELD:
    return "WL_NO_SHIELD";
  case WL_IDLE_STATUS:
    return "WL_IDLE_STATUS";
  case WL_NO_SSID_AVAIL:
    return "WL_NO_SSID_AVAIL";
  case WL_SCAN_COMPLETED:
    return "WL_SCAN_COMPLETED";
  case WL_CONNECTED:
    return "WL_CONNECTED";
  case WL_CONNECT_FAILED:
    return "WL_CONNECT_FAILED";
  case WL_CONNECTION_LOST:
    return "WL_CONNECTION_LOST";
  // case WL_WRONG_PASSWORD: return "WL_WRONG_PASSWORD";
  case WL_DISCONNECTED:
    return "WL_DISCONNECTED";
  default:
    return "NO_OPTION";
  }
}

const char *wifi_auth_mode_to_string(wifi_auth_mode_t auth)
{
  switch (auth)
  {
  case WIFI_AUTH_OPEN:
    return "OPEN";
  case WIFI_AUTH_WEP:
    return "WEP";
  case WIFI_AUTH_WPA_PSK:
    return "WPA_PSK";
  case WIFI_AUTH_WPA2_PSK:
    return "WPA2_PSK";
  case WIFI_AUTH_WPA_WPA2_PSK:
    return "WPA_WPA2_PSK";
  case WIFI_AUTH_WPA2_ENTERPRISE:
    return "WPA2_ENTERPRISE";
  case WIFI_AUTH_MAX:
    return "MAX";
  default:
    return "NO_OPTION";
  }
}

void prinScanResult(int16_t networksFound)
{
  Serial.printf("%d network(s) found\n", networksFound);
  for (int16_t i = 0; i < networksFound; i++)
  {
    // Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), wifi_auth_mode_to_string(WiFi.encryptionType(i)));
  }
}

void scanNetworksToConect()
{
  WiFi.disconnect();
  WiFi.scanDelete();
  delay(100);
  Serial.print("Scaing ...");
  networksFound = WiFi.scanNetworks(true, true);
  while (networksFound < 0)
  {
    delay(100);
    Serial.print(".");
    networksFound = WiFi.scanComplete();
  }
  Serial.println();
  prinScanResult(networksFound);
  Serial.println();
  Serial.print("Network index to connect: ");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(3000);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  WiFi.setHostname("Valiot-PentaWiFiTest");

  WiFi.mode(WIFI_STA);
  scanNetworksToConect();
}

void loop()
{
  if (Serial.available())
  {
    String incomingString = Serial.readString();
    int networkIndex = incomingString.toInt();
    Serial.println(networkIndex);
    if (networkIndex > 0 && networkIndex < networksFound)
    {
      Serial.printf("Trying to connect to: \"%s\" \r\n", WiFi.SSID(networkIndex - 1).c_str());
      Serial.print("enter password (don't press enter): ");
      while (!Serial.available())
      {
        delay(10);
      }
      String password = Serial.readString();

      if (password == "\r\n")
      {
        Serial.println("No password");
        WiFi.begin(WiFi.SSID(networkIndex - 1).c_str());
      }
      else
      {
        Serial.println(password);
        WiFi.begin(WiFi.SSID(networkIndex - 1).c_str(), password.c_str());
      }

      Serial.println("Connecting ...");
      while (WiFi.status() != WL_CONNECTED)
      {
        Serial.println(wl_status_to_string(WiFi.status()));
        delay(500);
        if (Serial.available())
          break;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println(wl_status_to_string(WiFi.status()));
        Serial.println();

        Serial.println("Connected.");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Gateway IP address: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("subnetMask address: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("subnetCIDR address: ");
        Serial.println(WiFi.subnetCIDR());
        Serial.print("dnsIP address: ");
        Serial.println(WiFi.dnsIP());

        while (!Serial.available())
        {
          Serial.printf("RSSI: %ddBm \r\n", WiFi.RSSI());
          delay(1000);
        }
      }
    }
    else
    {
      Serial.println("Invalid index");
    }
    scanNetworksToConect();
    Serial.readString(); //to clean Serial
  }
}

//press EN y IO0
//sol EN
//Leave IO0 en cuna
