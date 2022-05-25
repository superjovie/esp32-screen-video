#include <M5StickCPlus.h>
#include "View.h"
#include "TimeView.h"

#include "driver/gpio.h"
static const char *TAG = "TIME_VIEW";

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern String days[7];

TimeView::TimeView()
{
    M5.begin();
    
    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);
     
    
    disp_buffer = new TFT_eSprite(&M5.Lcd);
    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, TFT_WHITE);
    //disp_buffer2->pushImage(0, 0, 160, 80, clockC);
    disp_buffer->pushSprite(0, 0);
}

TimeView::~TimeView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
    gpio_reset_pin(GPIO_NUM_9);
}

void TimeView::render()
{
    M5.update();
    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_BLACK, TFT_WHITE);
    
    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {   
        //switchmidea();
        //blinkenLight();
    }
    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);
    disp_buffer->setCursor(0, 15);
    disp_buffer->setTextFont(0);
    disp_buffer->setFreeFont(&DSEG7_Classic_Regular_64);
    pinMode(10, OUTPUT);
    // if time has changed
    if (h_cache != RTC_TimeStruct.Hours || m_cache != RTC_TimeStruct.Minutes)
    {
        h_cache = RTC_TimeStruct.Hours;
        m_cache = RTC_TimeStruct.Minutes;
        sprintf(timeHHMM, "%02d:%02d", h_cache, m_cache);
        ESP_LOGD(TAG, "Updating time to - %s", timeHHMM);
        disp_buffer->drawString(timeHHMM, 5, 60);

        
    }

    // if seconds has changed
    if (s_cache != RTC_TimeStruct.Seconds)
    {
        s_cache = RTC_TimeStruct.Seconds;
        sprintf(seconds, "%02d", s_cache);
        ESP_LOGD(TAG, "Updating seconds to - %s", seconds);
        disp_buffer->setFreeFont(&DSEG7_Classic_Bold_30);
        disp_buffer->drawString(seconds, 175, 5);
    }

    // if day has changed
    if (d_cache != RTC_DateStruct.WeekDay)
    {
        d_cache = RTC_DateStruct.WeekDay;
        sprintf(day, "%-10s", days[d_cache].c_str());
        disp_buffer->drawString(day, 10, 5, 4);
    }

    // update date if its changed
    if (dt_cache != RTC_DateStruct.Date || mn_cache != RTC_DateStruct.Month || yr_cache != RTC_DateStruct.Year)
    {
        dt_cache = RTC_DateStruct.Date;
        mn_cache = RTC_DateStruct.Month;
        yr_cache = RTC_DateStruct.Year;
        disp_buffer->setTextColor(TFT_RED, TFT_WHITE);
        disp_buffer->drawString(String(dt_cache) + "/" + String(mn_cache), 10, 30, 4);
        disp_buffer->drawString(String(yr_cache), 75, 35, 2);
        disp_buffer->setTextColor(TFT_BLACK, TFT_WHITE);
    }   
    disp_buffer->pushSprite(0, 0);
    
}



void TimeView::blinkenLight()
{
    digitalWrite(10, LOW);
    delay(100);
    digitalWrite(10, HIGH);
}
