class timeSt { // работа с timestamp
public:
  time_t timeS;
  timeSt() {}
  void timeSetting(const char* ntpServer = "pool.ntp.org", const long gmtOffset_sec = 3 * 3600, const int daylightOffset_sec = 0) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
  void timeStam() {
    time(&timeS);  // Получаем текущее UNIX-время
  }
};