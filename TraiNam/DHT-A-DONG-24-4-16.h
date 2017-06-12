#include <SevenSeg.h>
#include <Button.h>
#include "DHT.h"
#include <EEPROM.h>

// DHT setup
#define DHTPIN 3     
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
 
// 7 seg set up
SevenSeg disp1 (A1,A0,A4,A3,0,A2,A5);
SevenSeg disp2 (A1,A0,A4,A3,0,A2,A5);
const int numOfDigits1 = 2;
const int numOfDigits2 = 2;
int digitPins1 [ numOfDigits1 ]={13,11};
int digitPins2 [ numOfDigits2 ]={12,9};

int state_DHT;

// button setup
#define up_PIN 2            
#define down_PIN 1  
#define err_PIN 10       
#define set_PIN 8            
#define ok_PIN 7     
#define PULLUP true        
#define INVERT true        

// buffon define
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.
#define REPEAT_FIRST 500   //ms required before repeating on long press
#define REPEAT_INCR 100    //repeat interval for long press
#define MIN_COUNT 0
#define MAX_COUNT 59

Button up(up_PIN, PULLUP, INVERT, DEBOUNCE_MS);    
Button down(down_PIN, PULLUP, INVERT, DEBOUNCE_MS);
Button err(err_PIN, PULLUP, INVERT, DEBOUNCE_MS);
Button set(set_PIN, PULLUP, INVERT, DEBOUNCE_MS);    
Button ok(ok_PIN, PULLUP, INVERT, DEBOUNCE_MS);

enum {WAIT, INCR, DECR};              //The possible states for the state machine
uint8_t STATE;                        //The current state machine state
int count;                            //The number that is adjusted
int lastCount = -1;                   //Previous value of count (initialized to ensure it's different when the sketch starts)
unsigned long rpt = REPEAT_FIRST;     //A variable time that is used to drive the repeats for long presses
//======================
int relay_t = 5;
int relay_h = 6;
int buzz = 4;

int lim_up_h;
int lim_down_h;
int lim_up_t;
int lim_down_t;
int on_off_h;
int on_off_t;
int state = 0;
int state_1 = 0;
int err_t = 0;
int err_h = 0;

int state_sw = 0;
//void setup
void setup () {
pinMode(relay_h,OUTPUT);
pinMode(relay_t,OUTPUT);
pinMode(buzz,OUTPUT);

disp1 . setDigitPins ( numOfDigits1 , digitPins1 );
disp1 . setDigitDelay (1667) ;

disp2 . setDigitPins ( numOfDigits2 , digitPins2 );
disp2 . setDigitDelay (1667) ;

lim_up_h = EEPROM.read(0);
lim_down_h = EEPROM.read(1);
lim_up_t = EEPROM.read(2);
lim_down_t = EEPROM.read(3);
on_off_h = EEPROM.read(4);
on_off_t = EEPROM.read(5);
err_h = EEPROM.read(6);
err_t = EEPROM.read(7);

for(int i = 0; i<150; i++){
  disp1.write(lim_up_h);
  disp1.clearDisp();
  disp2.write("uh");
  disp2.clearDisp();
}
for(int i = 0; i<150; i++){
  disp1.write(lim_down_h);
  disp1.clearDisp();
  disp2.write("dh");
  disp2.clearDisp();
}
for(int i = 0; i<150; i++){
  disp1.write(lim_up_t);
  disp1.clearDisp();
  disp2.write("uC");
  disp2.clearDisp();
}
for(int i = 0; i<150; i++){
  disp1.write(lim_down_t);
  disp1.clearDisp();
  disp2.write("dC");
  disp2.clearDisp();
}

for(int i = 0; i<150; i++){
  disp1.write(err_t);
  disp1.clearDisp();
  disp2.write("rC");
  disp2.clearDisp();
}

for(int i = 0; i<150; i++){
  disp1.write(err_h);
  disp1.clearDisp();
  disp2.write("rH");
  disp2.clearDisp();
}
for(int i = 0; i<150; i++){
  disp1.write(on_off_h);
  disp1.clearDisp();
  disp2.write(on_off_t);
  disp2.clearDisp();
}
 }



void loop () {
int h_1 =  dht.readHumidity();  
int t_1 =  dht.readTemperature();

// display temperature
for(int i = 0; i<500; i++){
int t = t_1 + err_t;
setup_limit();
disp1.write(t);
disp1.clearDisp();
disp2 . write ( "\370C") ;
disp2 . clearDisp ();
if(t<=lim_down_t) {
  if(on_off_t == 1){digitalWrite(relay_t,LOW);
  }else{digitalWrite(relay_t,HIGH);}
}
if(t>=lim_up_t) {
  if(on_off_t == 1){digitalWrite(relay_t,HIGH);
  }else{digitalWrite(relay_t,LOW);}
}
}

// display humidity
for(int i = 0; i<500; i++){
int h = h_1 + err_h;
setup_limit();
disp1.write(h);
disp1.clearDisp();
disp2.write ( " h") ;
disp2.clearDisp();

if(h<=lim_down_h) {
  if(on_off_h == 1){digitalWrite(relay_h,LOW);
  }else{digitalWrite(relay_h,HIGH);}
  }       
if(h>=lim_up_h) {
  if(on_off_h == 1){digitalWrite(relay_h,HIGH);
  }else{digitalWrite(relay_h,LOW);}
  }
}
}
// chuong trinh con =============================

void setup_limit(){
  read_sw();
  if(up.wasPressed())  {setup_limit_t();}
  if(down.wasPressed()){setup_limit_h();}
  if(err.wasPressed()) {setup_err();}
  if(set.wasPressed()) {setup_on_off();}
  if(ok.wasPressed())  {eeprom_write();led_flash();}
  state = 0;
  state_1 = 0;
  }

// setup limit tempeture
void setup_limit_t(){
  while(state < 1){
    disp1.write("SE");
    disp1.clearDisp();
    disp2.write("\370C");
    disp2.clearDisp();
    read_sw();
    if(ok.wasPressed()){state ++;}
    if(up.wasPressed()){
      while(state_1==0){
          read_sw();
          disp1.write(lim_up_t);
          disp1.clearDisp();
          disp2.write("UP");
          disp2.clearDisp();
          if(up.wasPressed())   {lim_up_t++;disp1.write(00);}
          if(down.wasPressed()) {lim_up_t--;}
          if(ok.wasPressed())   {state_1=1; }
        }
        state_1=0; 
      }
       if(down.wasPressed()){
        while(state_1==0){
          read_sw();
          disp1.write(lim_down_t);
          disp1.clearDisp();
          disp2.write("dO");
          disp2.clearDisp();
          if(up.wasPressed())   {lim_down_t++;}
          if(down.wasPressed()) {lim_down_t--;}
          if(ok.wasPressed())   {state_1=1;}
        }
      }
      state_1=0; 
    }
  }
// setup limit tempeture
void setup_limit_h(){
  while(state < 1){
    disp1.write("SE");
    disp1.clearDisp();
    disp2.write(" H");
    disp2.clearDisp();
    read_sw();
    if(ok.wasPressed()){state ++;}
    if(up.wasPressed()){
      while(state_1==0){
          read_sw();
          disp1.write(lim_up_h);
          disp1.clearDisp();
          disp2.write("UP");
          disp2.clearDisp();
          if(up.wasPressed())   {lim_up_h++;}
          if(down.wasPressed()) {lim_up_h--;}
          if(ok.wasPressed())   {state_1=1; }
        }
        state_1=0; 
      }
       if(down.wasPressed()){
        while(state_1==0){
          read_sw();
          disp1.write(lim_down_h);
          disp1.clearDisp();
          disp2.write("dO");
          disp2.clearDisp();
          if(up.wasPressed())   {lim_down_h++;}
          if(down.wasPressed()) {lim_down_h--;}
          if(ok.wasPressed())   {state_1=1;}
        }
        state_1=0; 
      }
    }
  }
// setup err
void setup_err(){
  while(state < 1){
    state_1 = 0;
    read_sw();
    disp1.write("SE");
    disp1.clearDisp();
    disp2.write("ER");
    disp2.clearDisp();

    if(ok.wasPressed()){
    state++;
    }
    if(down.wasPressed()){
        while(state_1==0){
          read_sw();
          disp1.write(err_h);
          disp1.clearDisp();
          disp2.write("EH");
          disp2.clearDisp();
          if(up.wasPressed())   {err_h++;}
          if(down.wasPressed()) {err_h--;}
          if(ok.wasPressed())   {state_1=1;}
        }
        state_1=0; 
      }

      if(up.wasPressed()){
        while(state_1==0){
          read_sw();
          disp1.write(err_t);
          disp1.clearDisp();
          disp2.write("EC");
          disp2.clearDisp();
          if(up.wasPressed())   {err_t++;}
          if(down.wasPressed()) {err_t--;}
          if(ok.wasPressed())   {state_1=1;}
        }
      }
      state_1=0; 
    } 
  }

void setup_on_off(){
  while(state < 1){
    state_1 = 0;
    read_sw();
    disp1.write("SE");
    disp1.clearDisp();
    disp2.write("--");
    disp2.clearDisp();
    if(ok.wasPressed()){state++;}
    if(up.wasPressed()){
         while(state_1==0){
         read_sw();
         disp1.write(on_off_t);
         disp1.clearDisp();
         disp2.write("-C");
         disp2.clearDisp();
         if(up.wasPressed())  {on_off_t = 1;}
         if(down.wasPressed()){on_off_t = 0;}
         if(ok.wasPressed())  {state_1=1;}
        }
        state_1=0;       
        }
    if(down.wasPressed()){
         while(state_1==0){
         read_sw();
         disp1.write(on_off_h);
         disp1.clearDisp();
         disp2.write("-H");
         disp2.clearDisp();
         if(up.wasPressed())  {on_off_h = 1;}
         if(down.wasPressed()){on_off_h = 0;}
         if(ok.wasPressed())  {state_1=1;}
        }
        state_1=0;    
        } 
     if(set.wasPressed()){
      for(int z=0; z<10; z++){
        EEPROM.write(z,0);
        delay(5);
          }
          lim_up_h=0;
          lim_down_h=0;
          lim_up_t=0;
          lim_down_t=0;
          on_off_h=0;
          on_off_t=0;
          state = 0;
          state_1 = 0;
          err_t = 0;
          err_h = 0;
          led_flash();
          led_flash();
        }
      }
    }
void read_sw(){
  up.read();
  down.read();
  err.read();
  set.read();
  ok.read();
}
// eeprom write
void eeprom_write(){
  eeprom_write_h();
  eeprom_write_t();
  eeprom_write_on_off();
  eeprom_write_err();
  void  led_flash();
}

void eeprom_write_h(){
  EEPROM.write(0,lim_up_h);
  delay(5);
  EEPROM.write(1,lim_down_h);
  delay(5);
}

void eeprom_write_t(){
  EEPROM.write(2,lim_up_t);
  delay(5);
  EEPROM.write(3,lim_down_t);
  delay(5);
}

void eeprom_write_on_off(){
  EEPROM.write(4,on_off_h);
  delay(5);
  EEPROM.write(5,on_off_t);
  delay(5);
}

void eeprom_write_err(){
  EEPROM.write(6,err_h);
  delay(5);
  EEPROM.write(7,err_t);
  delay(5);
}

void led_flash(){
for(int j=0; j<1; j++){

for(int i=0; i<50; i++){
  disp1.write("\370 ");
  disp1.clearDisp();
  disp2 . write ( "\370 ") ;
  disp2 . clearDisp ();
  digitalWrite(buzz,HIGH);
}
for(int i=0; i<50; i++){
  disp1.write(" \370");
  disp1.clearDisp();
  disp2 . write ( " \370") ;
  disp2 . clearDisp ();
  digitalWrite(buzz,LOW);
}
}
}

void  buzz_(){
  for(int i=0; i<30; i++){
    digitalWrite(buzz,HIGH);
  }
  for(int i=0; i<30; i++){
    digitalWrite(buzz,LOW);
  }
}