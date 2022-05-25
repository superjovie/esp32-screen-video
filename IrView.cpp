#include <M5StickCPlus.h>
#include "View.h"
#include "IrView.h"

#include "driver/gpio.h"
static const char *TAG = "IR_VIEW";



IrView::IrView()
{   

    M5.begin();
    nvs_flash_init();
    M5.Lcd.setRotation(0);
    M5.Lcd.setSwapBytes(false);
    M5.Axp.EnableCoulombcounter();
    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    pinMode(10, OUTPUT);
    // set mode, temperature and fan level and internal state to enabled
    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(135, 240);
    
    disp_buffer->setTextColor(TFT_BLACK, TFT_WHITE);
    
    disp_buffer->fillRect(0, 0, 135, 240, TFT_BLACK);

    
    disp_buffer->fillRect(13, 11, 50, 30, TFT_WHITE);
    disp_buffer->drawString("1", 33, 18, 2);//num1
    disp_buffer->fillRect(73, 11, 50, 30, TFT_WHITE);
    disp_buffer->drawString("2", 93, 18, 2);//num2
    disp_buffer->fillRect(13, 53, 50, 30, TFT_WHITE);
    disp_buffer->drawString("3", 33, 60, 2);//num3
    disp_buffer->fillRect(73, 53, 50, 30, TFT_WHITE);
    disp_buffer->drawString("4", 93, 60, 2);//num4
    disp_buffer->fillRect(13, 94, 50, 30, TFT_WHITE);
    disp_buffer->drawString("5", 33, 101, 2);//num5
    disp_buffer->fillRect(73, 94, 50, 30, TFT_WHITE);
    disp_buffer->drawString("6", 93, 101, 2);//num6
    disp_buffer->fillRect(13, 135, 50, 30, TFT_WHITE);
    disp_buffer->drawString("7", 33, 142, 2);//num7
    disp_buffer->fillRect(73, 135, 50, 30, TFT_WHITE);
    disp_buffer->drawString("8", 93, 142, 2);//num8
    disp_buffer->fillRect(13, 178, 50, 30, TFT_WHITE);
    disp_buffer->drawString("9", 33, 185, 2);//num9
    disp_buffer->fillRect(73, 178, 50, 30, TFT_WHITE);
    disp_buffer->drawString("10", 89, 185, 2);//num10
    irrecv = new IRrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
    irsend = new IRsend(9); 
     
    Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
     
    #if DECODE_HASH
      // Ignore messages with less than minimum on or off pulses.
      irrecv->setUnknownThreshold(kMinUnknownSize);
    #endif  // DECODE_HASH
      irrecv->setTolerance(kTolerancePercentage);  // Override the default tolerance.
      irrecv->enableIRIn();
      irsend->begin(); 
    disp_buffer->pushSprite(0, 0); 
}

IrView::~IrView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete irrecv;
    delete irsend;
    delete disp_buffer;
    irrecv->disableIRIn();
    ESP_LOGD(TAG, "Destructor finished");
    gpio_reset_pin(GPIO_NUM_9);
    gpio_reset_pin(GPIO_NUM_33);
}

void IrView::render()
{
    M5.update();
    delay(200);
    if(M5.BtnB.isPressed()){
        count++;
        if(count == 11){count = 1;}
        if(count == 1){
        disp_buffer->fillRect(73, 178, 50, 30, TFT_WHITE);
        disp_buffer->drawString("10", 89, 185, 2);//num10
        disp_buffer->fillRect(13, 11, 50, 30, TFT_GREEN);
        disp_buffer->drawString("1", 33, 18, 2);//num1
        }
        if(count == 2){
            disp_buffer->fillRect(13, 11, 50, 30, TFT_WHITE);
            disp_buffer->drawString("1", 33, 18, 2);//num1
            disp_buffer->fillRect(73, 11, 50, 30, TFT_GREEN);
            disp_buffer->drawString("2", 93, 18, 2);//num2
        }
        if(count == 3){
            disp_buffer->fillRect(73, 11, 50, 30, TFT_WHITE);
            disp_buffer->drawString("2", 93, 18, 2);//num2
            disp_buffer->fillRect(13, 53, 50, 30, TFT_GREEN);
            disp_buffer->drawString("3", 33, 60, 2);//num3
        }
        if(count == 4){
            disp_buffer->fillRect(13, 53, 50, 30, TFT_WHITE);
            disp_buffer->drawString("3", 33, 60, 2);//num3
            disp_buffer->fillRect(73, 53, 50, 30, TFT_GREEN);
            disp_buffer->drawString("4", 93, 60, 2);//num4
        }
        if(count == 5){
            disp_buffer->fillRect(73, 53, 50, 30, TFT_WHITE);
            disp_buffer->drawString("4", 93, 60, 2);//num4
            disp_buffer->fillRect(13, 94, 50, 30, TFT_GREEN);
            disp_buffer->drawString("5", 33, 101, 2);//num5
        }
        if(count == 6){
            disp_buffer->fillRect(13, 94, 50, 30, TFT_WHITE);
            disp_buffer->drawString("5", 33, 101, 2);//num5
            disp_buffer->fillRect(73, 94, 50, 30, TFT_GREEN);
            disp_buffer->drawString("6", 93, 101, 2);//num6
        }
        if(count == 7){
            disp_buffer->fillRect(73, 94, 50, 30, TFT_WHITE);
            disp_buffer->drawString("6", 93, 101, 2);//num6
            disp_buffer->fillRect(13, 135, 50, 30, TFT_GREEN);
            disp_buffer->drawString("7", 33, 142, 2);//num7
        }
        if(count == 8){
            disp_buffer->fillRect(13, 135, 50, 30, TFT_WHITE);
            disp_buffer->drawString("7", 33, 142, 2);//num7
            disp_buffer->fillRect(73, 135, 50, 30, TFT_GREEN);
            disp_buffer->drawString("8", 93, 142, 2);//num8
        }
        if(count == 9){
            disp_buffer->fillRect(73, 135, 50, 30, TFT_WHITE);
            disp_buffer->drawString("8", 93, 142, 2);//num8
            disp_buffer->fillRect(13, 178, 50, 30, TFT_GREEN);
            disp_buffer->drawString("9", 33, 185, 2);//num9
        }
        if(count == 10){
            disp_buffer->fillRect(13, 178, 50, 30, TFT_WHITE);
            disp_buffer->drawString("9", 33, 185, 2);//num9
            disp_buffer->fillRect(73, 178, 50, 30, TFT_GREEN);
            disp_buffer->drawString("10", 89, 185, 2);//num10
        }
        disp_buffer->pushSprite(0, 0); 
          
    }
    
    
    if (irrecv->decode(&results)) {
       
        protocol = results.decode_type;
        size = results.bits;
        //success = true;
        char const *head[] = {"head1","head2","head3","head4","head5","head6","head7","head8","head9","head10"};
        nvs_write_wifi(&results,sizeof(results),head[count-1]);
        disp_buffer->drawString("recieved", 30, 220, 2);
        disp_buffer->pushSprite(0, 0);
        delay(1000);
        disp_buffer->drawString("waiting send", 30, 220, 2);
        disp_buffer->pushSprite(0, 0);
        // Is it a protocol we don't understand?
    }
    if (digitalRead(M5_BUTTON_HOME) == LOW){
        char const *heads[] = {"head1","head2","head3","head4","head5","head6","head7","head8","head9","head10"};
        send_nvs(heads[count-1]);
      }
      
    M5.update();
}


void IrView::blinkenLight()
{
    digitalWrite(10, LOW);
    delay(100);
    digitalWrite(10, HIGH);
}

void IrView::send_nvs(const char *head)
{
        decode_results res; 
        nvs_read_wifi(&res,head);
        protocol = res.decode_type;
        size = res.bits;
        if (protocol == decode_type_t::UNKNOWN) {  // Yes.
          // Convert the results into an array suitable for sendRaw().
          // resultToRawArray() allocates the memory we need for the array.
          uint16_t *raw_array = resultToRawArray(&res);
          // Find out how many elements are in the array.
          size = getCorrectedRawLength(&res);
          #if SEND_RAW
          // Send it out via the IR LED circuit.
          irsend->sendRaw(raw_array, size, 38);
          blinkenLight();
          #endif  // SEND_RAW
          // Deallocate the memory allocated by resultToRawArray().
          delete [] raw_array;
        } else if (hasACState(protocol)) {  // Does the message require a state[]?
          // It does, so send with bytes instead.
          success = irsend->send(protocol, res.state, size / 8);
          blinkenLight();
        } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
          success = irsend->send(protocol, res.value, size);
          blinkenLight();
        }
        // Resume capturing IR messages. It was not restarted until after we sent
        // the message so we didn't capture our own message.
        irrecv->resume();

}

esp_err_t IrView::nvs_read_wifi(struct decode_results *res, const char *head)
{
    
    esp_err_t err;
  
  //首先一部操作打开存储空间
    // Open
  //para(空间名字（字符串），操作类型（只读还是读写），操作句柄)
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        Serial.println("NVS_READ：fail\n");
        return err;
    }

//注意这一步十分重要，因为在NVS存储已知长度类型的数据时，我们可以明确的传入已知的长度。
//但是这个地方对于我们传入的数组或者说结构体，我们不知道明确长度，于是我们采用下面的操作来获取要读取的数据的长度。

 // Read the size of memory space required for blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_blob(nvs_handle, head, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {return err;}


//再来读取对应的值
    err = nvs_get_blob(nvs_handle, head, res, &required_size);
    if (err ==ESP_ERR_NVS_NOT_FOUND )
    {
        Serial.println("NVS_READ：key doesn’t exist");
        return err;
    }
    else if (err ==ESP_ERR_NVS_INVALID_HANDLE  )
    {
        Serial.println("NVS_READ：handle has been closed or is NULL");
        return err;
    }
    else if (err ==ESP_ERR_NVS_INVALID_NAME  )
    {
        Serial.println("NVS_READ：name doesn’t satisfy constraints ");
        return err;
    }
    else if (err ==ESP_ERR_NVS_INVALID_LENGTH  )
    {
        Serial.println("NVS_READ：length is not sufficient to store data");
        return err;
    }
    else
    {
        Serial.println("NVS_READ：读取成功");
    }

  //关闭句柄
    nvs_close(nvs_handle);
    return ESP_OK;
}

esp_err_t IrView::nvs_write_wifi(struct decode_results *res, uint32_t len, const char *head)
{
    
    esp_err_t err;

    //Serial.println("NVS_WRITE：fail0\n");
    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        Serial.println("NVS_WRITE：写入失败1");
        return err;
    }
    err = nvs_set_blob(nvs_handle, head, res, len);
    if (err != ESP_OK)
    {
        Serial.println("NVS_WRITE：写入失败2");
        return err;
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        Serial.println("NVS_WRITE：写入失败3");
        return err;
    }
    else
    {
        Serial.println("NVS_READ：写入成功");
    }
    nvs_close(nvs_handle);
    
    return ESP_OK;
}
