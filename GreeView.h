#pragma once

#include <M5StickCPlus.h>
#include "View.h"
#include <ir_Gree.h>

#define grey 0x65DB

    
class GreeView : public View
{
public:
    GreeView();
    ~GreeView();

    void render();
    bool receive_event(EVENTS::event event){ return false; };

private:
    
    void blinkenLight();
    IRGreeAC *acgree;
    int32_t cases = 0;
    TFT_eSprite *disp_buffer;
    
};
