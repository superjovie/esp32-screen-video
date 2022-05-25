#pragma once

#include <M5StickCPlus.h>
#include "View.h"
#include <midea_ir.h>

#define grey 0x65DB

class MideaView : public View
{
public:
    MideaView();
    ~MideaView();

    void render();
    bool receive_event(EVENTS::event event){ return false; };
    
private:
    float accX = 0;
    float accY = 0;
    float accZ = 0;
    void blinkenLight();
    const uint8_t MIDEA_RMT_CHANNEL = 0;
    const uint8_t MIDEA_TX_PIN = 9;
    MideaIR ir;
    int32_t cases = 0;

    TFT_eSprite *disp_buffer;
};
