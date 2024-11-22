//FUNCIONA A COMUNICAÇÃO MAS NÃO ATIVA O MOTOR
#include <TMCStepper.h>

#define EN_PIN          13  // Pino de habilitação do driver
#define DIR_PIN         14  // Pino de direção
#define STEP_PIN        12  // Pino de passo
#define DRIVER_ADDRESS  0b00 // Endereço do driver TMC2209 conforme MS1 e MS2
#define R_SENSE         0.11f // Valor R_SENSE para o driver TMC2209

#define SERIAL_PORT Serial // Serial para ESP32

// Pinos dos sensores de fim de curso
#define LIMIT_SWITCH_LEFT  32  // Fim de curso esquerdo
#define LIMIT_SWITCH_RIGHT 33  // Fim de curso direito

TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS); // Inicializa o driver TMC2209

bool shaft = true;

void setup() {
  // Configuração dos pinos de fim de curso
  pinMode(LIMIT_SWITCH_LEFT, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_RIGHT, INPUT_PULLUP);

  // Configuração do motor
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);  // Habilita o motor

  SERIAL_PORT.begin(115200);
  Serial.begin(115200);
  Serial.println("Teste de Direção com Finais de Curso");

  driver.begin();  
  driver.toff(5);
  driver.rms_current(900);  // Corrente nominal do motor
  driver.microsteps(16);    // Configuração de microstepping
  driver.pwm_autoscale(true);
}

void loop() {
  bool leftLimitTriggered = !digitalRead(LIMIT_SWITCH_LEFT);
  bool rightLimitTriggered = !digitalRead(LIMIT_SWITCH_RIGHT);

  if (leftLimitTriggered) {
    Serial.println("Fim de curso ESQUERDO acionado! Invertendo para direita.");
    driver.shaft(false); // Gira para a direita
  } 
  else if (rightLimitTriggered) {
    Serial.println("Fim de curso DIREITO acionado! Invertendo para esquerda.");
    driver.shaft(true);  // Gira para a esquerda
  }

  driver.VACTUAL(10000);  // Define velocidade constante (ajuste conforme necessário)
  driver.shaft(shaft);
  delay(100);  // Evita excesso de log e dá tempo para o motor ajustar

  shaft = !shaft;
}
