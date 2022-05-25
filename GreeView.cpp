#include <M5StickCPlus.h>
#include "View.h"
#include "GreeView.h"
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "driver/gpio.h"
static const char *TAG = "GREE_VIEW";

uint8_t Temp = 22;


GreeView::GreeView()
{   
    
    M5.begin();
    M5.Lcd.setRotation(0);
    M5.Lcd.setSwapBytes(false);
    M5.Axp.EnableCoulombcounter();
    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    pinMode(10, OUTPUT);
    // set mode, temperature and fan level and internal state to enabled
    acgree = new IRGreeAC(9);
    acgree->begin();
    
    acgree->on();
    //acgree.setUseFahrenheit(false);
    acgree->setFan(3);
    // kGreeAuto, kGreeDry, kGreeCool, kGreeFan, kGreeHeat
    acgree->setMode(kGreeCool);
    acgree->setTemp(Temp);  // 16-30C
    acgree->setSwingVertical(true, kGreeSwingAuto);
    acgree->setXFan(false);
    acgree->setLight(false);
    acgree->setSleep(false);
    acgree->setTurbo(false);

    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(135, 240);
    disp_buffer->fillRect(0, 0, 135, 240, BLACK);
    disp_buffer->pushSprite(0, 0);
    disp_buffer->setTextSize(2);
    disp_buffer->setTextColor(TFT_RED, TFT_BLACK);
    disp_buffer->drawString("Gree", 40, 200, 2);
    disp_buffer->setTextSize(5);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
    
    
}

GreeView::~GreeView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    delete acgree;
    ESP_LOGD(TAG, "Destructor finished");
    gpio_reset_pin(GPIO_NUM_9);
    //acgree.off();
}

void GreeView::render()
{
    M5.update();
    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {
        if(Temp==30){Temp = 18; acgree->setTemp(Temp);}
        else{Temp++;acgree->setTemp(Temp);}
        
        acgree->send();
        blinkenLight();
        
    }
    
    delay(100);
    disp_buffer->setTextSize(3);
    disp_buffer->setTextColor(grey, TFT_BLACK);
    if (M5.BtnB.isPressed())
    {
        cases++;
       
        if(cases == 3){cases = 0;}
        
    }
    switch(cases)
    {
    case 0:
      acgree->setMode(kGreeHeat);
      acgree->on();
      disp_buffer->drawString("HAET", 25, 25, 2);
      break;
    case 1:
      acgree->setMode(kGreeCool);
      acgree->on();
      disp_buffer->drawString("COOL", 25, 25, 2);
      break;
    case 2:
      acgree->off();
      
      disp_buffer->drawString("OFF ", 25, 25, 2);
      break;
    default:
      break;
    }
    
    disp_buffer->setTextSize(5);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
    disp_buffer->drawString(String(Temp), 30, 110, 2);

    disp_buffer->pushSprite(0, 0);
    
}



void GreeView::blinkenLight()
{
    digitalWrite(10, LOW);
    delay(100);
    digitalWrite(10, HIGH);
}
