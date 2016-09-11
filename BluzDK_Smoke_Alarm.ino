/*Copyright 2015 Gustavo Gonnet https://www.hackster.io/gusgonnet/water-detection-system-227b08/

Modified 2016 Thomas Rockenbauer, rockenbauer.thomas@gmail.com, http://fablab-leoben.at

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// This #include statement was automatically added by the Particle IDE.
#include "elapsedMillis/elapsedMillis.h"

//this reads the output pin of the smoke sensor every 1 second
#define SMOKE_READ_INTERVAL 1000

//this defines the frequency of the notifications sent to the user
#define SMOKE_FIRST_ALARM 10000 //10 seconds
#define SMOKE_SECOND_ALARM 60000 //1 minute
#define SMOKE_THIRD_ALARM 300000 //5 minutes
#define SMOKE_FOURTH_ALARM 600000 //10 minutes
#define SMOKE_FIFTH_ALARM 900000 //15 minutes

#define SMOKE_NOTIF "SMOKE"

elapsedMillis smoke_timer;
elapsedMillis smoke_alarm_timer;

int smoke_alarms_array[5]={SMOKE_FIRST_ALARM, SMOKE_SECOND_ALARM, SMOKE_THIRD_ALARM, SMOKE_FOURTH_ALARM, SMOKE_FIFTH_ALARM};
int smoke_alarm_index = 0;
bool smoke_detected = false;
unsigned long smoke_next_alarm = 0;

int SMOKE_SENSOR = D7;


void setup() {
 pinMode(SMOKE_SENSOR, INPUT);
 
 // set the transmit power to 0dBm, lower values result in longer battery life
 // value must be one of the following: -30, -20, -16, -12, -8, -4, 0, or 4 dBm
BLE.setTxPower(-12);
 
 // take control of the LED
RGB.control(true);

// scales brightness of all three colors, 0-255.
// the following sets the RGB LED brightness to 25%:
RGB.brightness(0);

// wait one more second
delay(1000);

// resume normal operation
RGB.control(false);

}

void loop() {
    System.sleep(SLEEP_MODE_CPU);

    smoke_check();
    
    if (smoke_detected) {
        smoke_notify_user();
    }
}

/*******************************************************************************
 * Function Name  : smoke_check
 * Description    : check smoke sensor at SMOKE_READ_INTERVAL, turns on led on D7 and raises alarm if smoke is detected
 * Return         : 0
 *******************************************************************************/
int smoke_check()
{
    if (smoke_timer < SMOKE_READ_INTERVAL) {
        return 0;
    }
    
    //time is up, so reset timer
    smoke_timer = 0;

    if (digitalRead(SMOKE_SENSOR)) {

        //if smoke is already detected, no need to do anything, because an alarm will be fired
        if (smoke_detected){
            return 0;
        }
        
        smoke_detected = true;
    
        //reset alarm timer
        smoke_alarm_timer = 0;

        //set next alarm
        smoke_alarm_index = 0;
        smoke_next_alarm = smoke_alarms_array[0];
        
    } else {
        
        smoke_detected = false;
    }
    return 0;
}

/*******************************************************************************
 * Function Name  : smoke_notify_user
 * Description    : will fire notifications to user at scheduled intervals
 * Return         : 0
 *******************************************************************************/
int smoke_notify_user()
{

    if (smoke_alarm_timer < smoke_next_alarm) {
        return 0;
    }

    
    //time is up, so reset timer
    smoke_alarm_timer = 0;
    
    //set next alarm or just keep current one if there are no more alarms to set
    if (smoke_alarm_index < arraySize(smoke_alarms_array)-1) {
        smoke_alarm_index = smoke_alarm_index + 1;
        smoke_next_alarm = smoke_alarms_array[smoke_alarm_index];
    }

    //send an alarm to user (this one goes to the dashboard)
    Particle.publish("Smoke", "smoke detected!", 60, PRIVATE);
    
    //send an alarm to user (this one goes to pushbullet servers)
    Particle.publish("pushbullet", "smoke detected!", 60, PRIVATE);
   
   return 0; 
}