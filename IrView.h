#pragma once
#include <M5StickCPlus.h>
#include "View.h"
#include <Arduino.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <IRsend.h>
#include <esp_err.h>
#include <esp_log.h>

#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"

#define grey 0x65DB
#define STORAGE_NAMESPACE "storage"
    
class IrView : public View
{
public:
    IrView();
    ~IrView();

    void render();
    bool receive_event(EVENTS::event event){ return false; };

private:
    
    void blinkenLight();
    esp_err_t nvs_write_wifi(struct decode_results *res, uint32_t len, const char *head);
    esp_err_t nvs_read_wifi(struct decode_results *res, const char *head);
    void send_nvs(const char *head);
    uint8_t count = 0;
    const uint16_t kRecvPin = 33;
    const uint32_t kBaudRate = 115200;
    const uint16_t kCaptureBufferSize = 1024;
    const uint8_t kTimeout = 50;
    const uint16_t kMinUnknownSize = 12;
    #define LEGACY_TIMING_INFO false
    const uint8_t kTolerancePercentage = kTolerance;
    uint32_t nvs_handle;
    decode_type_t protocol;
    uint16_t size;
    bool success;
    IRrecv *irrecv;
    decode_results results; 
    IRsend *irsend;
    TFT_eSprite *disp_buffer;
    
};
