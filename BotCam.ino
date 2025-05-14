// Загрузка библиотек
#include <Arduino.h>
#include <FastBot2.h>
#include <WiFi.h>

#include "camera.h"

// Настройки
#define LED_PIN 4
#define WIFI_SSID "RT-WiFi-ABAC"
#define WIFI_PASS "Dev5cid5B2"
#define BOT_TOKEN "5867959560:AAFbTdPtgiRZA5W4teWDqIUqag59Xo3Otl0"

FastBot2 bot;

void updateh(fb::Update& u) {

    if (u.message().text() == "/photo") { // Отправка фото
        // пропустить прошлый кадр
        camera_fb_t* fbj = esp_camera_fb_get();
        esp_camera_fb_return(fbj);

        fbj = esp_camera_fb_get();
        if (fbj) {
            fb::File f("frame.jpg", fb::File::Type::photo, fbj->buf, fbj->len);
            f.chatID = u.message().chat().id();
            bot.sendFile(f, true);  // обязательно синхронная отправка
        }
        esp_camera_fb_return(fbj);
    } else if (u.message().text() == "/led_on"){ // Включение светодиода
      digitalWrite(4, HIGH);
      bot.sendMessage(fb::Message("Led is on.", u.message().chat().id()));
    } else if (u.message().text() == "/led_off"){ // Выключение светодиода 
      digitalWrite(4, LOW);
      bot.sendMessage(fb::Message("Led is off.", u.message().chat().id()));
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    Serial.println();

    // Подключение Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.println(WiFi.localIP());

    bool res = cam_init(FRAMESIZE_VGA, PIXFORMAT_JPEG);
    Serial.println(res ? "Camera OK" : "Camera ERROR");

    bot.attachUpdate(updateh);
    bot.setToken(F(BOT_TOKEN));
    bot.setPollMode(fb::Poll::Long, 20000);
}

void loop() {
    // "Тикаем" бота
    bot.tick();
}