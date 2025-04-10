#ifndef MY_WIFI_MANAGER_HPP
#define MY_WIFI_MANAGER_HPP

class MyWiFiManager {
public:
  static MyWiFiManager& getInstance();
  void begin();
private:
  MyWiFiManager();
};

#endif
