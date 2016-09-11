// This #include statement was automatically added by the Particle IDE.
#include "bluz_gateway/bluz_gateway.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
bluz_gateway gateway;

void setup() {
    gateway.init();
    
    //specify connection interval for more battery life
    gateway.set_connection_parameters(1000, 2000); 
    
    //only set this if you want the nrf51 central to not connect, recomended for Electron gateway
    // gateway.set_ble_local(true);
}

void loop() {
    gateway.loop();
}