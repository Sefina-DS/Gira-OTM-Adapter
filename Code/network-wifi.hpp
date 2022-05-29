#pragma once
#include "x-header.hpp"

extern String hostname;

void wlan_connect();
void scan_wifi_ssid();
void wlan_config();

IPAddress ipwandeln(String temp);
