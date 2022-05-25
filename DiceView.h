#pragma once

#include <M5StickCPlus.h>
#include "View.h"



    
class DiceView : public View
{
public:
    DiceView();
    ~DiceView();
    
    void render();
    bool receive_event(EVENTS::event event){ return false; };
    void loading(byte delayTime);
    
    
    int httpPort = 8081; //设置监听端口
    
    uint8_t buff[7000] PROGMEM= {0};//每一帧的临时缓存
    uint8_t img_buff[50000] PROGMEM= {0};//用于存储tcp传过来的图片
    uint16_t size_count=0;//
    byte loadNum = 26;
    uint16_t read_count=0;//读取buff的长度
    uint8_t pack_size[2];//用来装包大小字节
    uint16_t frame_size;//当前帧大小
    float start_time,end_time;//帧处理开始和结束时间
    float receive_time,deal_time;//帧接收和解码时间
    
private:
    
    
    
    
    


};
