#include <Servo.h>
#include <Stepper.h>

// Definição de pinos
const int servoPin = 52;
const int onOffSwitchPin = 53;
const int verticalLimitPin = 23;
const int horizontalLimitPin = 22;
const int ledPin = 50;

//TCS3200 pinos
const int s0Pin = 48;
const int s1Pin = 49;
const int s2Pin = 46;
const int s3Pin = 47;
const int tcsOutPin = 45;
const int tcsLedPin = 44;

//driver com o ULN2003A pinos (para o motor do braço)
const int stepperPin1 = 8;
const int stepperPin2 = 10;
const int stepperPin3 = 9;
const int stepperPin4 = 11;

//driver A4988 pinos (motor vertical)
const int motor2StepPin = 35;
const int motor2DirPin = 34;
const int motor2Ms1Pin = 39;
const int motor2Ms2Pin = 37;
const int motor2Ms3Pin = 36;
const int motor2EnablePin = 38;

// Constantes para direções
const int downDirection = LOW;
const int upDirection = HIGH;
const int leftDirection = -1;
const int rightDirection = 1;

// passo por volta motor do braço
const int stepsPerRevolution = 32;

Servo gripperServo;
Stepper armStepper(stepsPerRevolution, stepperPin1, stepperPin2, stepperPin3, stepperPin4);

bool initialPosition; // 0 para "sim" e 1 para "não"

void setup() {
  Serial.begin(9600);

  pinMode(onOffSwitchPin, INPUT);
  pinMode(verticalLimitPin, INPUT_PULLUP);
  pinMode(horizontalLimitPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);
  pinMode(tcsLedPin, OUTPUT);
  pinMode(tcsOutPin, INPUT);
  pinMode(motor2StepPin, OUTPUT);
  pinMode(motor2DirPin, OUTPUT);
  pinMode(motor2Ms1Pin, OUTPUT);
  pinMode(motor2Ms2Pin, OUTPUT);
  pinMode(motor2Ms3Pin, OUTPUT);
  pinMode(motor2EnablePin, OUTPUT);

  armStepper.setSpeed(1250);
  gripperServo.attach(servoPin);
  gripperServo.write(90);

  digitalWrite(motor2Ms1Pin, LOW);
  digitalWrite(motor2Ms2Pin, HIGH);
  digitalWrite(motor2Ms3Pin, LOW);
  digitalWrite(motor2EnablePin, HIGH);

  digitalWrite(tcsLedPin, LOW);
  digitalWrite(s0Pin, HIGH);
  digitalWrite(s1Pin, LOW);

  initialPosition = 1;
}

void loop() {
  if (digitalRead(onOffSwitchPin) == LOW) {
    // Quando desligado
    digitalWrite(ledPin, LOW);
    initialPosition = 1;
    gripperServo.write(90);
    digitalWrite(tcsLedPin, LOW);
    digitalWrite(motor2EnablePin, HIGH);  
  }
  else {
    // Quando ligado
    digitalWrite(ledPin, HIGH);

    // Coloque a máquina na posição inicial
    if (initialPosition == 1) {     
      while(true) {
        moveArm(1, leftDirection);
        if (digitalRead(horizontalLimitPin) == LOW) {
          break;   
        }
      }
      while(true) {
        moveVertical(1, downDirection);
        if (digitalRead(verticalLimitPin) == LOW) { 
          break;  
       } 
      }
      moveVertical(10000, upDirection);
      moveArm(2000, rightDirection);
      initialPosition = 0;  
      }
    }

    // Leitura e separação das peças por cor
    if (initialPosition == 0) {   
      digitalWrite(tcsLedPin, HIGH);
      detectColor();
      /*Serial.print("Vermelho: ");
      Serial.print(vermelho);
      Serial.print(" Verde: ");
      Serial.print(verde);
      Serial.print(" Azul: ");
      Serial.print(azul);
      Serial.print(" Branco: ");
      Serial.print(semfiltro);
      Serial.println();*/

      if (vermelho < azul && vermelho < verde && semfiltro < 100) {
        // Peça vermelha
        //Serial.println("Vermelho");
        moveArm(500, rightDirection);
        moveVertical(8000, downDirection);
        gripperServo.write(180);
        moveVertical(8000, upDirection);
        moveArm(1900, leftDirection);
        moveVertical(10000, downDirection);
        gripperServo.write(90);
        moveVertical(10000, upDirection);
        moveArm(1400, rightDirection);
      }

      if (azul < vermelho && azul < verde && semfiltro < 100) {
        // Peça azul
        //Serial.println("Azul");
        moveArm(500, rightDirection);
        moveVertical(8000, downDirection);
        gripperServo.write(180);
        moveVertical(8000, upDirection);
        moveArm(1200, leftDirection);
        moveVertical(9000, downDirection);
        gripperServo.write(90);
        moveVertical(9000, upDirection);
        moveArm(700, rightDirection);
      }

      if (verde < azul && verde < vermelho && semfiltro < 100) {
        // Peça verde
        //Serial.println("Verde");
        moveArm(500, rightDirection);
        moveVertical(8000, downDirection);
        gripperServo.write(180);
        moveVertical(8000, upDirection);
        moveArm(500, leftDirection);
        moveVertical(7000, downDirection);
        gripperServo.write(90);
        moveVertical(7000, upDirection);
      }
    }
  }
}

void detectColor() { //função para detectar o nivel incidente de cada cor de acordo com as especificaçoes do sensor TCS3200 
  //Vermelho
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  vermelho = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Sem filtro
  digitalWrite(s2, HIGH);
  semfiltro = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Azul
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  azul = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Verde
  digitalWrite(s2, HIGH);
  verde = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
 }
}

void moveVertical(int stepDef, bool directionDef) //função para acionar o motor vertical 
 {
  digitalWrite(motor2DirPin, directionDef);
  digitalWrite(motor2EnablePin , LOW);
  for(int i = 0; i <= stepDef; i++)
  {
    digitalWrite(motor2StepPin , HIGH);
    delayMicroseconds(500);
    digitalWrite(motor2StepPin , LOW);
    delayMicroseconds(500);   
  }
  digitalWrite(motor2EnablePin, HIGH);
}

void moveArm(int stepDef, int directionDef) //função para acionar o motor vertical 
{  
  stepDef = stepDef*directionDef;
  braco.step(stepDef);   
}
