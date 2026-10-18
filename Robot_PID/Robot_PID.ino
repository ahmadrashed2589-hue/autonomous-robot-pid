// motor driver (digital pins)
int ENA = 5;  // right motor
int In1 = 2;
int In2 = 4;
int ENB = 3;  // left motor
int In3 = 6;
int In4 = 7 ;


int leftMotorSpeed = 60;  // قيمة سرعة المحرك الأيسر
int rightMotorSpeed = 80; // قيمة سرعة المحرك الأيمن
int turnSpeedL = 65;
int turnSpeedR = 65;

// IR sensors (digital pins)
int L2_IR = 8;  // left IR sensor
int L1_IR = 9;  // front right IR sensor
int C_IR = 10;  // central IR sensor
int R1_IR = 11;  // right IR sensor
int R2_IR = 12;  // back IR sensor

int L_IR_Value;
int C_IR_Value;
int R_IR_Value;
int B_IR_Value;
int FR_IR_Value;

int white = ;
int black = ;

// PID control variables
float Kp = 1.9;
float Ki = 0.0;
float Kd = 10.0;
float previousError = 0;
float integral = 0;

enum Direction {
  FORWARD,
  LEFT,
  RIGHT
};

Direction lastDirection = FORWARD;

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
  analogWrite(ENB, turnSpeedL + 20);
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);

  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
}

void right() {
  analogWrite(ENA, turnSpeedL + 20);
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
  int L2_IR_Value = digitalRead(L2_IR);
  int C_IR_Value = digitalRead(C_IR);
  int R2_IR_Value = digitalRead(R2_IR);
  int R1_IR_Value = digitalRead(R1_IR);
  int L1_IR_Value = digitalRead(L1_IR);

  // Calculate error
  float error = 0.0;
  if (L2_IR_Value == black) error -= 25.0;
  if (L1_IR_Value == black) error -= 15.0;
  if (C_IR_Value == black) error -= 0.0;
  if (R1_IR_Value == black) error += 15.0;
  if (R2_IR_Value == black) error += 25.0;

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
  // Move the robot
  analogWrite(ENA, rightSpeed);
  analogWrite(ENB, leftSpeed);

  // Update previous error
  previousError = error;

  // Determine direction based on sensor values
  if (L2_IR_Value == black || L1_IR_Value == black || C_IR_Value == black || R1_IR_Value == black || R2_IR_Value == black) {
    if (L2_IR_Value == black || L2_IR_Value == black && L1_IR_Value == black && C_IR_Value == black || L2_IR_Value == black && L1_IR_Value == black) {
      lastDirection = LEFT;
    } else if ( R2_IR_Value == black || C_IR_Value == black && R1_IR_Value == black && R2_IR_Value == black || R1_IR_Value == black && R2_IR_Value == black) {
      lastDirection = RIGHT;
    } else {
      lastDirection = FORWARD;
    }
    forward();
  } else {
    // If all sensors read white, move in the last correction direction
    if (lastDirection == LEFT ) {
      right();
      //left();
    } else if (lastDirection == RIGHT ) {
      left();
      //right();
    } else {
      forward();
    }
  }
  if (L2_IR_Value == black && L1_IR_Value == black && C_IR_Value == black && R1_IR_Value == black && R2_IR_Value == black) {
      left();
    }

delay(5); // Delay for stability
}
