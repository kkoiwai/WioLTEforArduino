#include <WioLTEforArduino.h>

WioLTE Wio;

void setup()
{
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  GpsBegin(&Serial);
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyGrove(true);
  delay(5000);
}

void loop()
{
  const char* data = GpsRead();
  if (data != NULL && strncmp(data, "$GPGGA,", 7) == 0) {
    SerialUSB.println(data);
  }
}

////////////////////////////////////////////////////////////////////////////////////////
//

HardwareSerial* GpsSerial;
char GpsData[60];
char GpsDataLength;

void GpsBegin(HardwareSerial* serial)
{
  GpsSerial = serial;
  GpsSerial->begin(9600);
  GpsDataLength = 0;
}

const char* GpsRead()
{
  while (GpsSerial->available()) {
    char data = GpsSerial->read();
    if (data == '\r') continue;
    if (data == '\n') {
      GpsData[GpsDataLength] = '\0';
      GpsDataLength = 0;
      return GpsData;
    }
    
    GpsData[GpsDataLength++] = data;
    if (GpsDataLength >= sizeof (GpsData) - 1) {  // Overflow
      GpsDataLength = 0;
    }
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

