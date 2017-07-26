#include <SoftwareSerial.h>
#define SerialSpeed 9600
#include "MotorDriver.h"
#define MOTOR_R 0
#define MOTOR_L 1

#define IR_THRESHOLD 300
#define HUMID_WET_THRESHOLD 600
//#define HUMID_WET_THRESHOLD 200

MotorDriver motor;
SoftwareSerial softSerial(2, 3); //R,T

  int time;
  int ir[4];
  int humid0;
  int humid1;
  bool move_forwarded_flag = false;
  bool moved_flag = false;
  
void setup(){ 
  Serial.begin(SerialSpeed);
  softSerial.begin(SerialSpeed);
  pinMode(1,OUTPUT);
  motor.begin();
  motor.stop(MOTOR_L);                 // brake
  motor.stop(MOTOR_R);                 // brake
}

void loop() {
  delay(500);
  read_sensor_values();
  send_to_wifi_module();
  //delay(5000); //for debuging need to be tuning???
  
  if(demo_start()) {
    Serial.print("demo started");    
    //if (water_given() && move_forwarded_flag == true) {
    if (water_given() /*&& moved_flag == false*/) {
      Serial.print("water given");
      //turn();
      moved_flag = true;
      uda_uda();
    }
    /*
    else if(detect_forward_person()) {
      move_foward();
      move_forwarded_flag = true;
    }
    */
    
    
    /*
    int direction = person_direction();  
    if (direction !=-1) {
      Serial.print("person!!");
      Serial.print(direction);
      move_direction(direction);  
    }
    */   
  }
  
}

void move_foward(){
    
    //delay(1000);
    delayMicroseconds(10000);
    //need to be 
    motor.speed(MOTOR_R, 100);
    motor.speed(MOTOR_L, 100);
    delay(10000);
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    
    /*
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    delay(5000);
    motor.speed(MOTOR_R, -100);           
    motor.speed(MOTOR_L, -100);           
    delay(5000);
    */
    motor.stop(MOTOR_R);                  // stop
    motor.stop(MOTOR_L);                 // brake
    delay(3000); 
}

bool water_given() {
  if (humid0 > HUMID_WET_THRESHOLD) {
    return true;
  }
  return false;
}

int person_direction() {
  if (ir[0] > IR_THRESHOLD) {
    return 0;
  }
  else if (ir[1] > IR_THRESHOLD) {
    return 90;
  }
  else if (ir[2] > IR_THRESHOLD) {
    return 180;
  }
  else if (ir[3] > IR_THRESHOLD) {
    return 270;
  }
  else {
    return -1;
  }
}

bool detect_forward_person(){
  int detect_count=0;
  int i=0;
  for(i=0; i<4; ++i) {
    if (ir[i] > IR_THRESHOLD) {
      detect_count = detect_count +1;
    }
  }
  
  if (detect_count >=2) {
    return false; 
  }
  
  if (ir[0] > IR_THRESHOLD) {
    return true;
  }
    
  return false; 
}

bool detect_person(){
  if (ir[3] > IR_THRESHOLD && ir[0] > IR_THRESHOLD) {
    return true;
  }
  int i=0;
  for (i=0; i<=2; i++) {
    if (ir[i] > IR_THRESHOLD && ir[i+1] > IR_THRESHOLD) {
      return true;
    }
  }
  return false; 
}

void send_to_wifi_module(){
  //softSerial.flush();
  //しきい値で場合0,1に分けた値をsoftware serialでwifi moduleに送る
  send_ir(ir[0],'A');
  send_ir(ir[1],'B');
  send_ir(ir[2],'C');
  send_ir(ir[3],'D');
  
  //しきい値で場合0,1,3に分けた値をsoftware serialでwifi moduleに送る  
  send_humid(humid0,'X');
  //send_humid(humid1,'Y');
  //softSerial.write('\n');
}

void send_ir(int value,char header) {
  //Serial.print(", send_ir header=");
  //Serial.print(header);
  //Serial.print(" send_value=");
  
  softSerial.write(header);
  if(value < IR_THRESHOLD) {
    //Serial.print("0");
    softSerial.write('0');
  } else {
    //Serial.print("1");
    softSerial.write('1');
  }
}

void send_humid(int value, char header){  
  softSerial.write(header);
  if(value > HUMID_WET_THRESHOLD) {
    //Serial.print('1');
    softSerial.write('1');    
  }
  else  {
    //Serial.print('0');
    softSerial.write('0');
  }
  
  Serial.println("");
}

void move_direction(int angle){
    if(angle == 0) {
      rotate(80,80,5000);
    }
    else if(angle == 90) {
      rotate(100,80,1000);
      rotate(80,80,5000);
    }
    else if(angle == 180) {
      rotate(-80,-80,5000);
    }
    else if(angle == 270) {
      rotate(80,100,1000);
      rotate(80,80,3000);
    }
}
void rotate(int r_motor_speed,int l_motor_speed,int move_time_ms){
  delayMicroseconds(10000);
  motor.speed(MOTOR_R, r_motor_speed);
  motor.speed(MOTOR_L, l_motor_speed);
  motor.brake(MOTOR_R);                 // brake
  motor.brake(MOTOR_L);                 // brake
  motor.stop(MOTOR_R);                  // stop
  motor.stop(MOTOR_L);                  // brake
  delay(move_time_ms);
}

void uda_uda(){
    int interval = 500;
    int r_speed = 80;
    int l_speed = 90;
    int i =0;
    int direction =1;
    //delay(1000);
    for (i=0; i < 5000 / interval; i++) {
      if(i % 2 == 0) {
        direction = 1;
      }
      else { 
        direction = -1;
      }
      delayMicroseconds(10000);
 
      motor.speed(MOTOR_R, direction * r_speed);
      motor.speed(MOTOR_L, direction * l_speed);
      delay(interval);
    }
      
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    
    /*
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    delay(5000);
    motor.speed(MOTOR_R, -100);           
    motor.speed(MOTOR_L, -100);           
    delay(5000);
    */
    motor.stop(MOTOR_R);                  // stop
    motor.stop(MOTOR_L);                 // brake
    delay(4000);    
}


void turn(){
    //delay(1000);
    delayMicroseconds(10000);
    motor.speed(MOTOR_R, -80);
    motor.speed(MOTOR_L, -90);
    delay(10000);
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    
    /*
    motor.brake(MOTOR_R);                 // brake
    motor.brake(MOTOR_L);                 // brake
    delay(5000);
    motor.speed(MOTOR_R, -100);           
    motor.speed(MOTOR_L, -100);           
    delay(5000);
    */
    motor.stop(MOTOR_R);                  // stop
    motor.stop(MOTOR_L);                 // brake
    delay(4000);    
}


void read_sensor_values(){
  ir[0]  = analogRead(A0); 
  ir[1]  = analogRead(A1); 
  ir[2]  = analogRead(A2); 
  ir[3]  = analogRead(A3); 
  humid0 = analogRead(A4);
  //humid1 = analogRead(A5);
  humid1 = 0; //湿度センサを２個使う未定なのでとりあえず0で埋める
  
  Serial.print("ir0=");
  Serial.print(ir[0]);
  Serial.print(", ir1=");
  Serial.print(ir[1]);
  Serial.print(", ir2=");
  Serial.print(ir[2]);
  Serial.print(", ir3=");
  Serial.print(ir[3]);
  
  Serial.print(", humid0=");
  Serial.print(humid0);
  //Serial.print(", humid1=");
  //Serial.print(humid1);
  Serial.println("");
}

bool demo_start() {
  return true;
  bool is_timeouted = false;
  unsigned long timeout = millis();
  if (millis() - timeout > 15000) {
    is_timeouted = true;
  }
  int i = 0;
  int detect_count = 0;
  for(i=0; i<4; ++i) {
    if (ir[i] > IR_THRESHOLD) {
      detect_count = detect_count +1;
    }
  }
  if (detect_count == 0 /*|| is_timeouted*/) {
    return true;
  }
  

  //TODO implement
  return true;
}



