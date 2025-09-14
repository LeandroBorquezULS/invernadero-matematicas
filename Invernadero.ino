#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int rele_agua = 16, rele_ventilador = 17, rele_calefactor = 25;
const int sensor_humedad = 34; // ADC1
const int transistor_humedad = 27;    // controla transistor

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(rele_agua, OUTPUT);
  pinMode(rele_ventilador, OUTPUT);
  pinMode(rele_calefactor, OUTPUT);
  digitalWrite(rele_calefactor, HIGH);
  digitalWrite(rele_ventilador, HIGH);
  digitalWrite(rele_calefactor, HIGH);

  // transistor: salida con pull-down interno
  pinMode(transistor_humedad, OUTPUT);
  gpio_pulldown_en(GPIO_NUM_27);   // fuerza base a GND en arranque
  digitalWrite(transistor_humedad, LOW); // transistor apagado al inicio

  // ADC: resolución por defecto 12 bits (0–4095)
  analogReadResolution(12);
}

void loop() {
  // DHT22
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();
  if (!isnan(humedad) && !isnan(temperatura)) {
    Serial.printf("T=%.1fC  H=%.1f%%\n", temperatura, humedad);
  } else {
    Serial.println("Error DHT");
  }
  if (temperatura <= 20.0){
    digitalWrite(rele_calefactor, HIGH);
    Serial.println("calefactor on");
  } else {
    digitalWrite(rele_calefactor, LOW);
  }

  if (temperatura >=23.0){
    digitalWrite(rele_ventilador, HIGH);
    Serial.println("ventilador on");
  } else {
    digitalWrite(rele_ventilador, LOW);
  }

  // Suelo
  digitalWrite(transistor_humedad, HIGH);    // enciende sensor
  delay(1000);                           // breve estabilización
  int raw = analogRead(sensor_humedad); // 0 mojado, 4095 seco (calibra)
  digitalWrite(transistor_humedad, LOW);     // apaga sensor
  Serial.printf("Suelo raw=%d\n", raw);

  if (raw > 3500){
    digitalWrite(rele_agua, HIGH);  // ON
    Serial.println("agua on");
    delay(2000);
    digitalWrite(rele_agua, LOW);
  } else {
    digitalWrite(rele_agua, LOW); // OFF
  }
  delay(30000);
}

