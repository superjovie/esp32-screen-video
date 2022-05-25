


#include <Arduino.h>
#include <M5StickCPlus.h>
#include "View.h"
#include "TimeView.h"
#include "AnalogClock.h"
#include "MideaView.h"
#include "GreeView.h"
#include "DiceView.h"
#include "IrView.h"
#include <TJpg_Decoder.h>

#include "time.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "power.h"
#define NTP_SERVER "pool.ntp.org"
#define TIME_ZONE_OFFSET_HRS +8
#include "tasks.h"


#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define LED_BUILTIN 10

// Set these to your desired credentials.
const char *ssid = "esp32wifi";
const char *password = "11111111";
const char *mdnsname = "esp32wifi";

String ipaddress = "";
String savedSSID = "";
String savedPASS = "";
String networks = "";




static const char *TAG = "MAIN";

View *active_view;
uint8_t state = 1;
unsigned long rst_clicked_time = millis();
unsigned long home_clicked_time = millis();
unsigned long active_time = millis();

TASKS::Dimmer dimmer;
TASKS::IMUManager imumgr;
int32_t flags = 0;

#define M5_BUTTON_RESET_PRESSED 0x01
#define M5_BUTTON_HOME_PRESSED 0x02
#define M5_BUTTON_POWER_PRESSED 0x04
WiFiMulti wifiMulti;
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void reset_buttton_pressed()
{
    if (!(rst_clicked_time + 500 > millis()))
    {
        ESP_LOGD(TAG, "Reset Buttion Pressed");

        rst_clicked_time = millis();
        flags = flags | M5_BUTTON_RESET_PRESSED;
    }
}

void home_button_pressed()
{
    if (!(home_clicked_time + 500 > millis()))
    {
        ESP_LOGD(TAG, "Home Button Pressed");
        home_clicked_time = millis();
        flags = flags | M5_BUTTON_HOME_PRESSED;
    }
}

void power_button_pressed()
{
    ESP_LOGD(TAG, "Power Button Pressed");
    flags = flags | M5_BUTTON_POWER_PRESSED;
}

void blinkenLight()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
    M5.begin();
    Serial.begin(2000000);
    pinMode(LED_BUILTIN, OUTPUT);
    wifiMulti.addAP("superjovie", "147337332");
    wifiMulti.addAP("Superjovie", "147337332");
    wifiMulti.addAP("ChinaNet-34Lt", "db5rrlet");
    wifiMulti.addAP("lyc1", "18981390616");
    wifiMulti.addAP("lyc2", "18981390616");
    wifiMulti.addAP("D-Link_DIR-809", "21005370");
    if(wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else{
      WiFiManager wifiManager;
      //wifiManager.setConfigPortalBlocking(false);
      wifiManager.setConfigPortalTimeout(60);
      wifiManager.setAPCallback(configModeCallback);
      wifiManager.autoConnect("esp32wifi","147337332");
    }
    digitalWrite(LED_BUILTIN, HIGH);
    
    M5.Lcd.setRotation(3);
    M5.Imu.Init();
    M5.Axp.ScreenBreath(9);
    synchronizeTime();
    ESP_LOGD(TAG, "%d", M5.Axp.Read8bit(0x28) >> 4);

    active_view = new AnalogClock();

    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);//解码成功回调函数
}


void loop()
{
    M5.update();
   
    // render view
    active_view->render();
    
    //event_loop();
    // no dimming as long as in shake
    if (imumgr.is_moved())
    {
        dimmer.recover();
        active_time = millis();
    }

    // auto dimming timed
    if (active_time + 10000 < millis())
    {
        dimmer.go_dim();
    }

    // auto screenoff timed
    if (active_time + 30000 < millis())
    {
        dimmer.go_dark();
    }

    // capture events
    if (M5.Axp.GetBtnPress() == 2)
    {
        state++;

        if (state > 6)
        {
            state = 1;
        }

        delete active_view;
        active_view = NULL;

        switch (state)
        {
        case 1:
            active_view = new AnalogClock();
            break;
        case 2:
            active_view = new TimeView();
            break;
        case 3:
            active_view = new MideaView();
            break;
        case 4:
            active_view = new GreeView();
            break;
        case 5:
            active_view = new IrView();
            break;
        case 6:
            active_view = new DiceView();
            
            break;
       

        default:
            break;
        }
    }

    if (M5.Axp.GetBtnPress() == 0x02)
    {
        power_button_pressed();
    }

    

    // propagate events to view
    if (flags > 0)
    {
        if (dimmer.is_dim())
        {
            dimmer.recover();
            active_time = millis();
            flags = 0;
        }
        else if(!dimmer.dim_exitting())
        {
            
            flags = 0;
        }
        else
        {
            flags = 0;
        }
    }
    
    
}




const char *setMessage(const char *message)
{
  static const char *previousMessage = "";
  M5.Lcd.fillScreen(BLACK);
  
  M5.Lcd.drawString(message,80,40,2);

  const char *result = previousMessage;
  previousMessage = message;
  return result;
}

void setRtcTime(tm &timeInfo)
{
  RTC_TimeTypeDef newTime;
  newTime.Hours = timeInfo.tm_hour;
  newTime.Minutes = timeInfo.tm_min;
  newTime.Seconds = timeInfo.tm_sec;
  M5.Rtc.SetTime(&newTime);

  RTC_DateTypeDef newDate;
  newDate.WeekDay = timeInfo.tm_wday;
  newDate.Month = timeInfo.tm_mon + 1;
  newDate.Date = timeInfo.tm_mday;
  newDate.Year = timeInfo.tm_year + 1900;
  M5.Rtc.SetData(&newDate);
}




void synchronizeTime()
{
  //connectWifi();

  const char *resetMessage = setMessage("Synchronizing");
  configTime(TIME_ZONE_OFFSET_HRS * 3600, 0, NTP_SERVER);
  struct tm timeInfo;
  if (getLocalTime(&timeInfo))
  {
    setRtcTime(timeInfo);
  }
  delay(500);
  setMessage(resetMessage);
}




bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= M5.Lcd.height() ) return 0;
  M5.Lcd.pushImage(x, y, w, h, bitmap); // Initiate DMA - blocking only if last DMA is not complete
  return 1;
}
