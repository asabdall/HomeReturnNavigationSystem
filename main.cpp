// Including all of the Header Files

#include "hcsr04.h"
#include "math.h"
#include "mbed.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "FXOS8700Q.h"

//Port And Module Declarations

Serial pc(USBTX, USBRX); // Serial PC USB Declaration
HCSR04 xsensor(D8, D9); // ECHO Pin=D9, TRIG Pin=D8,X Supersonic Sensor Declaration
HCSR04 ysensor(D7, D6); // ECHO Pin=D6, TRIG Pin=D7.Y Supersonic Sensor Declaration
Serial HC06(PTC15, PTC14); // Bluetooth Module declaration
DigitalOut GLed(LED1);//Green LED Declaration
DigitalOut RLed(LED2);//Red LED Declaration
DigitalOut Yled(LED3);//Yellow LED Declaration
I2C i2c(PTE25, PTE24);
FXOS8700QMagnetometer mag(i2c, FXOS8700CQ_SLAVE_ADDR1);
FXOS8700QAccelerometer acc(i2c, FXOS8700CQ_SLAVE_ADDR1);
//Variable Declarations

char snd[512], rcv[1000]; // send and receive buffer
int initial_x_row, initial_y_column, x_distance, y_distance, roomwidth,roomlength, current_row, current_column;

//Function Declarations

void room_size_printout(int printitem, char *printelement);
int LED_Displacement_Indicator(int distance, int row, int column, int old_row,int old_column);
void Obstacle_Output(int row, int column, int width,int length);       
void Error_Output(int row, int column, int width,int length) ;
void Device_Levelness_Output();

int main() {
  //initializing magnemometer
  acc.enable();
  // Program Menu
  HC06.printf("-----Home Return Navigation System-----\n"); // Printing Title of Project to the Phone
  HC06.printf("Group Members: Ahmed Abdalla, Reza Soltani, Nujhat Tabassum\n"); // Printing Members of the Project to the Phone
  // Asking User for Room Width and Outputting it back to the phone
  HC06.printf("Please Enter the Width of the Room in Meters\n"); // Asking the user to enter the room width
  HC06.gets(rcv, 1000);  // Receiving the room width input from the user's phone
  roomwidth = stoi(rcv); // Setting the input equal to the input from the phone
  room_size_printout(roomwidth, (char *)"Width");
  // Asking User for Room Length and Outputting it back to the phone
  HC06.printf("Please Enter the Length of the Room in Meters\n"); // Asking the user to enter the room size
  HC06.gets(rcv, 1000);   // Receiving the room size input from the user's phone
  roomlength = stoi(rcv); // Setting the input equal to the input from the phone
  room_size_printout(roomlength, (char *)"Length");
  void Direction_To_Home_Output(int row, int column, int old_row,int old_column);
  int ishome = 0;
  // Prompting User to choose when they have reached their desired home location
  while (ishome == 0) {
    HC06.printf("Please Enter \"HOME\" When you have reached the Desired home Location\n");
    HC06.gets(rcv, 1000);
    if (strstr(rcv, "HOME") != NULL) {
      ishome = 1;
    }
  }
  // Setting Initial Position
  HC06.printf("Setting Home Location...\n");
  xsensor.start();
  ThisThread::sleep_for(500);
  ysensor.start();
  ThisThread::sleep_for(500);
  x_distance = xsensor.get_dist_cm();
  y_distance = ysensor.get_dist_cm();
  initial_x_row = round(x_distance / 100);
  initial_y_column = round(y_distance / 100);
  HC06.printf("Home Location Set!\n");
  // Looping For Direction Change
  int old_distance = 0;
  while (1) {
    // Ultrasound Sensor (HC-SR04) #1 Initialization
    Device_Levelness_Output();
    xsensor.start();
    ThisThread::sleep_for(500);
    ysensor.start();
    ThisThread::sleep_for(500);
    // Calculating Distance Percentage Remaining for Sensor # 1
    x_distance = xsensor.get_dist_cm();
    y_distance = ysensor.get_dist_cm();
    current_row = round(x_distance / 100);
    current_column = round(y_distance / 100);
    // Indicating Whether the user is closer (Green), farther(Red), or the same distance from home (Yellow) with LEDs
    old_distance =LED_Displacement_Indicator(old_distance, current_row, current_column,initial_x_row, initial_y_column);
    // Finding the direction towards the home location from the current location
    Direction_To_Home_Output(current_row, current_column,initial_x_row, initial_y_column);
    // Finding if there are any close obstacles, and sending a warning
    // accordingly
    Obstacle_Output(current_row, current_column,roomwidth,roomlength);
    // Finding if the user's current position is outside the input room size.
    Error_Output(current_row, current_column,roomwidth,roomlength) ;
    HC06.puts("\n\n----------------------------\n\n");
    ThisThread::sleep_for(6000);
  }
}

void room_size_printout(int printitem, char *printelement) {
  stringstream strs;
  strs << printitem;
  string temp_str = strs.str();
  char *char_type = (char *)temp_str.c_str();
  HC06.puts("Room ");
  HC06.puts(printelement);
  HC06.puts(" Is: ");
  HC06.puts(char_type);
  HC06.puts(" Meters\n");
}

int LED_Displacement_Indicator(int distance, int row, int column, int old_row,
                               int old_column) {
  if (distance > (abs(row - old_row) + abs(column - old_column))) {
    RLed = 0;
    Yled = 1;
    GLed = 1;
  } else if (distance < (abs(row - old_row) + abs(column - old_column))) {
    RLed = 1;
    Yled = 1;
    GLed = 0;
  } else {
    RLed = 0;
    Yled = 1;
    GLed = 0;
  }
  return (abs(row - old_row) + abs(column - old_column));
}

void Direction_To_Home_Output(int row, int column, int old_row,int old_column) {
    HC06.puts("Directions to Home:");
    if (column < old_column) {
      if (row < old_row) {
        HC06.puts("Move Right & Backwards\n");
      } else if (row > old_row) {
        HC06.puts("Move Right & Forwards\n");
      } else {
        HC06.puts("Move Right\n");
      }
    } else if (column > old_column) {
      if (row < old_row) {
        HC06.puts("Move Left & Backwards\n");
      } else if (row > old_row) {
        HC06.puts("Move Left & Forwards\n");
      } else {
        HC06.puts("Move Left\n");
      }
    } else if (row < old_row) {
      HC06.puts("Move Backward\n");
    } else if (row > old_row) {
      HC06.puts("Move Forward\n");
    } else {
      HC06.puts("You Are Home\n");
    }
}

void Obstacle_Output(int row, int column, int width,int length) {
    int obstacles=0;
    HC06.puts("Obstacles:");
    if (column == 0) {
      obstacles = obstacles + 1;
      HC06.puts("To The Left");
    } else if (column == width - 1) {
      obstacles = obstacles + 1;
      HC06.puts("To The Right");
    }
    if (row == 0) {
      obstacles = obstacles + 1;
      if (obstacles == 2) {
        HC06.puts(" & ");
      }
      HC06.puts("Straight Ahead");
    } else if (row == length - 1) {
      obstacles = obstacles + 1;
      if (obstacles == 2) {
        HC06.puts(" & ");
      }
      HC06.puts("Straight Backwards");
    }
    if (obstacles == 0) {
      HC06.puts("No Obstacles");
    }
}

void Error_Output(int row, int column, int width,int length) {
    HC06.puts("\nErrors:");
    if (column > width || row > length) {
      HC06.puts("Distance from Wall Inconsistent, User has left room?\n");
    } else {
      HC06.puts("None\n");
    }
}   
void Device_Levelness_Output(){
    float x, y;
    int level_good=0;
    while(level_good==0){
        acc.getX(x);
        acc.getY(y);
        if(abs(y)>.40 || abs(x)>.40){
            HC06.puts("\nBOARD IS NOT LEVEL, LEVEL OUT BOARD\n");
        }
        else{
            level_good=1;
        }
    ThisThread::sleep_for(500);  
    }
}