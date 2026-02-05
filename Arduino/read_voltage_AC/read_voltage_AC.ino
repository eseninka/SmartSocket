#include "ZMPT101B.h"

ZMPT101B voltageSensor(35, 50.0);  // Пин подключения датчика

int voltage_signal[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float voltage_V[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


float RMS(float arr[20]) {
  float sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += pow(arr[i], 2);
  }
  float sumMean = sum / 20;
  return sqrt(sumMean);
}

void setup() {
  Serial.begin(115200);
  voltageSensor.setSensitivity(526.25);
}

void loop() {
  for (int i = 0; i < 20; i++) {
    voltage_signal[i] = analogRead(35);
    delayMicroseconds(900);
  }
  for (int i = 0; i < 20; i++) {
    voltage_V[i] = (voltage_signal[i] - 2800) * 0.41;
  }
  for (int i = 0; i < 20; i++) {
    Serial.printf("Измерение напряжения %d\t",i + 1);
    Serial.print(voltage_V[i]);
    Serial.print("\t");
    Serial.println(voltage_signal[i]);
  }

  float rms_voltage = voltageSensor.getRmsVoltage();
  Serial.print("RMS по библиотеке:");
  Serial.println(rms_voltage, 3);
  Serial.print("RMS мгновенным:");
  Serial.println(RMS(voltage_V), 3);
  Serial.println();
  delay(1000);

}