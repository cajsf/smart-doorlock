#include <LiquidCrystal.h>
#include <Servo.h>

int Melody[] = {523, 659, 784, 1046};

int n1 = 8;
int n2 = 7;
int n3 = 6;
int shift1 = 13;
int shift2 = 9;
int piezo = 10;
int red = A2;
int green = A3;

Servo lockServo;
int servo = A0;
int lock = 90;
int unlock = 0;

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

String inputPassword = "";
String password = "1111";
String newPassword = "";
int incorrectAttempts = 0;
bool isChangeMode = false;
bool isAuthForChange = false;

void setup() {
  Serial.begin(9600);

  pinMode(n1, INPUT);
  pinMode(n2, INPUT);
  pinMode(n3, INPUT);
  pinMode(shift1, INPUT);
  pinMode(shift2, INPUT);
  pinMode(piezo, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);

  lockServo.attach(servo);
  lockServo.write(lock);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void loop() {
  if (digitalRead(shift1) == HIGH && digitalRead(shift2) == HIGH) {
    requestChange();
    delay(500);
  }
  if (digitalRead(n1) == HIGH) {
    NumberInput(1);
    delay(300);
  }
  if (digitalRead(n2) == HIGH) {
    NumberInput(2);
    delay(300);
  }
  if (digitalRead(n3) == HIGH) {
    NumberInput(3);
    delay(300);
  }
  if (inputPassword.length() == 4) {
    if (isChangeMode && !isAuthForChange) {
      Change();
    } else if (isAuthForChange) {
      changePassword();
    } else {
      checkPassword();
    }
  }
}

void NumberInput(int button) {
  int shiftValue = 0;
  if (digitalRead(shift1) == HIGH) {
    shiftValue = 1;
  } 
  if (digitalRead(shift2) == HIGH) {
    shiftValue = 2;
  }

  int number = (button - 1) * 3 + 1 + shiftValue;
  inputPassword += String(number);
  
  updateLCD();
}

void updateLCD() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(inputPassword);

  Serial.print("Current Password: ");
  Serial.println(inputPassword);
}

void requestChange() {
  isChangeMode = true;
  isAuthForChange = false;
  inputPassword = "";
  lcd.clear();
  lcd.print("Verify Password:");
  Serial.println("Verify current password to change");
}

void Change() {
  if (inputPassword == password) {
    isAuthForChange = true;
    lcd.clear();
    lcd.print("Enter New Pass:");
    Serial.println("Authenticated. Enter new password.");
    inputPassword = "";
  } else {
    lcd.clear();
    lcd.print("Access Denied");
    Serial.println("Access Denied");
    blinkLED(red);
    delay(2000);
    isChangeMode = false;
    lcd.clear();
    lcd.print("Enter Password:");
  }
}

void changePassword() {
  password = inputPassword;
  isChangeMode = false;
  isAuthForChange = false;
  lcd.clear();
  lcd.print("Password Changed");
  Serial.println("Password Changed to: " + password);
  delay(2000);
  inputPassword = "";
  lcd.clear();
  lcd.print("Enter Password:");
}

void checkPassword() {
  if (inputPassword == password) {
    lcd.clear();
    lcd.print("Access Granted");
    Serial.println("Access Granted");
    unlockDoor();
    playMelody();
    blinkLED(green);
    delay(2000);
    blinkLED(red);
    lockDoor();
    playMelodyreverse();
    incorrectAttempts = 0;
    Serial.println(incorrectAttempts);
  } else {
    lcd.clear();

    lcd.print("Access Denied");
    Serial.println("Access Denied");
    playwarning();
    blinkLED(red);
    Serial.println(incorrectAttempts);
    delay(1000);
    if(incorrectAttempts >= 5){
      Serial.println(incorrectAttempts);
      incorrectAttempts = 0;
      delay(10000);
    }
    incorrectAttempts += 1;
  }
  inputPassword = "";
  lcd.clear();
  lcd.print("Enter Password:");
}

void unlockDoor() {
  lockServo.write(unlock);
}

void lockDoor() {
  lockServo.write(lock);
}

void playMelody() {
  for(int i = 0; i < 4; i++){
    tone(piezo, Melody[i], 300);
    delay(100);
  }
  noTone(piezo);
}

void playMelodyreverse(){
  for(int i = 3; i > -1; i--){
    tone(piezo, Melody[i], 300);
    delay(100);
  }
  noTone(piezo);
}

void playwarning(){
  for (int i = 0; i < 3; i++) {
    tone(piezo, 1000, 200);
    delay(300);
  }
  noTone(piezo);
}

void blinkLED(int ledPin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
