#include <M5StickCPlus.h>
#include "View.h"
#include "MideaView.h"
#include <midea_ir.h>
#include "driver/gpio.h"

static const char *TAG = "MIDEA_VIEW";

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern String days[7];

MideaView::MideaView()
{
    M5.begin();
    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    M5.Lcd.setRotation(0);
    M5.Lcd.setSwapBytes(false);
    M5.Axp.EnableCoulombcounter();
    midea_ir_init(&ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN);
    ir.enabled = true;
    ir.mode = MODE_HEAT;
    ir.fan_level = 3;
    ir.temperature = 27; 
    pinMode(10, OUTPUT);


    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(135, 240);
    disp_buffer->fillRect(0, 0, 135, 240, BLACK);
    disp_buffer->pushSprite(0, 0);
    disp_buffer->setTextSize(2);
    disp_buffer->setTextColor(TFT_RED, TFT_BLACK);
    disp_buffer->drawString("Midea", 30, 200, 2);
    
    disp_buffer->setTextSize(5);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
}

MideaView::~MideaView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    
    ESP_LOGD(TAG, "Destructor finished");
 
    midea_ir_stop(MIDEA_RMT_CHANNEL);
    
    ir.~MideaIR();
    gpio_reset_pin(GPIO_NUM_9);
}

void MideaView::render()
{
    M5.update();
    
    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {
        if(ir.temperature==30){ir.temperature=18;}
        else{ir.temperature++;}
        
        midea_ir_send(&ir);
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
      ir.mode=MODE_HEAT;
      ir.enabled=true;
      disp_buffer->drawString("HAET", 25, 25, 2);
      break;
    case 1:
      ir.mode=MODE_COOL;
      ir.enabled=true;
      disp_buffer->drawString("COOL", 25, 25, 2);
      break;
    case 2:
      ir.enabled=false;
      
      disp_buffer->drawString("OFF ", 25, 25, 2);
      break;
    default:
      break;
    }
    
  
    disp_buffer->setTextSize(5);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
    disp_buffer->drawString(String(ir.temperature), 30, 110, 2);

    disp_buffer->pushSprite(0, 0);
    
}




void MideaView::blinkenLight()
{
    digitalWrite(10, LOW);
    delay(100);
    digitalWrite(10, HIGH);
}
