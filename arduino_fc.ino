// Interrupt Pin Ids
const byte INTERRUPT_1 = 2;
const byte INTERRUPT_2 = 3;

struct Channel {
  volatile unsigned long int pulse_start;
  int pulse_width;
  int pulse_width_max;
  int pulse_width_min;
  
  int pulse_period; // 20ms for FS-iA6
};

// PPM signals go from 1000 - 1900. so a safe min is 1200, safe max is 1500 and a safe starting point is somewhere in the middle 1350.
// We use these values for pulse_width, pulse_width_min and pulse_width_max to initialize
Channel ch1 = {0, 1350, 1500, 1200, 20000};
Channel ch2 = {0, 1350, 1500, 1200, 20000};

void setup() {
  Serial.begin(9600);
   
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(2), ch1_ppm_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), ch2_ppm_change, CHANGE);

  Serial.println("begin");
}

void ch1_ppm_change()  { 
  int x = digitalRead(2);
  unsigned long int t = micros();
  
  if (x == HIGH) { // Rising edge
    ch1.pulse_period = t - ch1.pulse_start;
    ch1.pulse_start  = t;
  }else{           // Falling edge
    ch1.pulse_width = t -ch1.pulse_start;
  }
}

void ch2_ppm_change()  { 
  int x = digitalRead(3);
  unsigned long int t = micros();
  
  if (x == HIGH) { // Rising edge
    ch2.pulse_period = t - ch2.pulse_start;
    ch2.pulse_start  = t;
  }else{           // Falling edge
    ch2.pulse_width = t -ch2.pulse_start;
  }
}

void track_min_max(Channel *ch){
  if (ch->pulse_width_min > ch->pulse_width) { ch->pulse_width_min = ch->pulse_width; } // Track min
  if (ch->pulse_width_max < ch->pulse_width) { ch->pulse_width_max = ch->pulse_width; } // Track max
}

// Normalize width between given min max values 
int normalized_width(Channel *ch, int min_val, int max_val) {
  track_min_max(ch);
  return map(ch->pulse_width, ch->pulse_width_min, ch->pulse_width_max, min_val, max_val);
  //return  (int) ( (float)(ch->pulse_width - ch->pulse_width_min) * 100 / (float) (ch->pulse_width_max - ch->pulse_width_min)  );
}

void print_debug(){
   String chn1="|CH1| period:" + (String)ch1.pulse_period + " width:" + (String)ch1.pulse_width + " min:" + (String)ch1.pulse_width_min + " max:" + (String)ch1.pulse_width_max ;
   String chn2="|CH2| period:" + (String)ch2.pulse_period + " width:" + (String)ch2.pulse_width + " min:" + (String)ch2.pulse_width_min + " max:" + (String)ch2.pulse_width_max ;
   Serial.println(chn1); 
   Serial.println(chn2); 
}

void loop() {
  // put your main code here, to run repeatedly:
  int ch1_norm = normalized_width(&ch1, -100, 100);
  int ch2_norm = normalized_width(&ch2, 0   , 100);
  
  Serial.println("CH1: " + (String)ch1_norm + "  CH2: " + (String)ch2_norm);
  
  //mix_signals();

  // Debug and find channel mins and maxes
  // print_debug();
  
  delay(100);
 }
