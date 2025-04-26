#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ====================== 云平台对接规则参数（密钥模块） ======================
// WiFi 配置
const char* ssid = "FAST_C377";          // WiFi 名称
const char* passwordWiFi = "201d201d";    // WiFi 密码

// MQTT 密钥参数（核心模块，从平台获取）
const char* mqttServer = "sh-3-mqtt.iot-api.com";  // MQTT 主机（平台提供）
const int mqttPort = 1883;                       // MQTT 端口（平台指定）
const char* username = "0h1n33917c0myacc";         // 用户名（平台颁发的设备令牌/密钥 1）
const char* passwordMQTT = "3hbYFc6lJb";                    // 密码（平台颁发的密钥 2，若无则留空）
const char* clientId = "ESP32_000000";             // 客户端 ID（需唯一，建议用 MAC 生成）

// DHT11 传感器配置
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ====================== 核心功能函数 ======================
void setup() {
  Serial.begin(115200);       // 初始化串口
  dht.begin();                // 初始化传感器
  
  // 校验密钥是否为空（关键安全检查）
  if (strlen(username) == 0 || strlen(passwordMQTT) == 0 && !isPasswordOptional()) {
    Serial.println("❌ 错误：密钥（用户名/密码）为空，请检查平台配置！");
    while (1); // 停止执行
  }

  connectWiFi();              // 连接 WiFi
  mqttClient.setServer(mqttServer, mqttPort); // 设置服务器和端口
  connectMQTT();              // 连接 MQTT 服务器
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // 数据上报逻辑（同原代码）
  static unsigned long lastReportTime = 0;
  if (millis() - lastReportTime >= 5000) {
    float temperature = readTemperature();
    publishSensorData(temperature);
    lastReportTime = millis();
  }
}

// ---------------------- WiFi 连接函数 ----------------------
void connectWiFi() {
  WiFi.begin(ssid, passwordWiFi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功，IP: " + WiFi.localIP().toString());
}

// ---------------------- MQTT 连接函数（密钥认证核心） ----------------------
void connectMQTT() {
  Serial.print("尝试连接 MQTT 服务器（密钥：");
  Serial.print(username);
  Serial.print("）... ");

  // 使用密钥进行认证
  if (mqttClient.connect(
    clientId,       // 客户端 ID
    username,       // 用户名（密钥 1）
    passwordMQTT    // 密码（密钥 2）
  )) {
    Serial.println("成功");
  } else {
    Serial.print("失败，错误码: ");
    Serial.println(mqttClient.state()); // 打印 MQTT 连接错误码
    delay(10000); // 10 秒后重试
  }
}

// ---------------------- 辅助函数：检查密码是否可选（根据平台规则） ----------------------
bool isPasswordOptional() {
  // 若平台允许密码为空（如仅用设备令牌认证），返回 true
  // 否则返回 false（需根据平台文档修改）
  return true; 
}

// ---------------------- 温度读取与数据发布函数（同原代码） ----------------------
float readTemperature() { /* 保持不变 */ }
void publishSensorData(float temperature) { /* 保持不变 */ }