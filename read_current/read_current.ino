#include "ACS712.h"

ACS712 sensor(A0, 5.0, 1023, 185);

void setup() {
  Serial.begin(115200);
  sensor.autoMidPoint();
}

void loop() {
  // Мгновенное значение через analogRead
  int raw = analogRead(A0);                        // 0-1023
  float voltage = raw * (5.0 / 1023.0);            // 0-5В
  float instantCurrent = (voltage - 2.5) / 0.185;  // В Амперах

  // RMS значение через библиотеку
  int rms_mA = sensor.mA_AC_sampling();
  float rms_A = rms_mA / 1000.0;

  Serial.print(instantCurrent, 3);
  Serial.print("\t");
  Serial.println(rms_A, 3);

}