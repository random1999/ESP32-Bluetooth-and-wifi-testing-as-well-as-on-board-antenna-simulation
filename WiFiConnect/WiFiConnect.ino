#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "ChinaNet-QgdJ";          // your WiFi SSID
const char *password = "ihmvzmvq";           // your WiFi password
String url = "http://apis.juhe.cn/simpleWeather/query"; // 使用聚合数据API,一天免费查询30次 :)
String city = "上海";
String key = "23a8ec0d4174c05a313ae5cfd98d9d8e";


void setup() {
  Serial.begin(115200);

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

  HTTPClient http;                           //创建HTTPClient对象
  http.begin(url+"?city="+city+"&key="+key); //发生HTTP请求
  int http_code=http.GET();                  //接收HTTP响应状态码
  Serial.printf("HTTP状态码:%d \n",http_code);
  String response=http.getString();          //获取HTTP响应正文
  Serial.print("响应数据:");
  Serial.println(response);
  http.end();                                //关闭连接

  DynamicJsonDocument doc(1024); //创建Json对象
  deserializeJson(doc,response); //将响应正文解析到Json对象
  unsigned int temp=doc["result"]["realtime"]["temperature"].as<unsigned int>(); //获取当前气温
  String weather=doc["result"]["realtime"]["info"].as<String>(); //获取当前天气

  Serial.printf("当前温度:%d 当前天气:%s\n",temp,weather);
}

void loop() {

  delay(1000);
}
