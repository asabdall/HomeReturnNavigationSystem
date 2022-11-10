//Including all of the Header Files
#include <stdio.h>
#include <string.h>
#include "mbed.h"
#include <sstream>
//IO declarations
Serial pc(USBTX, USBRX); //PC UART
//DigitalOut led1(LED1);
DigitalOut GREEN(LED2); //Input Mode
DigitalOut BLUE(LED3); //output mode
DigitalIn sw2(SW2); //INPUT MODE SELECT
DigitalIn sw3(SW3); //Output mode select
//Bluetooth module declaration
Serial HC06(PTC15, PTC14); //BT TX, RX
//Supersonic Module Declaration


//Instantiate Send and Receive Variables
char snd[512], rcv[1000]; //send and receive buffer
int main()
{
    //Instantiate Variable for the Size of the Room
    int roomsize = 0;
   //default settings
    HC06.printf("Starting ECE 533 Final Project\n");//
    HC06.printf("Group Members: Ahmed Abdalla, Reza Soltani, Nujhat Tabassum\n");
    HC06.printf("Please Enter the Size of the Room you are In\n");
    HC06.gets(rcv, 1000);
    roomsize=stoi(rcv);
    stringstream strs;
    strs << roomsize;
    string temp_str = strs.str();
    char* char_type = (char*) temp_str.c_str();
    HC06.puts("Room Size Is: ");
    HC06.puts(char_type);
    HC06.puts(" feet");
}