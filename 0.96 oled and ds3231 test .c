#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================== 硬件配置 ==================
// DS3231 (I2C0: GPIO21/22)
#define DS3231_SDA 21
#define DS3231_SCL 22
RTC_DS3231 rtc;

// OLED (I2C1: GPIO18/19)
#define OLED_SDA 18
#define OLED_SCL 19
Adafruit_SSD1306 oled(128, 64, &Wire1, -1); // 使用Wire1实例

// ================== 初始化函数 ==================
void setup() {
  Serial.begin(115200);

  // 初始化DS3231的I2C总线 (Wire)
  Wire.begin(DS3231_SDA, DS3231_SCL);
  if (!rtc.begin()) {
    Serial.println("DS3231初始化失败！");
    while (1);
  }

  // 初始化OLED的I2C总线 (Wire1)
  Wire1.begin(OLED_SDA, OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED初始化失败！");
    while (1);
  }

  // 显示启动界面
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("System Ready");
  oled.display();
  delay(2000);

  // 如果RTC掉电，同步时间（示例：手动设置初始时间）
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // 使用编译时间初始化
  }
}

// ================== 主循环 ==================
void loop() {
  DateTime now = rtc.now();
  
  // 更新OLED显示
  oled.clearDisplay();
  oled.setCursor(0, 0);
  
  // 显示日期（格式：YYYY-MM-DD）
  oled.print("Date: ");
  oled.print(now.year());
  oled.print("-");
  if (now.month() < 10) oled.print("0");
  oled.print(now.month());
  oled.print("-");
  if (now.day() < 10) oled.print("0");
  oled.println(now.day());
  
  // 显示时间（格式：HH:MM:SS）
  oled.print("Time: ");
  if (now.hour() < 10) oled.print("0");
  oled.print(now.hour());
  oled.print(":");
  if (now.minute() < 10) oled.print("0");
  oled.print(now.minute());
  oled.print(":");
  if (now.second() < 10) oled.print("0");
  oled.println(now.second());
  
  // 显示温度（单位：℃）
  float temp = rtc.getTemperature(); // 从DS3231读取温度
  oled.print("Temp: ");
  oled.print(temp, 1); // 保留1位小数
  oled.println(" °C"); // 添加温度符号
  
  oled.display();
  delay(1000); // 每秒刷新一次
}