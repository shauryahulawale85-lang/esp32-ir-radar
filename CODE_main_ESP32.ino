#include <ESP32Servo.h>
#include "driver/ledc.h" 

Servo radarServo;

const int SERVO_PIN = 18;   // Servo signal pin on GPIO 18
const int IR_PIN = 15;      // Analog input pin for IR Sensor on GPIO 15
const int BUZZER_PIN = 23;  // Buzzer pin on GPIO 23 (please ignore)

const int DETECTION_THRESHOLD = 2500; 

void setup() {
  Serial.begin(115200);
  radarServo.attach(SERVO_PIN, 544, 2400); 
  analogReadResolution(12);

  // Corrected struct ordering initialization to bypass strict compiler constraints
  ledc_timer_config_t ledc_timer;
  ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
  ledc_timer.duty_resolution  = LEDC_TIMER_8_BIT;
  ledc_timer.timer_num        = LEDC_TIMER_1;       // Dedicated hardware timer slot
  ledc_timer.freq_hz          = 2000;               // 2 kHz tone frequency
  ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel;
  ledc_channel.gpio_num       = (gpio_num_t)BUZZER_PIN;
  ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
  ledc_channel.channel        = LEDC_CHANNEL_1;
  ledc_channel.intr_type      = LEDC_INTR_DISABLE;
  ledc_channel.timer_sel      = LEDC_TIMER_1;
  ledc_channel.duty           = 0;                  // Start off
  ledc_channel.hpoint         = 0;
  ledc_channel_config(&ledc_channel);
}

void loop() {
  // Sweep from 0 to 180 degrees
  for (int pos = 0; pos <= 180; pos += 2) {
    radarServo.write(pos);
    delay(30); 
    int irRawValue = analogRead(IR_PIN);
    
    int detected = (irRawValue < DETECTION_THRESHOLD) ? 1 : 0;
    
    if (detected) {
      // Force hardware channel to toggle pin 23 
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 128); // 50% Volume
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
      delay(15);
      
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);   // Volume Off
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
      delay(15);
    } else {
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0); 
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    }
    
    Serial.print(pos);
    Serial.print(",");
    Serial.print(irRawValue);
    Serial.print(",");
    Serial.println(detected);
  }

  // Sweep back from 180 to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 2) {
    radarServo.write(pos);
    delay(30);
    int irRawValue = analogRead(IR_PIN);
    
    int detected = (irRawValue < DETECTION_THRESHOLD) ? 1 : 0;
    
    if (detected) {
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 128); 
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
      delay(15);
      
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0); 
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
      delay(15);
    } else {
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0); 
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    }
    
    Serial.print(pos);
    Serial.print(",");
    Serial.print(irRawValue);
    Serial.print(",");
    Serial.println(detected);
  }
}
