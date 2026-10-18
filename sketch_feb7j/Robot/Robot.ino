// motor driver (digital pins)
int ENA = 9;  // right motor
int In1 = 8;
int In2 = 7;
int ENB = 3;  // left motor
int In3 = 5;
int In4 = 4;

int turnSpeedl;
int turnSpeedr;

int leftMotorSpeed = 85;  // قيمة سرعة المحرك الأيسر
int rightMotorSpeed = 80; // قيمة سرعة المحرك الأيمن
int turnSpeedL = 80 ;
int turnSpeedR = 80;

// IR sensors (digital pins)
int L2_IR = 13;  // left IR sensor
int C_IR = 11;  // central IR sensor
int R1_IR = 12;  // right IR sensor
int R2_IR = 10;  // back IR sensor
int L1_IR = 6;  // front right IR sensor

int L_IR_Value;
int C_IR_Value;
int R_IR_Value;
int B_IR_Value;
int FR_IR_Value;

int white = 1;
int black = 0;

// PID control variables
float Kp = 5.0;
float Ki = 0.0;
float Kd = 0.5;
float previousError = 0;
float integral = 0;

enum Direction {
  FORWARD,
  LEFT,
  RIGHT
};

Direction lastDirection = FORWARD;
int m = true ;

void setup() {
  // IR sensors
  pinMode(L2_IR, INPUT);
  pinMode(C_IR, INPUT);
  pinMode(R1_IR, INPUT);
  pinMode(R2_IR, INPUT);  // set the back sensor pin as input
  pinMode(L1_IR, INPUT);  // set the front right sensor pin as input

  // Motor control pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, leftMotorSpeed);  // تطبيق إشارة PWM للمحرك الأيسر
  analogWrite(ENB, rightMotorSpeed); // تطبيق إشارة PWM للمحرك الأيمن

  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);

  Serial.begin(9600);  // Start serial communication for debugging
}

void forward() {
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
}

void left() {
  analogWrite(ENA, turnSpeedR - 30);
  analogWrite(ENB, turnSpeedL);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
}

void right() {
  analogWrite(ENA, turnSpeedL);
  analogWrite(ENB, turnSpeedR - 30);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
}

void stop() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);

  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
}

void loop() {
  // Read IR sensor values
  int L1_IR_Value = digitalRead(L1_IR);
  int L2_IR_Value = digitalRead(L2_IR);
  int C_IR_Value = digitalRead(C_IR);
  int R2_IR_Value = digitalRead(R2_IR);
  int R1_IR_Value = digitalRead(R1_IR);


  // Calculate error
  if(m){
  float error = 0.0;
  if (L2_IR_Value == black) error -= 10.0;
  if (L1_IR_Value == black) error -= 5.0;
  if (C_IR_Value == black) error += 0.0;
  if (R1_IR_Value == black) error += 5.0;
  if (R2_IR_Value == black) error += 10.0;

  // Calculate PID terms
  float P = error;
  integral += error;
  float I = integral;
  float D = error - previousError;
  float correction = Kp * P + Ki * I + Kd * D;

  // Calculate motor speeds
  int leftSpeed = leftMotorSpeed + correction;
  int rightSpeed = rightMotorSpeed - correction;
  
  // Constrain speeds to maxSpeed
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  turnSpeedl =leftSpeed;
  turnSpeedr = rightSpeed;
  // Move the robot
  analogWrite(ENA, rightSpeed);
  analogWrite(ENB, leftSpeed);
  
  // Update previous error
  previousError = error;
  }
  if (C_IR_Value == black){
      m = true;
  }
  // Determine direction based on sensor values
  if (L2_IR_Value == black || L1_IR_Value == black || C_IR_Value == black || R1_IR_Value == black || R2_IR_Value == black) {
    if (L2_IR_Value == black) {
      lastDirection = LEFT;
      m = false ;
    } else if (R2_IR_Value == black) {
      lastDirection = RIGHT;
      m = false ;
    } else {
      lastDirection = FORWARD;
    }
    forward();
  } else {
    // If all sensors read white, move in the last correction direction
    if (lastDirection == LEFT) {
     // left();
      right();
    } else if (lastDirection == RIGHT) {
     // right();
      left();
    } else {
      forward();
    
    }
  }

  delay(1); // Delay for stability
}