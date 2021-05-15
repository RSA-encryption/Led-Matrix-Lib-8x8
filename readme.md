# How to import the library

1. Download the repository as zip
2. Go to Arduino IDE Sketch -> Include library -> Include zip library
3. Done

# Methods

| Method                            | Description                                             |
|-----------------------------------|---------------------------------------------------------|
| Matrix(const int latch, const int data, const int clock) | Constructor                      |
| Matrix::DrawText()                | Draws text on the led matrix                            |
| Matrix::Attach()                  | Sets passed pins to output mode                         |
| Matrix::IsBusy()                  | Checks if we are finished projecting @return boolean    |
| Matrix::Interrupt()               | Enable timer1 interrupt                                 |
| Matrix::SetString(char *string)   | Set string which you want to displey                    |
| Matrix::DrawMatrix(byte *arr)     | Draws your custom 2D byte array onto the led matrix     |
| Matrix::SetPoint(int x, int y)    | Sets point on the led matrix (0,0 being the start)      |
| Matrix::RemovePoint(int x, int y) | Removes point from the led matrix (0,0 being the start) |
| Matrix::DrawPoints()              | Draws set points                                        |

# Example usage
```
#include <Matrix.h>

Matrix controller(5,4,6);

ISR(TIMER1_COMPA_vect){
    controller.DrawText();
}

void setup() 
{
    controller.Attach(); //Setups pins
    controller.Interrupt(); //Enables interrupt routine
}

void loop(){
  if(controller.IsBusy()){
    controller.SetString("hello world");
  }
}
```

###### There are still some things that are not implemented I may come back to it when I have arduino near me again (Also I'll fix the interrupt routine timings)
