#if defined(ESP32)
#include "esp32BLEUtilities.h"
bool BLE_status=0;

bool isDeviceConnected=false;
bool prevDeviceConnected = false;
uint8_t tx_Value = 0;
uint8_t rxdatalength_pri = 0;
uint8_t rxdatalength_alt = 0;
uint8_t bytesremaining_pri=0;
uint8_t bytesremaining_alt=0;
uint8_t rxdataBuffer_pri[256];// = NULL;
uint8_t rxdataBuffer_alt[256];
bool newDataReceived = 0;
unsigned long last_rx = 0;
bool read_finished_pri = false;
bool read_finished_alt = false;



unsigned time_bw_rx = 300;
uint8_t * rxdataBuffer_write = rxdataBuffer_pri;
uint8_t * rxdataBuffer_read = rxdataBuffer_pri;
BLEServer *bleServer = NULL;
BLECharacteristic *bleTxCharacteristic;


class BleServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* bleServer) {
      isDeviceConnected = true;
    };

    void onDisconnect(BLEServer* bleServer) {
      isDeviceConnected = false;
    }
};

class BleCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *bleCharacteristic) {
      std::string rx_Value = bleCharacteristic->getValue();
	  if(millis() - last_rx < time_bw_rx && last_rx != 0)
		  return;
	 last_rx = millis();

	  // If there is any data available
	  if (rx_Value.length() > 0) {
		  // If next buffer is pri
	   if ( rxdataBuffer_write == rxdataBuffer_pri)
	   {
		   // If the read has not finished
		   if (bytesremaining_pri != 0)
		   {
			   return;
		   }
		   // If next buffer is alt
	   }else if( rxdataBuffer_write == rxdataBuffer_alt)
	   {
		   // If the read has finished
		   if (bytesremaining_alt != 0)
		   {
			   return;
		   }
	   }
        for (int i = 0; i < rx_Value.length(); i++)
		{
		  rxdataBuffer_write[i] = rx_Value[i];
		}
				  // Just filled pri buffer
		  if (rxdataBuffer_write == rxdataBuffer_pri)
		  {   
			//Serial.println("Changing WRITE buffer to ALT");
		      bytesremaining_pri = rx_Value.length();
			  rxdatalength_pri = rx_Value.length();
			  rxdataBuffer_write = rxdataBuffer_alt;
		  }
		  else if(rxdataBuffer_write == rxdataBuffer_alt)
		  {
			  //Serial.println("Changing WRITE buffer to PRI");
			  bytesremaining_alt = rx_Value.length();
			  rxdatalength_alt = rx_Value.length();
			  rxdataBuffer_write = rxdataBuffer_pri;
		  }
		  
      }
    }
};

void Esp32ble::begin(std::string a)
{
  BLEDevice::init(a);

  // Create the BLE Server
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new BleServerCallbacks());

  // Create the BLE Service
  BLEService *bleService = bleServer->createService(UUID_Service);

  // Create a BLE Characteristic
  bleTxCharacteristic = bleService->createCharacteristic(
                    UUID_Transmit,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
                      
  bleTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * bleRxCharacteristic = bleService->createCharacteristic(
                       UUID_Receive,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  bleRxCharacteristic->setCallbacks(new BleCallbacks());

  // Start the service
  bleService->start();

  // Start advertising
  bleServer->getAdvertising()->start();
  #ifdef DEBUG
  Serial.println("Waiting a client connection to notify...");
  #endif
}

void Esp32ble::write(uint8_t a)
{
	 if (isDeviceConnected) {
        bleTxCharacteristic->setValue(&a,1);
        bleTxCharacteristic->notify();
	    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        bleServer->startAdvertising(); // restart advertising
		#ifdef DEBUG
        Serial.println("start advertising");
		#endif
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(std::string x)
{
	 if (isDeviceConnected) {
        bleTxCharacteristic->setValue(x);
        bleTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        bleServer->startAdvertising(); // restart advertising
		#ifdef DEBUG
        Serial.println("start advertising");
		#endif
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(int a)
{
	 if (isDeviceConnected) {
        bleTxCharacteristic->setValue(a);
        bleTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        bleServer->startAdvertising(); // restart advertising
		#ifdef DEBUG
        Serial.println("start advertising");
		#endif
        prevDeviceConnected = isDeviceConnected;
    }
}

void Esp32ble::write(float a)
{
	 if (isDeviceConnected) {
        bleTxCharacteristic->setValue(a);
        bleTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!isDeviceConnected && prevDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        bleServer->startAdvertising(); // restart advertising
		#ifdef DEBUG
        Serial.println("start advertising");
		#endif
        prevDeviceConnected = isDeviceConnected;
    }
}

uint8_t Esp32ble::available()
{
	if (rxdataBuffer_read == rxdataBuffer_pri)
	{
		return bytesremaining_pri;
	}
	else if (rxdataBuffer_read == rxdataBuffer_alt)
	{
		return bytesremaining_alt;
	}
	return 0;
}
uint8_t Esp32ble::read()
{
	uint8_t a;
	if ( rxdataBuffer_read == rxdataBuffer_pri)
    {
		   if(bytesremaining_pri > 0)
		   {
			   a = rxdataBuffer_pri[rxdatalength_pri - bytesremaining_pri];
			   bytesremaining_pri--;
			   if (bytesremaining_pri == 0)
			   {
				   //Serial.println("Changing READ buffer to ALT");
					rxdataBuffer_read = rxdataBuffer_alt;
			   }
			   return a;
		   }else
		   {
			   return 0;
		   }
    }else if ( rxdataBuffer_read == rxdataBuffer_alt)
    {
		   if(bytesremaining_alt > 0)
		   {
			   a = rxdataBuffer_alt[rxdatalength_alt - bytesremaining_alt];
			   bytesremaining_alt--;
			   if (bytesremaining_alt == 0)
			   {
				   //Serial.println("Changing READ buffer to PRI");
					rxdataBuffer_read = rxdataBuffer_pri;
			   }
			   return a;
		   }else
		   {
			   return 0;
		   }
    }
}

void Esp32ble::stop()
{
	btStop();
}

Esp32ble esp32ble;
#endif
