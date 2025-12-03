#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- PINOS RFID ----
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

// Servo e buzzer
#define SERVO_PIN 5
#define BUZZER 7

Servo cancela;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// UID do cartão autorizado (PREENCHA COM O SEU)
byte uidAutorizado[] = {0xE3, 0x5A, 0xA1, 0x2F};  
int tamanhoUID = 4;

void setup() {

  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER, OUTPUT);

  cancela.attach(SERVO_PIN);
  cancela.write(0); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Passe o Cartao");

  tone(BUZZER, 1000, 150);
  delay(200);
}

bool compararUID(byte *uidLido, byte *uidValido, int tamanho) {
  for (int i = 0; i < tamanho; i++) {
    if (uidLido[i] != uidValido[i]) return false;
  }
  return true;
}

void loop() {

  // Verifica se há novo cartão
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Cartao Detectado");

  // Verifica se o cartão é permitido
  if (compararUID(rfid.uid.uidByte, uidAutorizado, tamanhoUID)) {

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Bem-Vindo");

    cancela.write(90);

    // som alerta
    for (int i = 0; i < 4; i++) {
      tone(BUZZER, 1500);
      delay(150);
      noTone(BUZZER);
      delay(150);
    }

    delay(3000);
    cancela.write(0);

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Passe o Cartao");

  } else {

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Acesso Negado");

    tone(BUZZER, 500, 400);
    delay(600);

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Passe o Cartao");
  }

  rfid.PICC_HaltA();
}
