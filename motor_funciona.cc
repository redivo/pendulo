//motor FUNCIONA PERFEITAMENTE, ativa o fim de curso sozinho qnd liga o sistema e não muda mais. Mas liga o motor.
//e só funciona um dos finais de curso 

#include <SpeedyStepper.h>
#include <TMCStepper.h>

// Definições de pinos
#define EN_PIN          13  // Pino de habilitação do driver
#define DIR_PIN         14  // Pino de direção
#define STEP_PIN        12  // Pino de passo
#define DRIVER_ADDRESS  0b00 // Endereço do driver TMC2209 conforme MS1 e MS2
#define R_SENSE         0.11f // Valor R_SENSE para o driver TMC2209

#define SERIAL_PORT Serial // Define Serial para ESP32

#define ENDSTOP_LEFT    32 // Pino do fim de curso esquerdo
#define ENDSTOP_RIGHT   33 // Pino do fim de curso direito

// Inicialização do driver TMC2209
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
using namespace TMC2209_n;

bool shaft = false;  // Usado para inverter a direção via UART

// Inicialização do controlador de passo
SpeedyStepper stepper;

// Variável para controlar a direção do motor
bool movingRight = true;


void setup() {
  // Configuração dos pinos de controle
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // Habilita o driver do motor

  // Configuração dos pinos dos finais de curso com pull-up interno
  pinMode(ENDSTOP_LEFT, INPUT_PULLUP);
  pinMode(ENDSTOP_RIGHT, INPUT_PULLUP);


  // Inicializa a conexão do stepper
  stepper.connectToPins(STEP_PIN, DIR_PIN);

  // Inicializa a comunicação serial para depuração
  SERIAL_PORT.begin(115200); // Inicializa a UART para o TMC2209
  Serial.begin(115200);       // Inicializa a comunicação serial padrão
  Serial.println("Iniciando comunicação serial...");

  // Configurações do driver TMC2209
  driver.begin();                // Inicializa o driver TMC2209
  driver.toff(5);                // Habilita o driver via software
  driver.rms_current(900);       // Define a corrente RMS do motor
  driver.microsteps(2);          // Define os micropassos para 1/2 passo
  driver.pwm_autoscale(true);    // Necessário para stealthChop
  driver.en_spreadCycle(true);   // Define SpreadCycle como ativo

  // Configurações de velocidade e aceleração do motor
  stepper.setSpeedInStepsPerSecond(1000);      // Define a velocidade em passos por segundo
  stepper.setAccelerationInStepsPerSecondPerSecond(2000); // Define a aceleração

}

void loop() {
  // Ler o estado dos finais de curso
  bool endstopLeft = digitalRead(ENDSTOP_LEFT) == LOW;  // Ativado quando LOW
  bool endstopRight = digitalRead(ENDSTOP_RIGHT) == LOW; // Ativado quando LOW
  
  // Exibir estado dos endstops para depuração
  Serial.print("Fim de curso esquerdo: ");
  Serial.println(endstopLeft ? "Ativado" : "Desativado");
  Serial.print("Fim de curso direito: ");
  Serial.println(endstopRight ? "Ativado" : "Desativado");

  // Trocar de direção ao atingir um fim de curso, se o motor estiver em movimento
  if (endstopLeft && !movingRight) {
     // Se está movendo para a esquerda e atingiu o fim de curso esquerdo
    movingRight = true; // Troca para a direita
   Serial.println("Fim de curso esquerdo atingido. Invertendo direção para a direita.");
    stepper.setSpeedInStepsPerSecond(10000); // Parar o motor (definindo a velocidade para 0)
    delay(20); // Aguarda um tempo para o motor parar completamente (ajustar se necessário)
    stepper.moveRelativeInSteps(100000); // Move para a direita
  }
  else if (endstopRight && movingRight) {
     //Se está movendo para a direita e atingiu o fim de curso direito
    movingRight = false; // Troca para a esquerda
    Serial.println("Fim de curso direito atingido. Invertendo direção para a esquerda.");
    stepper.setSpeedInStepsPerSecond(1000); // Parar o motor (definindo a velocidade para 0)
    delay(20); // Aguarda um tempo para o motor parar completamente (ajustar se necessário)
    stepper.moveRelativeInSteps(-100000); // Move para a esquerda
  }
  
  // Pequeno atraso para evitar flooding do monitor serial
  delay(10);
}
