//================================================================================
//================================================================================
//	Author -> Adam Krajcovic

//TODO: Lacks mapping for numbers and special chars

#include "Matrix.h"

//================================================================================
//================================================================================
//	Matrix

Matrix::Matrix(const int latch, const int data, const int clock)
{
  _data = data;
  _clock = clock;
  _latch = latch;
}
bool Matrix::IsBusy() { //Checking if matrix is currently busy projecting text
  return ((_string[0] == NULL) ? true : false);
}
void Matrix::Attach() {//Set output
  pinMode(_latch, OUTPUT);
  pinMode(_data, OUTPUT);
  pinMode(_clock, OUTPUT);
}
void Matrix::Interrupt() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;   //TODO: Test on real arduino
  OCR1A = FREQ; //Should be around 60hz if my calculations are correct
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}
void Matrix::RegUpdateAll(int x){ //Update two registers in series
  static int c = 0;
  static byte s = B00000000;
  if (c > A_ROWS) c = 0;
  (s == 0x0) ? s += 1 : s <<= 1;
  RegUpdate(~s, _alpha[x][c]);
  c++;
}
void Matrix::RegUpdate(byte x, byte y)
{
  digitalWrite(_latch, LOW);
  shiftOut(_data, _clock, LSBFIRST, x); //Shift 16 bits and then enable output
  shiftOut(_data, _clock, LSBFIRST, y);
  digitalWrite(_latch, HIGH);
}
int Matrix::DumpIndex(const char c) { // Transform char to index that we can actually use when parsing
  if (isSpace(c)) return (A_COLUMNS - 1);
  char x = c;
  if (isLowerCase(x)) x -= 32;
  x = (int) x - 65;
  return (int) x;
}
void Matrix::SetString(char *s) {
  char temp[S_BOUND];
  temp[0] = ' ';
  strcat(temp, s);
  strcpy(_string, temp);
}
void Matrix::DrawText() {
  static unsigned long e = millis(), s = millis();
  static int c = 0;
  static byte x[A_ROWS] = { NULL };
  if (_string[0] == NULL) { //First index is null so we are at the end of the string
    RegUpdate(B00000000, B00000000);
    return;
  }
  else if (c == 0) { //Copy memory to coresponding array
    memcpy(x, _alpha[DumpIndex(_string[0])], sizeof(byte) * A_ROWS);
    c++;
  }
  else if (_string[0] != NULL && millis() > (s + PART_DELAY)) { //Shift logic
    Shift(x, _alpha[DumpIndex(_string[0])], _alpha[DumpIndex(_string[1])], c);
    (c == 8) ? c = 0 : c++;
    s = millis();
  }
  else if (_string[0] != NULL && millis() > (e + _delay)) { 
    memmove(&_string[0], &_string[1], strlen(_string)); //Shift memory by one to left once we shifted the byte array enough times
    e = millis();
  }
  DrawMatrix(x);
}
void Matrix::DrawMatrix(byte *arr){
  static int c = 0;
  static byte s = B00000000;
  if (c > A_ROWS) c = 0;
  (s == 0x0) ? s += 1 : s <<= 1;
  RegUpdate(~s, arr[c]);
  c++;
}
void Matrix::Shift(byte *o, byte *f, byte *s, int c) { 
  for (int i = 0; i < A_ROWS; i++) { //Essentialy shift the byte array by one to the left and then add part of next array
    if (_string[1] != NULL) {
      (c == 0) ? o[i] = f[i] << 1 : o[i] = o[i] << 1;
      o[i] += bitRead(s[i], A_ROWS - c);
    } else o[i] <<= 1;
  }
}
void Matrix::SetPoint(int x, int y) {
  if (bitRead(_points[A_ROWS - y], A_ROWS - x) == 0x1) return; //Don't add another bit if it's already 1 on certain index
  bitWrite(_points[A_ROWS - y], A_ROWS - x, 1);
}
void Matrix::RemovePoint(int x, int y) { 
  bitWrite(_points[A_ROWS - y], A_ROWS - x, 0); //We don't care if it's 0 or 1 just set it to 0
}
void Matrix::DrawPoints() {
  DrawMatrix(_points);
}