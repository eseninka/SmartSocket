#include "ACS712.h"

ACS712 sensor(32, 3.3, 4095, 160.0);

int current_signal[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float current_A[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int rms_mA = 0;

float RMS(float arr[10]) {
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += pow(arr[i], 2);
  }
  float sumMean = sum / 10;
  return sqrt(sumMean);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  sensor.autoMidPoint();
}

void loop() {
  for (int i = 0; i < 10; i++) {
    current_signal[i] = analogRead(32);
    delayMicroseconds(1900);
  }
  for (int i = 0; i < 10; i++) {
    current_A[i] = (current_signal[i] - 2837) * 0.00435;
  }
  for (int i = 0; i < 10; i++) {
    Serial.printf("Измерение %d\t",i + 1);
    Serial.println(current_A[i]);
  }

  int rms_mA = sensor.mA_AC_sampling();
  float rms_A = rms_mA / 1000.0;
  Serial.print("RMS по библиотеке:");
  Serial.println(rms_A, 3);
  Serial.print("RMS мгновенным:");
  Serial.println(RMS(current_A), 3);
  Serial.println();
  delay(1000);
}