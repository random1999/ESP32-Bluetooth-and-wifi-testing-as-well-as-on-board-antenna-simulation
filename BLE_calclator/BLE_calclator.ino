//===============================================================================================
// 作者： cx
// 时间： 2025-06-03
// 实现： 手机使用"BLE调试助手"与ESP32蓝牙连接，手机发送计算问题(如"34+56")，ESP32返回计算结果，同时在串口打印
//===============================================================================================
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <Regexp.h>
#define serviceUUID          "2b6a7476-411a-11f0-9fe2-0242ac120002" //用户自定义UUID，使用网址https://www.uuidgenerator.net/version1生成
#define sendMsgCrticUUID     "326988de-411a-11f0-9fe2-0242ac120002" 
#define receiveMsgCrticUUID  "18c46978-4118-11f0-9fe2-0242ac120002" 
BLECharacteristic sendMsgCrtic(sendMsgCrticUUID,BLECharacteristic::PROPERTY_READ);     //用于给用户发送数据
BLECharacteristic receiveMsgCrtic(sendMsgCrticUUID,BLECharacteristic::PROPERTY_WRITE); //用于接收用户数据
String receiveMsg;
String sendMsg;
int num1,num2,result;
int count=0;

// 正则表达式-匹配回调函数
void match_callback(const char *match,         // matching string (not null-terminated)
                    const unsigned int length, // length of matching string
                    const MatchState &ms)      // MatchState in use (to get captures)
{
  String cap=match;
  cap=cap.substring(0,length);
  if(count==0){
    num1=cap.toInt();
    count++;
  }
  else {
    num2=cap.toInt();
    count=0;
    
    if(receiveMsg.indexOf("+")!=-1)result=num1+num2;
    else if(receiveMsg.indexOf("-")!=-1)result=num1-num2;
    else if(receiveMsg.indexOf("*")!=-1)result=num1*num2;
    else if(receiveMsg.indexOf("/")!=-1)result=num1/num2;
    else result=0;
    String s="\n "+receiveMsg+"= \n"+result;
    Serial.println(s);
    sendMsgCrtic.setValue(s);
    receiveMsgCrtic.setValue("");
    num1=0;num2=0;
  }
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init("ESP32_BLE");                //初始化BLE设备
    BLEServer *pServer = BLEDevice::createServer();       //在BLE设备里创建一个服务器
      BLEService *pService=pServer->createService(serviceUUID);    //在服务器里创建一个服务
        BLECharacteristic *nameCrtic = pService->createCharacteristic(BLEUUID((uint16_t)0x2A00), BLECharacteristic::PROPERTY_READ); //在服务里创建一个特征
        nameCrtic->setValue("BLE calculator");            //为特征赋值
        pService->addCharacteristic(&sendMsgCrtic);       //将一个特征添加到服务
        sendMsgCrtic.setValue("input your question:");    //为特征赋值
        pService->addCharacteristic(&receiveMsgCrtic);    //将一个特征添加到服务
    pService->start(); //启动服务

    BLEAdvertising *pAdvertising=BLEDevice::getAdvertising(); //获取广播器
    pAdvertising->addServiceUUID(pService->getUUID());        //将服务添加到广播器
  BLEDevice::startAdvertising();                              //启动广播器
}

void loop() {
  receiveMsg = receiveMsgCrtic.getValue(); //接收BLE蓝牙数据
  if(receiveMsg != ""){
    MatchState ms(const_cast<char*>(receiveMsg.c_str())); //定义正则表达式
    ms.GlobalMatch("%d+", match_callback);                //在正则表达式回调函数中提取数字,并输出运算结果
  }

  delay(500);
}
