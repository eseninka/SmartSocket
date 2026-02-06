#include "ZMPT101B.h"
#include "ACS712.h"

class read_AC {
private:
  ACS712 sensor;
  ZMPT101B voltageSensor;
public:
  read_AC()
    : sensor(32, 3.3, 4095, 160.0), voltageSensor(35, 50.0) {}
  int current_signal[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  float current_A[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int voltage_signal[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  float voltage_V[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int rms_mA = 0;
  float rms_A = 0;
  float rms_voltage = 0;


  void initialization() {
    voltageSensor.setSensitivity(526.25);
    sensor.autoMidPoint();
  }

  float RMS(float arr[20]) {
    float sum = 0;
    for (int i = 0; i < 20; i++) {
      sum += pow(arr[i], 2);
    }
    float sumMean = sum / 20;
    return sqrt(sumMean);
  }

  void read_current() {
    for (int i = 0; i < 20; i++) {
      current_signal[i] = analogRead(32);
      delayMicroseconds(900);
    }
    for (int i = 0; i < 20; i++) {
      current_A[i] = (current_signal[i] - 2837) * 0.00435;
    }
    rms_mA = sensor.mA_AC_sampling();
    rms_A = rms_mA / 1000.0;
  }

  void read_voltage() {
    for (int i = 0; i < 20; i++) {
      voltage_signal[i] = analogRead(35);
      delayMicroseconds(900);
    }
    for (int i = 0; i < 20; i++) {
      voltage_V[i] = (voltage_signal[i] - 2800) * 0.41;
    }
    rms_voltage = voltageSensor.getRmsVoltage();
  }
};