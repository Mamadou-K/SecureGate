#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// --- LCD ---
LiquidCrystal_I2C lcd(0x20, 16, 2);

// --- Servo ---
Servo gateServo;
const int SERVO_PIN = 9;

// --- LEDs and Buzzer ---
const int RED_LED   = 11;
const int GREEN_LED = 12;
const int BUZZER    = 10;

// --- Keypad ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2,3,4,5};
byte colPins[COLS] = {6,7,8,13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Password Variables ---
String setPassword = ""; // user-set password
String input = "";
bool passwordSet = false;

void setup() {
  Serial.begin(115200);

  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Gate closed
  
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Set Password:");
  lcd.setCursor(0,1);
  lcd.print("Press # to save");
}

// --- Helper Functions ---
void grantAccess() {
  digitalWrite(GREEN_LED, HIGH);
  gateServo.write(90); // Open gate
  lcd.clear(); lcd.print("Access Granted");
  delay(3000);
  gateServo.write(0); // Close gate
  digitalWrite(GREEN_LED, LOW);
  lcd.clear(); lcd.print("Enter Password:");
}

void denyAccess() {
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 100, 500);
  lcd.clear(); lcd.print("Access Denied!");
  delay(2000);
  digitalWrite(RED_LED, LOW);
  lcd.clear(); lcd.print("Enter Password:");
}

void clearInput() {
  input = "";
  lcd.clear();
  if (!passwordSet) lcd.print("Set Password:");
  else lcd.print("Enter Password:");
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  if (!passwordSet) {
    // --- Setting Password ---
    if (key == '#') {
      passwordSet = true;
      lcd.clear(); lcd.print("Password Saved!");
      delay(1500);
      lcd.clear(); lcd.print("Enter Password:");
    } 
    else if (key == '*') {
      setPassword = "";
      lcd.clear(); lcd.print("Cleared!");
      delay(1000);
      lcd.clear(); lcd.print("Set Password:");
    } 
    else {
      setPassword += key;
      lcd.setCursor(0,1); lcd.print(setPassword);
    }
  } 
  else {
    // --- Password Verification ---
    if (key == '#') {
      if (input == setPassword) grantAccess();
      else denyAccess();
      input = "";
    } 
    else if (key == '*') clearInput();
    else {
      input += key;
      lcd.setCursor(0,1);
	  for (int i = 0; i < input.length(); i++) lcd.print('*');
    }
  }
}
