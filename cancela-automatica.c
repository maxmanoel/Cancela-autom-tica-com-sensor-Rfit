#include #include #include #include #include
// ---- PINOS RFID ----
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

// Servo e buzzer
#define SERVO_PIN 5
#define BUZZER 7

Servo cancela;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// UID do cartão autorizado (93 D3 75 E4)
byte uidAutorizado = {0x93, 0xD3, 0x75, 0xE4};
int tamanhoUID = 4;

// Função para comparar UID
bool compararUID(byte *uidLido, byte *uidValido, int tamanho) {
for (int i = 0; i < tamanho; i++) {
if (uidLido != uidValido) return false;
}
return true;
}

void setup() {

Serial.begin(9600);
SPI.begin();
rfid.PCD_Init();

pinMode(BUZZER, OUTPUT);

cancela.attach(SERVO_PIN);
cancela.write(0);

lcd.init();
lcd.backlight();
lcd.setCursor(1, 0);
lcd.print("Passe o Cartao");

tone(BUZZER, 1000, 150);
delay(200);
}

void loop() {

// Verifica presença de novo cartão
if (!rfid.PICC_IsNewCardPresent()) return;
if (!rfid.PICC_ReadCardSerial()) return;

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("UID:");

// ---- Monta UID em String para exibir ----
String uidString = "";
for (byte i = 0; i < rfid.uid.size; i++) {
uidString += String(rfid.uid.uidByte < 0x10 ? "0" : "");
uidString += String(rfid.uid.uidByte, HEX);
}
uidString.toUpperCase();

Serial.print("UID detectado: ");
Serial.println(uidString);

lcd.setCursor(4, 0);
lcd.print(uidString);

delay(300);

// ---- Verifica se é o cartão permitido ----
if (compararUID(rfid.uid.uidByte, uidAutorizado, tamanhoUID)) {

lcd.setCursor(1, 1);
lcd.print("Acesso Liberado");

// Abre a cancela
cancela.write(90);

// Som de acesso liberado
for (int i = 0; i < 4; i++) {
tone(BUZZER, 1500);
delay(150);
noTone(BUZZER);
delay(150);
}

delay(3000);
cancela.write(0); // Fecha cancela

} else {

lcd.setCursor(1, 1);
lcd.print("Acesso Negado");

// Som de erro
for (int i = 0; i < 2; i++) {
tone(BUZZER, 500);
delay(200);
noTone(BUZZER);
delay(200);
}

delay(2000);
}

lcd.clear();
lcd.setCursor(1, 0);
lcd.print("Passe o Cartao");

// Encerrar comunicação com o cartão
rfid.PICC_HaltA();
}