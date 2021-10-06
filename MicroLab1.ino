 //storage variables for the LEDS, buzzer, and button
 const int button_Pin = 2;
 const int red_LED = 13;
 const int yellow_LED = 12;
 const int green_LED = 11;
 const int buzzer = 8;

 //Variables for button
 int buttonState = 0;
 int buttonPushCounter = 0;
 int lastButtonState = 0;

 //Timer for keeping track of the ticks 
 int timer = 0;

 char led_State; //Serve to change the color of which LED is on
 int timeStart;  //Start time of when the button was pressed
 
 void setup(){

   //set pins as outputs  or input respecrtively 
   pinMode(red_LED, OUTPUT);
   pinMode(yellow_LED, OUTPUT);
   pinMode(green_LED, OUTPUT);
   pinMode(button_Pin, INPUT);
   pinMode(buzzer, OUTPUT);

   Serial.begin(9600);

   cli();//stop interrupts

   //set timer1 interrupt at 1Hz
   TCCR1A = 0;// set entire TCCR1A register to 0
   TCCR1B = 0;// same for TCCR1B
   TCNT1  = 0;//initialize counter value to 0
   // set compare match register for 1hz increments
   OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
   // turn on CTC mode
   TCCR1B |= (1 << WGM12);
   // Set CS12 and CS10 bits for 1024 prescaler
   TCCR1B |= (1 << CS12) | (1 << CS10);  
   // enable timer compare interrupt
   TIMSK1 |= (1 << OCIE1A);

   sei();//allow interrupts

 }//end setup

 // Timer1's interrupt service routing (ISR)
 // The code in ISR will be executed every time timer1 interrupt occurs
 // That is, the code will be called once every second
 // TODO
 //   you need to set a flag to trigger an evaluation of the conditions
 //   in your state machine, then potentially transit to next state
 //
 ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz increments timer which acts as a clock 
   timer++;
 }

 void loop(){
   buttonState = digitalRead(button_Pin);  //Reads button input

   if(buttonState != lastButtonState)  //If the button  was pressed, button state will be different than last state.
   {
    if(buttonState == HIGH)
    {
      buttonPushCounter++;
      digitalWrite(led_State, LOW);
      digitalWrite(buzzer, LOW);
    }
    delay(100);
   }
   lastButtonState = buttonState;

   if (buttonPushCounter % 2 == 0) //Inital off state of the button press
   {
    timeStart = timer;
    startup();
   }else //Traffic lights will function. 
   {
    traffic();
   }
 }

int startup(){  //Default blinking state when traffic lights are off. Blinks at 1 second intervals
  digitalWrite(led_State, LOW);
  led_State = red_LED;
  if(timer % 2 == 0){
    digitalWrite(led_State, HIGH);
  }else
  {
    digitalWrite(led_State, LOW);
  }
  
}

int traffic(){
  int timeDifference =  timer - timeStart; //Functions as a time counter for the traffic lights. Reset back to timer ie 0 when the value is 43
  switch(timeDifference)  
  {
    case 0: //Red Lights will be on when the time is at 0
    digitalWrite(buzzer, LOW);
    digitalWrite(led_State, LOW);
    led_State = red_LED;
    digitalWrite(led_State, HIGH);
    break;
    
    case 20: //Green lights will be on when time is at 20
    digitalWrite(buzzer, LOW);
    digitalWrite(led_State, LOW);
    led_State = green_LED;
    digitalWrite(led_State, HIGH);
    break;

    case 40: //Yellow Lights will turn on when time is at 40
    digitalWrite(led_State, LOW);
    led_State = yellow_LED;
    digitalWrite(led_State, HIGH);
    break;

    case 17: case 37: //Buzzer turns on for the last 3 seconds of green yellow and red
    digitalWrite(buzzer, HIGH);
    break;

    case 43: //Resets the time counter down to zero. 
    timeStart = timer;
    break;

    default:
    break;
  }
}

