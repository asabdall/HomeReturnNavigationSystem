//Including all of the Header Files
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "mbed.h"
#include "hcsr04.h"
#include "math.h"
//IO declarations
Serial pc(USBTX, USBRX); //PC UART
//Supersonic Sensor Declaration
HCSR04 xsensor(D8,D9);//ECHO Pin=D9, TRIG Pin=D8
HCSR04 ysensor(D7,D6);//ECHO Pin=D7, TRIG Pin=D6
//Bluetooth module declaration
Serial HC06(PTC15, PTC14); //BT TX, RX
//Instantiate Send and Receive Variables
char snd[512], rcv[1000]; //send and receive buffer
//Instantiate Variable for the Size of the Room, and distance from walls in the x and y direction
int initial_x_row, initial_y_column,x_distance,y_distance,roomwidth, roomlength,current_row,current_column,obstacles;
stringstream strs;
int main()
{
   //default settings
    HC06.printf("-----Home Return Navigation System-----\n");//Printing Title of the Project to the Phone
    HC06.printf("Group Members: Ahmed Abdalla, Reza Soltani, Nujhat Tabassum\n");//Printing Members of the Project to the Phone
    HC06.printf("Please Enter the Width of the Room in Meters\n");//Asking the user to enter the room width
    HC06.gets(rcv, 1000);//Receiving the room width input from the user's phone
    roomwidth=stoi(rcv);//Setting the input equal to the input from the phone
    //Outputting Entered room size back to phone
    strs << roomwidth;
    string temp_str = strs.str();
    char* char_type = (char*) temp_str.c_str();
    HC06.puts("Room Width Is: ");
    HC06.puts(char_type);
    HC06.puts(" Meters\n");
    strs=stringstream();
    temp_str=string();
    char_type=(char*)"";

    //
    HC06.printf("Please Enter the Length of the Room in Meters\n");//Asking the user to enter the room size
    HC06.gets(rcv, 1000);//Receiving the room size input from the user's phone
    roomlength=stoi(rcv);//Setting the input equal to the input from the phone
    //Outputting Entered room size back to phone
    strs << roomlength;
    temp_str = strs.str();
    char_type = (char*) temp_str.c_str();
    HC06.puts("Room Length Is: ");
    HC06.puts(char_type);
    HC06.puts(" Meters\n");
    strs=stringstream();
    temp_str=string();
    char_type=(char*)"";
    int ishome=0;
    //Prompting User to choose when they have reached their desired home location
    while(ishome==0){
    HC06.printf("Please Enter \"HOME\" When you have reached the Desired home Location\n");
    HC06.gets(rcv, 1000);
    if(strstr(rcv,"HOME")!=NULL){
        ishome=1;
    }
    }
    //Setting Initial Position
        HC06.printf("Setting Home Location...\n");
        xsensor.start();
        wait_ms(500);
        ysensor.start();
        wait_ms(500);
        x_distance = xsensor.get_dist_cm();
        y_distance = ysensor.get_dist_cm(); 
        initial_x_row=round(x_distance/100);
        initial_y_column=round(y_distance/100);
        HC06.printf("Home Location Set!\n");
    //Looping For Direction Change
    int Done=0;
    while(1){
    //Ultrasound Sensor (HC-SR04) #1 Initialization
        obstacles=0;
        xsensor.start();
        wait_ms(500);
        ysensor.start();
        wait_ms(500);
    //Calculating Distance Percentage Remaining for Sensor # 1
        x_distance = xsensor.get_dist_cm();
        y_distance = ysensor.get_dist_cm();     
        current_row=round(x_distance/100);
        current_column=round(y_distance/100);
        HC06.puts("Directions to Home:");
        if (current_column<initial_y_column){
            if (current_row<initial_x_row){
                HC06.puts("Move Right & Backwards\n");
            }
            else if (current_row>initial_x_row){
                HC06.puts("Move Right & Forwards\n");
            }
            else{
                HC06.puts("Move Right\n");
            }
        }
        else if (current_column>initial_y_column){
            if (current_row<initial_x_row){
                HC06.puts("Move Left & Backwards\n");
            }
            else if (current_row>initial_x_row){
                HC06.puts("Move Left & Forwards\n");
            }
            else{
                HC06.puts("Move Left\n");
            }
        }
        else if (current_row<initial_x_row){
            HC06.puts("Move Backward\n");
        }
        else if (current_row>initial_x_row){
            HC06.puts("Move Forward\n");
        }
        else {
            HC06.puts("You Are Home\n");
        }
        HC06.puts("Obstacles:");
        if (current_column==0){
            obstacles=obstacles+1;
            HC06.puts("To The Left");
        }
        else if (current_column==roomwidth-1){
            obstacles=obstacles+1;
            HC06.puts("To The Right");
        }
        if (current_row==0){
            obstacles=obstacles+1;
            if(obstacles==2){
            HC06.puts(" & ");
            }
            HC06.puts("Straight Ahead");
        }
        else if (current_row==roomlength-1){
            obstacles=obstacles+1;
            if(obstacles==2){
            HC06.puts(" & ");
            }
            HC06.puts("Straight Backwards"); 
        }
        if(obstacles==0){
             HC06.puts("No Obstacles");
        }
        HC06.puts("\nErrors:");
        if(current_column>roomwidth || current_row>roomlength){
            HC06.puts("Distance from Wall Inconsistent, User has left room?\n");
        }
        else{
            HC06.puts("None\n");
        }
        HC06.puts("\n\n----------------------------\n\n");
        wait(5);
    }
}