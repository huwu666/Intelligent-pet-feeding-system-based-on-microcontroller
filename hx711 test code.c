#include <HX711.h>

// 硬件配置
#define HX711_DT  23    // 数据引脚
#define HX711_SCK 22    // 时钟引脚
#define SAMPLES   20    // 滤波采样次数

HX711 scale;
float calibration_factor = -440;  // 初始校准系数
bool is_sensor_ready = false;        // 传感器状态标志

// ================== 初始化函数 ==================
void setup() {
  Serial.begin(115200);
  initialize_sensor();
  print_help_menu();
}

void initialize_sensor() {
  scale.begin(HX711_DT, HX711_SCK);
  scale.set_scale(calibration_factor);
  scale.tare();  // 强制空载清零
  is_sensor_ready = true;
  Serial.println("[系统] 传感器初始化完成");
}

// ================== 主循环函数 ==================
void loop() {
  if (is_sensor_ready) {
    handle_serial_commands();
    display_weight();
  }
  delay(300);  // 降低刷新频率减少串口输出干扰
}

// ================== 功能模块 ==================
void display_weight() {
  static float stabilized_weight = 0;
  float raw_weight = scale.get_units(SAMPLES);
  
  // 滑动平均滤波
  stabilized_weight = 0.7 * stabilized_weight + 0.3 * abs(raw_weight);
  
  Serial.print("当前重量: ");
  Serial.print(stabilized_weight, 1);
  Serial.print("g | 校准系数: ");
  Serial.println(calibration_factor, 2);
}

void handle_serial_commands() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch(cmd) {
      case 't':  // 清零指令
        handle_tare();
        break;
        
      case 'c':  // 校准指令
        handle_calibration();
        break;
        
      case '+':  // 系数微增
        adjust_calibration_factor(1.05);
        break;
        
      case '-':  // 系数微减
        adjust_calibration_factor(0.95);
        break;
        
      case '?':  // 帮助菜单
        print_help_menu();
        break;
        
      default:
        Serial.println("[错误] 未知指令");
    }
  }
}

// ================== 核心操作函数 ==================
void handle_tare() {
  scale.tare();
  Serial.println("[操作] 已执行清零！当前重量基准已重置");
}

void handle_calibration() {
  Serial.println("[校准] 请放置已知重量的物体后输入重量（单位：g）");
  
  while (!Serial.available());  // 等待输入
  float known_weight = Serial.parseFloat();
  
  if (known_weight <= 0) {
    Serial.println("[错误] 重量必须大于0");
    return;
  }

  long raw_value = scale.read_average(SAMPLES);
  if (abs(raw_value) < 1000) {  // 有效负载阈值
    Serial.println("[错误] 未检测到有效负载");
    return;
  }

  calibration_factor = raw_value / known_weight;
  scale.set_scale(calibration_factor);
  
  Serial.print("[校准] 新系数: ");
  Serial.println(calibration_factor, 2);
  Serial.println("注意：若重量显示为负值，请翻转传感器方向");
}

void adjust_calibration_factor(float multiplier) {
  calibration_factor *= multiplier;
  scale.set_scale(calibration_factor);
  Serial.print("[微调] 当前系数: ");
  Serial.println(calibration_factor, 2);
}

// ================== 帮助信息 ==================
void print_help_menu() {
  Serial.println("\n=== 称重传感器控制台 ===");
  Serial.println("t - 执行清零（空载时使用）");
  Serial.println("c - 启动校准流程");
  Serial.println("+ - 增加5%校准系数");
  Serial.println("- - 减少5%校准系数");
  Serial.println("? - 显示本帮助菜单");
  Serial.println("==========================");
}