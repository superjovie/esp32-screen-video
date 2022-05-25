/*

A couple of dices on a tiny 80x160px TFT display

Author: Alfonso de Cala <alfonso@el-magnifico.org>

*/

#include <M5StickCPlus.h>
#include <SPI.h>
#include <WiFi.h>
#include <TJpg_Decoder.h>
#include <pgmspace.h>
#include "DiceView.h"
#include "View.h"
static const char *TAG = "DICE_VIEW";
WiFiServer server1; //初始化一个服务端对象


void DiceView::loading(byte delayTime){//启动动画
  
  
  
  M5.Lcd.fillScreen(0x0000);

  M5.Lcd.drawRoundRect(20,0,220,16,8,0xFFFF);
  M5.Lcd.fillRoundRect(23,3,loadNum,10,5,0xFFFF);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextColor(TFT_GREEN, 0x0000); 
  M5.Lcd.drawString("Connecting to WiFi",120,40,2);
  
  loadNum += 1;
  if(loadNum>=195){
    loadNum = 195;
  }
  delay(delayTime);
}


DiceView::DiceView() {
  
  M5.begin();
  
  M5.Lcd.setRotation(3);//横屏
  M5.Lcd.fillScreen(TFT_BLACK);//黑色
  M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
  
  delay(1000); //等待1秒
  if (WiFi.status() != WL_CONNECTED) {
    for(byte n=0;n<10;n++){ //每500毫秒检测一次状态
      loading(50);
    }
  }
   while(loadNum < 195){ //让动画走完
    loading(3);
  }
  if (WiFi.status() == WL_CONNECTED) //判断如果wifi连接成功
  { 
    
   
    //client.setNoDelay(false);//关闭Nagle算法
    Serial.println("wifi is connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println("Port: "+String(httpPort));
    M5.Lcd.fillScreen(TFT_BLACK);//黑色
    M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
    M5.Lcd.setCursor(0, 20, 2);
    M5.Lcd.println("  WiFi Connected:"+String(WiFi.SSID())+ "  ");
    M5.Lcd.setCursor(0, 50, 2);
    M5.Lcd.println("  IP: "+ip.toString()+ "  ");
    M5.Lcd.setCursor(0, 80, 2);
    M5.Lcd.println("  Port: "+String(httpPort)+ "  ");
    
    
    Serial.println("Waiting a client to connect.........");
    server1.begin(httpPort); //服务器启动监听端口号
    server1.setNoDelay(true);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    M5.Lcd.fillScreen(TFT_BLACK);//黑色
    M5.Lcd.setCursor(20, 20, 2);
    M5.Lcd.println(" WiFi not connect ");
    M5.Lcd.setCursor(20, 50, 2);
    M5.Lcd.println(" connect to WiFi esp32wifi ");
    M5.Lcd.setCursor(20, 80, 2);
    M5.Lcd.println(" and visit 192.168.4.1 ");
    M5.Lcd.setCursor(20, 110, 2);
    M5.Lcd.println(" to see more ");
  }
    
    
  
}


DiceView::~DiceView()
{
    ESP_LOGD(TAG, "Destructor called");
    server1.stop();
    ESP_LOGD(TAG, "Destructor finished");
}


void DiceView::render() { 
   WiFiClient client = server1.available(); //尝试建立客户对象
  if(client){
    Serial.println("[New Client!]");
    client.write("ok");//向上位机发送下一帧发送指令
    
    while(client.connected())//如果客户端处于连接状态client.connected()
    {
      M5.update();
      if (M5.BtnB.wasReleasefor(700))
      {
        client.write("exit");
        break;
      }
      if (M5.BtnA.wasReleased())
      {
        client.write("go");
      }
      client.write("no");//向上位机发送当前帧未写入完指令
      while(client.available()){
        while (client.available()) {//检测缓冲区是否有数据
             if(read_count==0)
              {
                //start_time=millis();
                client.read(pack_size,2);//读取帧大小
                frame_size=pack_size[0]+(pack_size[1]<<8);
             }
             read_count=client.read(buff,7000);//向缓冲区读取数据
             memcpy(&img_buff[size_count],buff,read_count);//将读取的buff字节地址复制给img_buff数组
             size_count=size_count+read_count;//计数当前帧字节位置
             
             if(img_buff[frame_size-3]==0xaa && img_buff[frame_size-2]==0xbb && img_buff[frame_size-1]==0xcc)//判断末尾数据是否当前帧校验位
             {
              
              img_buff[frame_size-3]=0;img_buff[frame_size-2]=0;img_buff[frame_size-1]=0;//清除标志位
              
              TJpgDec.drawJpg(0,0,img_buff, sizeof(img_buff));//在左上角的0,0处绘制图像——在这个草图中，DMA请求在回调tft_output()中处理
              
              size_count=0;//下一帧
              read_count=0;
              client.write("ok");//向上位机发送下一帧发送指令
              //end_time = millis(); //计算mcu刷新一张图片的时间，从而算出1s能刷新多少张图，即得出最大刷新率
              break;
              }       
        }
      }
    }
    client.stop();
    Serial.println("连接中断,请复位重新创建服务端");
  }
}
