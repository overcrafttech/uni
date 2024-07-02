#include <Wire.h>

//i2c Setup
#define i2c_sda 12 
#define i2c_scl 13
#define i2c_speed 400000
#define sensirion_address 0x44

//Serial Setup
#define baud_rate 9600

uint16_t temperature_raw = 0;
uint16_t humidity_raw = 0;
float temperature = 0;
float humidity = 0;


void setup() {
  //Serial Setup
  Serial.begin(baud_rate);
  //while(!Serial.available()){
  //  ;
  //}
  Serial.println("USB serial port connected.");

  //i2c Setup
  Serial.print("Configuring software-i2c with ");
  Serial.print(i2c_sda, DEC);
  Serial.print(" as sda and ");
  Serial.print(i2c_scl, DEC);
  Serial.println(" as scl. ");
  Wire.begin(i2c_sda, i2c_scl);
  //Wire.setClock(i2c_speed);

  //pin Setup
  Serial.println("Pin setup");
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Initiate measurements");
  Wire.beginTransmission(sensirion_address);
  Wire.write(0x27); //start periodic, 10 mps
  Wire.write(0x37); //high repeatibility
  Wire.endTransmission();

  Serial.println("Disable Heater");
  Wire.beginTransmission(sensirion_address);
  Wire.write(0x30); //start periodic, 10 mps
  Wire.write(0x66); //high repeatibility
  Wire.endTransmission();
}

void loop() {
  read_data();
  Serial.print("Temperatur: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(humidity);
  Serial.println("%");
  delay(2000);
}

uint8_t read_data(){
  uint8_t data[6] = {0};
  Wire.beginTransmission(sensirion_address);
  Wire.write(0xE0); //data fetch command
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(sensirion_address,6);

  data[0] = Wire.read();// temp MSB
  data[1] = Wire.read();// temp LSB
  data[2] = Wire.read();// temo CRC

  data[3] = Wire.read();// humi MSB
  data[4] = Wire.read();// humi LSB
  data[5] = Wire.read();// humi CRC

  temperature_raw = (data[0] << 8) | data[1];
  temperature = -45.0 + 175.0 * (float(temperature_raw)/65535.0);

  humidity_raw = (data[3] << 8) | data[4];
  humidity = 100 * (float(humidity_raw)/65535.0 );

  return 0;
}