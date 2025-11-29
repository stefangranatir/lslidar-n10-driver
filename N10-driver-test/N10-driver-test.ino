#include <HardwareSerial.h>

HardwareSerial lidarSerial(1);

int counter=0;
int state=0;
uint8_t uartFrame[58];
void setup() {  
  Serial.begin(115200);
  lidarSerial.begin(230400, SERIAL_8N1, 4, 5);
}


void loop() {

  while(lidarSerial.available()){
    uint8_t byte=lidarSerial.read();
    switch(state){
      //Waiting untill byte is 0xA5
      case 0:
        if(byte==0xA5){
          uartFrame[0]=byte;
          state=1;
        }
        break;
      
      //Waiting until byte is 0x5A
      case 1:
        if(byte==0x5A){
          uartFrame[1]=byte;
          state=2;
          counter=0;
        }
        else{
          state=0;
          counter=0;
          memset(uartFrame, 0, 58);
        }
        break;
      
      //Filling rest of the frame
      case 2:
        if(lidarSerial.available()){
          uartFrame[counter+2]=byte;
          counter++;
          if(counter==56){
            state=0;
            counter=0;
            /*for(int i=0;i<58;i++){
              Serial.printf("%02X", uartFrame[i]);
            }
            Serial.printf("\n");*/
            uint16_t angle_bits_start_raw = (uartFrame[5] | (uartFrame[6]<<8))%36000;
            uint16_t end_angle_bits_start = (uartFrame[55] | (uartFrame[56]<<8))%36000;
            uint16_t delta;
            if(angle_bits_start_raw > end_angle_bits_start){
              delta = end_angle_bits_start + 36000 - angle_bits_start_raw; 
            }
            else{
              delta = end_angle_bits_start - angle_bits_start_raw;
            }
            uint8_t actualPoints=32;
            float angleIncrement = (float)delta/15;
            for(size_t group_idx=0;group_idx<16; ++group_idx){
              int current_angle = (angle_bits_start_raw + (int)(angleIncrement * group_idx)) % 36000;
              Serial.printf("%d ", current_angle);
            }
            Serial.printf("\n");
          }
        }
        break;

    }
  }

}
