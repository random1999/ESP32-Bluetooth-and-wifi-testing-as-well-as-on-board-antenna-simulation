//===============================================================================================
// 作者： cx
// 时间： 2025-06-06
// 实现： ESP32通过wifi联网，获取日期、天气信息，并在OLED屏上显示，每按一次按键切换显示内容
//===============================================================================================
#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "ChinaNet-QgdJ";          // your WiFi SSID
const char *password = "ihmvzmvq";           // your WiFi password
String url_date = "http://apis.juhe.cn/fapig/timezone/show";   // 使用聚合数据API,一天免费查询30次 :)
String key_date = "21e1e5769e7bebf3e96334ad209bf2db";
String area = "asia";
String url_weather = "http://apis.juhe.cn/simpleWeather/query"; 
String city = "上海";
String key_weather = "23a8ec0d4174c05a313ae5cfd98d9d8e";
String response;

U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI OLED(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* cs=*/ 16, /* dc=*/ 7, /* reset=*/ 6);
const int button = 13;
int buttonPressed=0;
int display_state=0;

void showDate(){
  HTTPClient http;                           //创建HTTPClient对象
  http.begin(url_date+"?key="+key_date+"&c="+area); //发送HTTP请求
  int http_code=http.GET();                  //接收HTTP响应状态码
  Serial.printf("HTTP状态码:%d \n",http_code);
  response=http.getString();                 //获取HTTP响应正文
  Serial.print("响应数据:");
  Serial.println(response);
  http.end();                                //关闭HTTP连接

  DynamicJsonDocument doc(2048); //创建Json对象
  deserializeJson(doc,response); //将响应正文解析到Json对象
  String time=doc["result"]["tz"][63]["time"].as<String>();  //获取当前时间
  String week=doc["result"]["tz"][63]["week"].as<String>();  //获取当前星期

  Serial.printf("当前地区:%s 当前时间:%s 当前星期:%s\n",city,time,week);
  OLED.clearBuffer();
  OLED.setCursor(0, 10);
  OLED.print("当前地区: "+city);
  OLED.setCursor(0, 35);
  OLED.print(time);
  OLED.setCursor(0, 60);
  OLED.print("当前星期: "+week);
  OLED.sendBuffer();    //刷新OLED
}

void showWeather(){
  HTTPClient http;                           //创建HTTPClient对象
  http.begin(url_weather+"?city="+city+"&key="+key_weather); //发送HTTP请求
  int http_code=http.GET();                  //接收HTTP响应状态码
  Serial.printf("HTTP状态码:%d \n",http_code);
  response=http.getString();                 //获取HTTP响应正文
  Serial.print("响应数据:");
  Serial.println(response);
  http.end();                                //关闭HTTP连接

  DynamicJsonDocument doc(1024); //创建Json对象
  deserializeJson(doc,response); //将响应正文解析到Json对象
  String weather=doc["result"]["realtime"]["info"].as<String>(); //获取当前天气
  String temp=doc["result"]["realtime"]["temperature"].as<String>(); //获取当前气温
  String wind=doc["result"]["realtime"]["direct"].as<String>();  //获取当前风向

  Serial.printf("当前天气:%s 当前气温:%s 当前风向:%s\n",weather,temp,wind);
  OLED.clearBuffer();
  OLED.setCursor(0, 10);
  OLED.print("当前天气: "+weather);
  OLED.setCursor(0, 35);
  OLED.print("当前气温: "+temp);
  OLED.setCursor(0, 60);
  OLED.print("当前风向: "+wind);
  OLED.sendBuffer();    //刷新OLED
}

void showTomorrowWeather(){
  DynamicJsonDocument doc(1024); //创建Json对象
  deserializeJson(doc,response); //将响应正文解析到Json对象
  String weather=doc["result"]["future"][0]["weather"].as<String>(); //获取明日天气
  String temp=doc["result"]["future"][0]["temperature"].as<String>(); //获取明日气温
  String wind=doc["result"]["future"][0]["direct"].as<String>();  //获取明日风向

  Serial.printf("明日天气:%s 明日气温:%s 明日风向:%s\n",weather,temp,wind);
  OLED.clearBuffer();
  OLED.setCursor(0, 10);
  OLED.print("明日天气: "+weather);
  OLED.setCursor(0, 35);
  OLED.print("明日气温: "+temp);
  OLED.setCursor(0, 60);
  OLED.print("明日风向: "+wind);
  OLED.sendBuffer();    //刷新OLED
}

void buttonHandler(){    //按键中断函数
  buttonPressed=1;
  //delay(300);         //注意！在中断函数中不要delay,否则会死机重启!
}

void setup(void){
  Serial.begin(115200);

  OLED.begin();   //初始化OLED屏
  OLED.enableUTF8Print();
  //OLED.setFont(u8g2_font_unifont_t_chinese2); 
  OLED.setFont(u8g2_font_wqy12_t_gb2312); //比较全的中文字库

  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //连接wifi，STA模式
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  OLED.clearBuffer();
  OLED.setCursor(0, 15);
  OLED.print("WiFi connected");
  OLED.sendBuffer();

  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), buttonHandler, FALLING);  //绑定按键中断
}

void loop(void) {
  if(buttonPressed==1){
    if(display_state==0)showDate();
    else if(display_state==1)showWeather();
    else if(display_state==2)showTomorrowWeather();
    display_state++;
    if(display_state>=3)display_state=0;

    buttonPressed=0;
  }
  
  delay(3000); 
}