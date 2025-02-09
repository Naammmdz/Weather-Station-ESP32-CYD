#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h> 
#include "ui.h" // Interface generated from SquareLine Studio
#include "weather_images.h" 
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Audio.h>
#include "FS.h"
#include <Adafruit_AHTX0.h>
#include "SPIFFS.h"

// ATH_10
Adafruit_AHTX0 aht;

// Loa
Audio audio(true, I2S_DAC_CHANNEL_LEFT_EN);

// *** RTC ***
RTC_DS3231 rtc;

// *** Define touchscreen pins ***
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

// *** Configure screen parameters ***
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240,touchScreenMaximumY = 3800;
// *** Initialize SPI communication ***
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// *** Log function ***
void log_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// *** Handle touchscreen input ***
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = map(p.x, 200, 3700, 0, SCREEN_WIDTH);
    data->point.y = map(p.y, 240, 3800, 0, SCREEN_HEIGHT);
    // data->point.x = ::map(p.x, 200, 3700, 0, SCREEN_WIDTH);
    // data->point.y = ::map(p.y, 240, 3800, 0, SCREEN_HEIGHT);

  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// *** Initialize the interface from SquareLine Studio ***
void setup_ui() {
  ui_init(); // Automatically generated function from SquareLine Studio
}
///////////////////// WiFi /////////////////////

// *** WiFi setup ***
#define MAX_NETWORKS 5
String ssidList[MAX_NETWORKS];
int networkCount = 0;
String selectedSSID = "";
String password = "";

// *** Scan for Wi-Fi ***
void scanWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  networkCount = WiFi.scanNetworks();
  for (int i = 0; i < networkCount && i < MAX_NETWORKS; i++) {
    ssidList[i] = WiFi.SSID(i);
  }

  // Update dropdown in the interface
  String options = "";
  for (int i = 0; i < networkCount; i++) {
    options += ssidList[i];
    if (i < networkCount - 1) options += "\n";
  }
  lv_dropdown_set_options(ui_Dropdown4, options.c_str());
}

void rescanWiFi(lv_event_t *event) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  scanWiFi();
}

// *** Handle selected SSID from dropdown ***
void dropdown_event_handler(lv_event_t *event) {
  char ssid[64];
  lv_dropdown_get_selected_str(ui_Dropdown4, ssid, sizeof(ssid));
  selectedSSID = String(ssid);
  Serial.println("Selected SSID: " + selectedSSID);
}

// *** Connect to Wi-Fi ***
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");

  // Start Wi-Fi connection
  WiFi.begin(selectedSSID.c_str(), password.c_str());

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000); // Wait for 1 second
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    _ui_screen_change(&ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen4_screen_init);
    _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen3_screen_init);
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

// Function to get the password from the TextArea
void button_event_handler(lv_event_t *event) {
  lv_event_code_t code = lv_event_get_code(event); // Get the event code
  if (code == LV_EVENT_CLICKED) {  // Check if the button is clicked
    // Retrieve the password from the TextArea
    const char *passwordText = lv_textarea_get_text(ui_TextArea2);
    password = String(passwordText);
    
    // Print the password to the Serial Monitor for debugging
    Serial.print("Entered Password: ");
    Serial.println(password);
    
    // Attempt to connect to Wi-Fi
    connectToWiFi();
  }
}

///////////////////// Weather /////////////////////
String latitude = "10.8231";
String longitude = "106.6297";
// Enter your location
String location = "Ho_Chi_Minh";
// Type the timezone you want to get the time for
String timezone = "Asia/Ho_Chi_Minh";
// Store date and time
String current_date;
String last_weather_update;
String temperature;
String humidity;
int is_day;
int weather_code = 0;
String weather_description;
// SET VARIABLE TO 0 FOR TEMPERATURE IN FAHRENHEIT DEGREES
#define TEMP_CELSIUS 1

#if TEMP_CELSIUS
  String temperature_unit = "";
  const char degree_symbol[] = "\u00B0C";
#else
  String temperature_unit = "&temperature_unit=fahrenheit";
  const char degree_symbol[] = "\u00B0F";
#endif

void get_weather_data() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Construct the API endpoint
    String url = String("http://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,relative_humidity_2m,is_day,precipitation,rain,weather_code" + temperature_unit + "&timezone=" + timezone + "&forecast_days=1");
    http.begin(url);
    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) {
      // Check for the response
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          const char* datetime = doc["current"]["time"];
          temperature = doc["current"]["temperature_2m"].as<float>();
          humidity = doc["current"]["relative_humidity_2m"].as<int>();
          is_day = doc["current"]["is_day"].as<int>();
          weather_code = doc["current"]["weather_code"].as<int>();

          String datetime_str = String(datetime);
          int splitIndex = datetime_str.indexOf('T');
          current_date = datetime_str.substring(0, splitIndex);
          last_weather_update = datetime_str.substring(splitIndex + 1, splitIndex + 9); // Extract time portion
        } else {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        }
      }
      else {
        Serial.println("Failed");
      }
    } else {
      Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end(); // Close connection
  } else {
    Serial.println("Not connected to Wi-Fi");
  }
}

void get_weather_description(int code) {
  switch (code) {
    case 0:
      if(is_day==1) { lv_image_set_src(uic_weather_image, &image_weather_sun); }
      else { lv_image_set_src(uic_weather_image, &image_weather_night); }
      weather_description = "CLEAR SKY";
      break;
    case 1: 
      if(is_day==1) { lv_image_set_src(uic_weather_image, &image_weather_sun); }
      else { lv_image_set_src(uic_weather_image, &image_weather_night); }
      weather_description = "MAINLY CLEAR";
      break;
    case 2: 
      lv_image_set_src(uic_weather_image, &image_weather_cloud);
      weather_description = "PARTLY CLOUDY";
      break;
    case 3:
      lv_image_set_src(uic_weather_image, &image_weather_cloud);
      weather_description = "OVERCAST";
      break;
    case 45:
      lv_image_set_src(uic_weather_image, &image_weather_cloud);
      weather_description = "FOG";
      break;
    case 48:
      lv_image_set_src(uic_weather_image, &image_weather_cloud);
      weather_description = "DEPOSITING RIME FOG";
      break;
    case 51:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "DRIZZLE LIGHT INTENSITY";
      break;
    case 53:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "DRIZZLE MODERATE INTENSITY";
      break;
    case 55:
      lv_image_set_src(uic_weather_image, &image_weather_rain); 
      weather_description = "DRIZZLE DENSE INTENSITY";
      break;
    case 56:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "FREEZING DRIZZLE LIGHT";
      break;
    case 57:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "FREEZING DRIZZLE DENSE";
      break;
    case 61:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN SLIGHT INTENSITY";
      break;
    case 63:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN MODERATE INTENSITY";
      break;
    case 65:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN HEAVY INTENSITY";
      break;
    case 66:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "FREEZING RAIN LIGHT INTENSITY";
      break;
    case 67:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "FREEZING RAIN HEAVY INTENSITY";
      break;
    case 71:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW FALL SLIGHT INTENSITY";
      break;
    case 73:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW FALL MODERATE INTENSITY";
      break;
    case 75:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW FALL HEAVY INTENSITY";
      break;
    case 77:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW GRAINS";
      break;
    case 80:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN SHOWERS SLIGHT";
      break;
    case 81:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN SHOWERS MODERATE";
      break;
    case 82:
      lv_image_set_src(uic_weather_image, &image_weather_rain);
      weather_description = "RAIN SHOWERS VIOLENT";
      break;
    case 85:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW SHOWERS SLIGHT";
      break;
    case 86:
      lv_image_set_src(uic_weather_image, &image_weather_snow);
      weather_description = "SNOW SHOWERS HEAVY";
      break;
    case 95:
      lv_image_set_src(uic_weather_image, &image_weather_thunder);
      weather_description = "THUNDERSTORM";
      break;
    case 96:
      lv_image_set_src(uic_weather_image, &image_weather_thunder);
      weather_description = "THUNDERSTORM SLIGHT HAIL";
      break;
    case 99:
      lv_image_set_src(uic_weather_image, &image_weather_thunder);
      weather_description = "THUNDERSTORM HEAVY HAIL";
      break;
    default: 
      weather_description = "UNKNOWN WEATHER CODE";
      break;
  }
}

static void timer_cb(lv_timer_t *timer){
  LV_UNUSED(timer);
  int year, month, day;
  get_weather_data();
  get_weather_description(weather_code);
  sscanf(current_date.c_str(), "%d-%d-%d", &year, &month, &day);
  char formatted_date[12];
  sprintf(formatted_date, "%02d/%02d/%04d", day, month, year);
  lv_label_set_text(uic_text_label_date, formatted_date);
  lv_label_set_text(uic_text_label_temperature, String("      " + temperature + degree_symbol).c_str());
  lv_label_set_text(uic_text_label_humidity, String("   " + humidity + "%").c_str());
  lv_label_set_text(uic_text_label_weather_description, weather_description.c_str());
  lv_label_set_text(uic_text_label_time_location, String("Last Update: " + last_weather_update + "  |  " + location).c_str());
  
  lv_calendar_set_today_date(ui_Calendar1, year, month, day);
  lv_calendar_set_showed_date(ui_Calendar1, year, month);
}

void weather_station_ui(lv_event_t *event) {

  // Get the weather data from open-meteo.com API
  get_weather_data();
  
  get_weather_description(weather_code);
  lv_image_set_src(uic_weather_image_temperature, &image_weather_temperature);
  lv_image_set_src(uic_weather_image_humidity, &image_weather_humidity);
  lv_timer_t * timer = lv_timer_create(timer_cb, 600000, NULL);
  lv_timer_ready(timer);
}

///////////////////// Display Time /////////////////////

// Store hour, minute, second
static int32_t hour;
static int32_t minute;
static int32_t second;

String format_time(int time) {
  return (time < 10) ? "0" + String(time) : String(time);
}

static void timer_1cb(lv_timer_t * timer){
  LV_UNUSED(timer);
  DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();

  String hour_time_f = format_time(hour);
  String minute_time_f = format_time(minute);

  String final_time_str = String(hour_time_f) + ":" + String(minute_time_f);
  lv_label_set_text(ui_Label13, final_time_str.c_str());
}

void setup_time(lv_event_t *event) {
  lv_timer_t * timer = lv_timer_create(timer_1cb, 9000, NULL);
  lv_timer_ready(timer);
}

///////////////////// Alarm /////////////////////
int selected_hour = 0;           
int selected_minute = 0;  
int selected_hour_check = 0;           
int selected_minute_check = 0;          
bool weekdays_selected[7] = {false, false, false, false, false, false, false}; 
bool alarm_triggered = false;
bool alarm_on = false;

void save_alarm_settings(lv_event_t *e) {

    selected_hour = lv_roller_get_selected(ui_Roller1);
    selected_minute = lv_roller_get_selected(ui_Roller3);
    selected_hour_check = selected_hour;           
    selected_minute_check = selected_minute; 
    weekdays_selected[0] = lv_obj_has_state(uic_checkbox_sunday, LV_STATE_CHECKED);
    weekdays_selected[1] = lv_obj_has_state(uic_checkbox_monday, LV_STATE_CHECKED);
    weekdays_selected[2] = lv_obj_has_state(uic_checkbox_tuesday, LV_STATE_CHECKED);
    weekdays_selected[3] = lv_obj_has_state(uic_checkbox_wednesday, LV_STATE_CHECKED);
    weekdays_selected[4] = lv_obj_has_state(uic_checkbox_thursday, LV_STATE_CHECKED);
    weekdays_selected[5] = lv_obj_has_state(uic_checkbox_friday, LV_STATE_CHECKED);
    weekdays_selected[6] = lv_obj_has_state(uic_checkbox_saturday, LV_STATE_CHECKED);
    weekdays_selected[7] = lv_obj_has_state(uic_checkbox_sunday, LV_STATE_CHECKED);

    String hour_time = format_time(selected_hour);
    String minute_time = format_time(selected_minute);
    String time_str = hour_time + " : " + minute_time;
    lv_label_set_text(ui_Label20, time_str.c_str());
    String days_str = "";
    const char* days_of_week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    for (int i = 0; i < 7; i++) {
        if (weekdays_selected[i]) {
            if (days_str.length() > 0) days_str += ", ";
            days_str += days_of_week[i];
        }
    }
    lv_label_set_text(ui_Label22, days_str.c_str());
}

bool check_alarm(DateTime now) {
  if (!lv_obj_has_state(ui_Switch1, LV_STATE_CHECKED)) {
    return false;
  }
  int current_weekday = now.dayOfTheWeek();

  if (now.hour() == lv_roller_get_selected(ui_Roller1) && now.minute() == lv_roller_get_selected(ui_Roller3)) {

    if (weekdays_selected[current_weekday] && !alarm_triggered) {
      alarm_triggered = true;
      alarm_on = true;
      return true;
    }
  } else {
    alarm_triggered = false;
  }

  return false;
}


void stop_alarm_action(lv_event_t *e) {
  alarm_on = false;
  audio.stopSong();
  audio.freeSpace();
  _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen3_screen_init);
}

void activateAlarm() {

  if (alarm_on) {
    audio.connecttoFS(SPIFFS, "/baothuc.mp3");
    _ui_screen_change(&ui_Screen10, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen10_screen_init);
  }
}

static void timer_alarm_cb(lv_timer_t * timer_alarm){
  LV_UNUSED(timer_alarm);
  DateTime now = rtc.now();

  if (check_alarm(now)) {
    activateAlarm();
  }
}

///////////////////// Chatbot /////////////////////

#define BOT_TOKEN "X"
#define CHAT_ID "X"
const unsigned long BOT_MTBS = 2000; // mean time between scan messages

unsigned long bot_lasttime; // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    String answer = "";
    if (text == "/help")
      answer += "So you need _help_, uh?🤔 me too! use /in_house or /weather or /status";
    else if (text == "/start") {
      answer += "Welcome my new friend!🤗 You are the first *" + from_name + "* I've ever met" + ".\n";
      answer += "Use the following commands to control your outputs.\n\n";
      answer += "🌡️ /in_house get temperature and humidity in your home.\n";
      answer += "🌤️ /weather get information about weather.\n";
      answer += "❓ /help I can help you 😉\n";
    }
    else if (text == "/status")
      answer += "All is good here, thanks for asking!";
    else if (text == "/in_house"){
      sensors_event_t temp_event, hum_event;
      aht.getEvent(&hum_event, &temp_event); // Lấy dữ liệu
      float temperature = temp_event.temperature;
      float humidity = hum_event.relative_humidity;


      int tempInteger = (int)temperature;
      int humInteger = (int)humidity;
      answer += "Temperature: " + String(tempInteger) + degree_symbol + ".\n";
      answer += "Humidity: " + String(humInteger) + "%" + ".\n";
    }
    else if (text == "/weather")
      answer += weather_description;
    else
      answer += "Say what?";

    bot.sendMessage(chat_id, answer, "");
  }
}

void botTask(void *pvParameters) {
  for (;;) {
    if (millis() - bot_lasttime > BOT_MTBS) {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

      if (numNewMessages > 0) {
        handleNewMessages(numNewMessages);
      }

      bot_lasttime = millis();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Giảm tải CPU
  }
}

static void timer_sensor_cb(lv_timer_t * timer_sensor){
  sensors_event_t temp_event, hum_event;
  aht.getEvent(&hum_event, &temp_event); // Lấy dữ liệu
  float temperature = temp_event.temperature;
  float humidity = hum_event.relative_humidity;


  int tempInteger = (int)temperature;
  int humInteger = (int)humidity;
  lv_label_set_text(ui_Label14, String("T: " + String(tempInteger) + degree_symbol).c_str());
  lv_label_set_text(ui_Label19, String("H: " + String(humInteger) + "%").c_str());

  if (tempInteger > 35){
    String answer = "⚠️ Temperature high: " + String(tempInteger) + "°C!";

    bot.sendMessage(CHAT_ID, answer, "");
  }
}

// *** Setup ***
void setup() {

  Serial.begin(115200);
  Serial.println("Starting system...");

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  
  // I2C
  Wire.begin(27, 22);
  
  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Backup set time

  lv_init(); // Initialize LVGL
  lv_log_register_print_cb(log_print); // Register log function

  // Configure SPI for touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(2);

  // Initialize TFT display
  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  // Initialize input device (touchscreen)
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Interface from SquareLine Studio
  setup_ui();

  // Setup Wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  scanWiFi(); // Scan Wi-Fi and update dropdown

  xTaskCreatePinnedToCore(
      botTask,            // Hàm xử lý bot
      "Bot Task",         // Tên task
      8192,               // Kích thước stack (8192)
      NULL,               // Tham số
      1,                  // Độ ưu tiên
      NULL,               // Handle task
      1                   // Chạy trên Core 1
  );

  // Timer check alarm
  lv_timer_t *timer_alarm = lv_timer_create(timer_alarm_cb, 5000, NULL); 
  lv_timer_ready(timer_alarm);


  // Setup SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Setup audio
  audio.forceMono(true);
  audio.setVolume(20);

  // Setup ATH_10
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
  lv_timer_t *timer_sensor = lv_timer_create(timer_sensor_cb, 4000, NULL); 
  lv_timer_ready(timer_sensor);
}

// *** Main loop ***
void loop() {
  audio.loop();
  lv_task_handler(); // Operate GUIq
  lv_tick_inc(5);    // Update LVGL tick
  delay(5);          // Maintain timing
}
