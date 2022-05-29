#include "network-bluetooth.hpp"

BLEScan *pBLEScan;

void bluetooth_config()
{
    Serial.println("Starte BLE Scanner");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Aktives Bluetooth Scannen
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void bluetooth_scan()
{
    for (;;)
    {
        BLEScanResults foundDevices = pBLEScan->start(10); // BLE Scanen

        Serial.println();
        Serial.print("Bluetooth- Scan auf Core : ");
        Serial.println(xPortGetCoreID());
        int j;
        for (j = 0; j < foundDevices.getCount(); j++) // Alle Gefundene Bluetooth Device durchgehen
        {
            Serial.print(foundDevices.getDevice(j).getAddress().toString().c_str());
            Serial.print(" | ");
            Serial.println(foundDevices.getDevice(j).getRSSI());
        }

        pBLEScan->clearResults();
    }
}