#include <WiFi.h>

#define PWM_CHANNEL_BASE 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8 // 8-bit = 0–255


#define SSID "PLAY internet 5GHz_1A23"
#define PASSWORD "75051808"
#define SERVER_PORT 6969

#define PIN_FRONT 13
#define PIN_RIGHT 12
#define PIN_BACK 14
#define PIN_LEFT 27

#define INTERNAL_DIODE 2

#define PWM_MAX 255
#define PWM_MIN 0

#define BUFFER_SIZE 4

class MotorController{
  private:
    int pin;
    int pwmChannel; // it ranges from 1 to 100 in precentage
  public:
    MotorController(int pin, int channel){
      this->pin = pin;
      this->pwmChannel = 0;

      pinMode(this->pin, OUTPUT);
      ledcAttachChannel(this->pin, PWM_FREQ, PWM_RESOLUTION, channel);

    }

    ~MotorController(){
      this->pin = 0;
      this->pwmChannel = 0;
    }

    void setMotorSpeed(int speed){
      uint32_t pwmValue = constrain(speed, PWM_MIN, PWM_MAX);
      ledcWrite(this->pin, speed);
      //analogWrite(this->pin, this->pwmValue);
    }
};

void indicateConnection(int delayTime = 100, int blinks = 5){
  for(int i = 0 ; i < blinks; i ++){
    digitalWrite(INTERNAL_DIODE, HIGH);
    delay(delayTime);
    digitalWrite(INTERNAL_DIODE, LOW);
    delay(delayTime);
  }
}

MotorController motorFront(PIN_FRONT,0);
MotorController motorLeft(PIN_LEFT,1);
MotorController motorRight(PIN_RIGHT,2);
MotorController motorBack(PIN_BACK,3);

MotorController motors[4] = {motorFront, motorRight, motorBack, motorLeft};

WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(11200);
  //IPAddress localIp(192, 168, 0, );
  pinMode(INTERNAL_DIODE, OUTPUT);
  digitalWrite(INTERNAL_DIODE, OUTPUT);
  Serial.print("Connecting to: ");
  Serial.print(SSID);
  Serial.print(" with password: ");
  Serial.println(PASSWORD);
  WiFi.begin(SSID, PASSWORD);
  uint16_t tries = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(50);
    tries++;
    if(tries >= 1){
      digitalWrite(INTERNAL_DIODE, HIGH);
      delay(200);
      digitalWrite(INTERNAL_DIODE, LOW);
      tries = 0;
    }
  }
  digitalWrite(INTERNAL_DIODE, LOW);
  indicateConnection();
  server.begin();
  Serial.println(WiFi.getHostname());
}

int32_t getSpeedFromBuffer(uint8_t *buffer){
  /*int32_t speed = 0;
  memcpy(&speed, buffer, sizeof(int32_t));
  return speed;*/
    return ((int32_t)buffer[0]) |
         ((int32_t)buffer[1] << 8) |
         ((int32_t)buffer[2] << 16) |
         ((int32_t)buffer[3] << 24);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();

  uint8_t buffer[BUFFER_SIZE];

  if(client){
    indicateConnection(100, 10);
    while(client.connected()){
      if(client.available() >= BUFFER_SIZE){
        client.readBytes(buffer, BUFFER_SIZE);
        indicateConnection(20, 4);
        int32_t speed = getSpeedFromBuffer(buffer);
        for(int i = 0; i < 4; i ++){
          (motors + i)->setMotorSpeed(speed);
        }
      }
    }
  }
}
