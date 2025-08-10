#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int errorPin = 2;
const int buzzerPin = 3;
const int ledPin = 4;
const int startPin = 6;
const int endPin = 7;
const int checkpointPin = 5;
const int bonusButtonPin = 8;

int score = 0;
int errors = 0;
bool checkpointPassed = false;
unsigned long startTime;
bool gameStarted = false;
bool shownReadyText = false;

bool bonusButtonPrevState = HIGH;
bool checkpointPrevState = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(errorPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(startPin, INPUT);
  pinMode(endPin, INPUT);
  pinMode(checkpointPin, INPUT_PULLUP);
  pinMode(bonusButtonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Refleks Oyunu");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long elapsed = (millis() - startTime) / 1000;

  if (!gameStarted && !shownReadyText) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bas ve Basla");
    shownReadyText = true;
  }

  if (!gameStarted && digitalRead(startPin) == HIGH) {
    gameStarted = true;
    shownReadyText = false;
    score = 0;
    errors = 0;
    checkpointPassed = false;
    startTime = millis();

    bonusButtonPrevState = digitalRead(bonusButtonPin);
    checkpointPrevState = digitalRead(checkpointPin);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Oyun Basladi!");
    delay(1000);
    lcd.clear();
  }

  if (gameStarted && score >= 100) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tebrikler!");
    lcd.setCursor(0, 1);
    lcd.print("Skor: ");
    lcd.print(score);
    delay(4000);

    gameStarted = false;
    lcd.clear();
    return;
  }

  if (gameStarted && digitalRead(endPin) == HIGH) {
    unsigned long totalTime = (millis() - startTime) / 1000;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Oyun Bitti!");
    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(score);
    lcd.print(" H:");
    lcd.print(errors);
    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sure: ");
    lcd.print(totalTime);
    lcd.print(" sn");

    lcd.setCursor(0, 1);
    lcd.print("Skor: ");
    lcd.print(score);
    delay(4000);

    gameStarted = false;
    lcd.clear();
    return;
  }

  if (gameStarted) {
    if (digitalRead(errorPin) == LOW) {
      errors++;
      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 1000, 200);
      delay(200);
      digitalWrite(ledPin, LOW);
      while (digitalRead(errorPin) == LOW);
    }

    bool checkpointState = digitalRead(checkpointPin);
    if (!checkpointPassed && checkpointPrevState == HIGH && checkpointState == LOW) {
      checkpointPassed = true;
      score += 5;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Checkpoint!");
      lcd.setCursor(0, 1);
      lcd.print("Skor +5");
      delay(1500);
      lcd.clear();
    }
    checkpointPrevState = checkpointState;

    bool bonusState = digitalRead(bonusButtonPin);
    if (bonusButtonPrevState == HIGH && bonusState == LOW) {
      score += 25;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BONUS!");
      lcd.setCursor(0, 1);
      lcd.print("Skor +25");
      delay(1500);
      lcd.clear();
    }
    bonusButtonPrevState = bonusState;

    lcd.setCursor(0, 0);
    lcd.print("Sure: ");
    lcd.print((millis() - startTime) / 1000);
    lcd.print(" sn   ");

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(score);
    lcd.print(" H:");
    lcd.print(errors);
  }

  delay(100);
}
