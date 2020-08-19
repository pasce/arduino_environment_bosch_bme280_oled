/* 
 * Environment sensor: Bosch Sensortec BME280
 * Measures relative humidity, barometric pressure and ambient temperature
 * 
 * Display: MakerHawk OLED Display 128x64
 * 
 * Used protocol: I2C
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 1013.25 hPa: Normalized sea level pressure (760 mm Hg)
#define SEALEVELPRESSURE_HPA (1013.25) //(1015.53) //(1014.9)//(1014.7)//(1013.25)

Adafruit_BME280 bme;

Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

#define SERIAL_PORT_BOUD_RATE 9600 //<- change if you use a different boud rate for serial communication

unsigned long delayTime;

void setup(void)
{
  Serial.begin(SERIAL_PORT_BOUD_RATE);
    
  Serial.println("Maxim DS1820 and Bosch BMP280 (Temperature, Humidty & Altitude) Example");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.display();
  delay(2000);
  display.clearDisplay();

  // setup bosch bme280
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // weather monitoring mode -- Weather Station Scenario of BME280--
  // forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"); ("filter off");
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
                      
  // suggested rate is 1/60Hz (1m)
  delayTime = 60000;
  
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  Serial.println();

} 

void loop(void)
{ 
  
  bme.takeForcedMeasurement(); // needed only in forced mode

  float temp = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float height = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();

  Serial.print(F("Sensor B: "));
  Serial.print(temp);
  Serial.println(F("°C"));
  
  Serial.println();
  Serial.print(F("Druck = "));
  Serial.print(pressure); //Readings are not normalized to sea-level. TODO: Use formular
  Serial.println(F("hPa"));

  Serial.print(F("Ungefähre Höhe = "));
  Serial.print(height);
  Serial.println(F("m"));

  Serial.print(F("Luft = "));
  Serial.print(humidity);
  Serial.println(F("%"));
  Serial.println();

  // print to the oled display
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.cp437(true); 

  display.print(F("T: "));
  display.print(temp);
  display.println(F(" C"));

  display.print(F("D: "));
  display.print(pressure);
  display.println("");//display.println(F(" hPa"));
  
  display.print(F("L: "));
  display.print(humidity);
  display.println(F(" %"));

  display.display();
  
  delay(delayTime);
}
