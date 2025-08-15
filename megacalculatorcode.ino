#include <Keypad.h>
#include <LiquidCrystal.h>
#include <math.h>

// ===== LCD =====
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ===== Keypad =====
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'}, // A = +
  {'4','5','6','B'}, // B = -
  {'7','8','9','C'}, // C = *
  {'*','0','#','D'}  // D = /, * = clear, # = equals, √ handled separately
};
byte rowPins[ROWS] = {35, 37, 39, 41};
byte colPins[COLS] = {43, 45, 47, 49};
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ===== State =====
String num1 = "";
String num2 = "";
char op = 0;
bool enteringSecond = false;
const int MAX_DIGITS = 11;

// ===== Overloaded calculation methods =====
float calc(float a, float b, char oper) {
  switch (oper) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b != 0) ? a / b : NAN;
    default: return NAN;
  }
}

float calc(float a, char oper) { // unary op
  if (oper == 'R') { // sqrt
    return (a >= 0) ? sqrt(a) : NAN;
  }
  return NAN;
}

// ===== Helpers =====
void showLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(num1);
  if (op) {
    lcd.print(op);
    lcd.print(num2);
  }
}

void doCalc() {
  if (op == 'R') { // sqrt mode
    if (num1.length() == 0) return;
    float a = num1.toFloat();
    float r = calc(a, 'R');
    lcd.clear();
    lcd.setCursor(0,0);
    if (isnan(r)) lcd.print("Error");
    else lcd.print(r, 6);
  }
  else {
    if (num1.length() == 0 || num2.length() == 0) return;
    float a = num1.toFloat();
    float b = num2.toFloat();
    float r = calc(a, b, op);
    lcd.clear();
    lcd.setCursor(0,0);
    if (isnan(r)) lcd.print("Error");
    else lcd.print(r, 6);
  }

  // reset for next calculation
  num1 = "";
  num2 = "";
  op = 0;
  enteringSecond = false;
}

// ===== Setup =====
void setup() {
  lcd.begin(16, 2);
  lcd.print("Biz Calc Ready");
  delay(800);
  lcd.clear();
}

// ===== Main Loop =====
void loop() {
  char k = keypad.getKey();
  if (!k) return;

  if ((k >= '0' && k <= '9') || k == '.') {
    String &target = enteringSecond ? num2 : num1;
    if (target.length() < MAX_DIGITS) {
      if (k == '.' && target.indexOf('.') != -1) return; // only one decimal
      target += k;
    }
  }
  else if (k == 'A' || k == 'B' || k == 'C' || k == 'D') {
    if (!enteringSecond && num1.length() > 0) {
      if (k == 'A') op = '+';
      else if (k == 'B') op = '-';
      else if (k == 'C') op = '*';
      else if (k == 'D') op = '/';
      enteringSecond = true;
    }
  }
  else if (k == '#') { // equals
    doCalc();
    return;
  }
  else if (k == '*') { // clear
    num1 = "";
    num2 = "";
    op = 0;
    enteringSecond = false;
  }
  else if (k == '-') { // if you map sqrt to a spare key, example
    if (!enteringSecond && num1.length() > 0) {
      op = 'R'; // sqrt
      doCalc();
      return;
    }
  }

  showLCD();
}
