
// Valores de ajustes para o seguidor de linha MIF



#define TRESHOLD 700                       // Valor de referencia para cor da linha branca
#define SPEED0 100                        // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 1 1 0 0) 
#define SPEED1 210*0.3                         // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 1 1 1 0) 

#define SPEED2 180*0.3                          // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 1 0 0) 
#define SPEED3 150*0.3                         // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 1 1 0)  
#define SPEED4 100*0.3                          // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 1 1 1) 

#define SPEED5 0                            // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 0 1 0) 
#define SPEED6 0                            // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 0 1 1) 
#define SPEED7 100*0.3                          // Valor de 0 a 255 para velocidade com a seguinte leitura do sensor (0 0 0 0 0 1) 

#define RUNTIME 50000                      // Valor para executar o percurso 







//-----------------------------------------


#include <SoftwareSerial.h>
#include "QTRSensors.h"

// Portas Bluetooth
#define PINBTX 12
#define PINBRX 8

SoftwareSerial serial1(PINBRX, PINBTX);

// Portas driver motor
#define PININ1 2
#define PININ2 4
#define PININ3 5
#define PININ4 7
#define PINENA 3
#define PINENB 6

// Portas led rgb
#define PINLEDR 11
#define PINLEDG 9
#define PINLEDB 10

// Portas sensor QTR
#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             13  // emitter is controlled by digital pin 2
//sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {
  A0, A1, A2, A3, A4, A5, A6, A7
},
NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

void setup() {

  serial1.begin(9600);
  rgbControl(255, 0, 0, 1000);
  rgbControl(0, 255, 0, 1000);
  rgbControl(0, 0, 255, 1000);
}

void loop() {
  if (serial1.available()) {
    rgbControl(0, 0, 255, 1);
    char dado = serial1.read();
    if (dado == '8' || dado == '2' || dado == '4' || dado == '6' || dado == '0') {
      motorOption(dado, 100, 100);
    }
    if (dado == 'z' || dado == 'x' || dado == 'y' || dado == 'a' || dado == 'b' || dado == 'c') {
      advancedOption(dado);
      
    }
  } else {
    rgbControl(0, 255, 0, 1);
  }
}

void advancedOption(char option) {
  // Função para controlar de opçãoes avançadas
  switch (option) {
    case 'a':
      readSensors();
      break;
    case 'b':
      followLineMEF();   // Seguidor de linha com Máquina de Estados Finitos
      break;
  }
}

void motorControl(int speedLeft, int speedRight) {
  // Função para controle do driver de motor

  // Definições das portas digitais
  pinMode(PININ1, OUTPUT);
  pinMode(PININ2, OUTPUT);
  pinMode(PININ3, OUTPUT);
  pinMode(PININ4, OUTPUT);
  pinMode(PINENA, OUTPUT);
  pinMode(PINENB, OUTPUT);

  // Ajustes motor da esquerda
  if (speedLeft < 0) {
    speedLeft = -speedLeft;
    digitalWrite (PININ3, HIGH);
    digitalWrite (PININ4, LOW);
  } else {
    digitalWrite (PININ3, LOW);
    digitalWrite (PININ4, HIGH);
  }

  // Ajustes motor da direita
  if (speedRight < 0) {
    speedRight = -speedRight;
    digitalWrite (PININ1, LOW);
    digitalWrite (PININ2, HIGH);
  } else {
    digitalWrite (PININ1, HIGH);
    digitalWrite (PININ2, LOW);
  }

  analogWrite (PINENA, speedLeft);
  analogWrite (PINENB, speedRight);
}

void motorOption(char option, int speedLeft, int speedRight) {
  // Função para controle de motor com pre definições
  switch (option) {
    case '6': // Esquerda
      motorControl(-speedLeft, speedRight);
      break;
    case '4': // Direita
      motorControl(speedLeft, -speedRight);
      break;
    case '2': // Trás
      motorControl(-speedLeft, -speedRight);
      break;
    case '8': // Frente
      motorControl(speedLeft, speedRight);
      break;
    case '0': // Parar
      motorControl(0, 0);
      break;
  }
}

bool motorStop(long runtime, long currentTime) {
  // Função de parada do robô
  if (millis() >= (runtime + currentTime)) {
    motorOption('0', 0, 0);
    int cont = 0;
    while (cont < 5) {
      rgbControl(255, 0, 0, 500);
      rgbControl(0, 0, 0, 500);
      cont++;
    }
    return false;
  }
  return true;
}

void rgbControl(int red, int green, int blue, long rumtime) {
  // Função para controle do led rgb
  pinMode(PINLEDR, OUTPUT);
  pinMode(PINLEDG, OUTPUT);
  pinMode(PINLEDB, OUTPUT);

  digitalWrite(PINLEDR, HIGH);
  digitalWrite(PINLEDG, HIGH);
  digitalWrite(PINLEDB, HIGH);

  analogWrite(PINLEDR, 255 - red);
  analogWrite(PINLEDG, 255 - green);
  analogWrite(PINLEDB, 255 - blue);
  delay(rumtime);
}

void readSensors(void) {
  // Função para leitura dos sensores
  qtra.read(sensorValues);
   serial1.println(' ');
  serial1.print(" s1 ");
  serial1.print(sensorValues[0]);
  serial1.print("  s2 ");
  serial1.print(sensorValues[1]);
  serial1.print("  s3 ");
  serial1.print(sensorValues[2]);
  serial1.print("  s4 ");
  serial1.print(sensorValues[3]);
  serial1.print("                                                                       s5 ");
  serial1.print(sensorValues[4]);
  serial1.print("  s6 ");
  serial1.print(sensorValues[5]);
  serial1.print("  s7 ");
  serial1.print(sensorValues[6]);
  serial1.print("  s8 ");
  serial1.println(sensorValues[7]);
  serial1.print(' ');
}

void followLineMEF(void) {
  // Função para controle do seguidor de linha em modo de maquina de estado finita
  bool flag = true;
  long currentTime = millis();

  while (flag) {
    flag = motorStop(RUNTIME, currentTime);
    qtra.read(sensorValues);

    // leitura do sensor (1 1 1 1 1 1)
    if (sensorValues[1] <= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] <= TRESHOLD) {
      motorOption('8', SPEED0, SPEED0);
      // leitura do sensor (0 1 1 1 1 0)
    } else if ( sensorValues[1] >= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED0);
      // leitura do sensor (0 0 1 1 0 0)
    } else if ( sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED0);

      // leitura do sensor (0 1 1 1 0 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED1);

      // leitura do sensor (0 0 1 1 1 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] >= TRESHOLD ) {
      motorOption('8', SPEED1, SPEED0);

      // leitura do sensor (0 0 1 0 0 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED2);
      // leitura do sensor (0 0 0 1 0 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD ) {
      motorOption('8', SPEED2, SPEED0);

      // leitura do sensor (0 1 1 0 0 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED3);

      // leitura do sensor (0 0 0 1 1 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED3, SPEED0);

      // leitura do sensor (1 1 1 0 0 0)
    } else if (sensorValues[1] <= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] <= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED4);
      // leitura do sensor (0 0 0 1 1 1)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] <= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] <= TRESHOLD) {
      motorOption('8', SPEED4, SPEED0);

      // leitura do sensor (0 1 0 0 0 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED5);

      // leitura do sensor (0 0 0 0 1 0)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED5, SPEED0);

      // leitura do sensor (1 1 0 0 0 0)
    } else if (sensorValues[1] <= TRESHOLD && sensorValues[2] <= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('8', SPEED0, SPEED6);

      // leitura do sensor (0 0 0 0 1 1)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] <= TRESHOLD && sensorValues[6] <= TRESHOLD) {
      motorOption('8', SPEED6, SPEED0);

      // leitura do sensor (1 0 0 0 0 0)
    } else if (sensorValues[1] <= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] >= TRESHOLD) {
      motorOption('6', SPEED7, SPEED7);
      // leitura do sensor (0 0 0 0 0 1)
    } else if (sensorValues[1] >= TRESHOLD && sensorValues[2] >= TRESHOLD && sensorValues[3] >= TRESHOLD && sensorValues[4] >= TRESHOLD && sensorValues[5] >= TRESHOLD && sensorValues[6] <= TRESHOLD) {
      motorOption('4', SPEED7, SPEED7);

    }
  }
  motorOption('0', 0, 0);
}
