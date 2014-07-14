/*

 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int jstkLongiInPin = A0;  // Analog input pin of the joystick longitudinal potentiometer
const int jstkTransInPin = A1;  // Analog input pin of the joystick transversal potentiometer

const int stbdMotorPin = 10;  // Analog output connected to the starboard motor
const int portMotorPin = 11;  // Analog output connected to the port motor
const int ledPin       = 13;  // Analog output with build in led 

int jstkLongiValue = 0;        // value read from the longitudinal pot
int jstkTransValue = 0;        // value read from the transversal pot
int stbdMotorValue = 0;
int portMotorValue = 0;

// nodes for linear equations / nodepoits are concatening  lines
float nodepoints[5][2]= {
  {
    0, 0  }
  , {
    512-50, 512  }
  , {
    512, 512  }
  , {
    512+50, 512  }
  ,{
    1024, 1024  }
};

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(stbdMotorPin, OUTPUT);   // sets the pin as output
  pinMode(portMotorPin, OUTPUT);   // sets the pin as output
}

void loop() {
  // read the analog in value:
  jstkLongiValue = analogRead(jstkLongiInPin);   // Positive forward
  jstkTransValue = analogRead(jstkTransInPin);   // Positive to starboard
  
  // Remap to add a deadband
  jstkLongiValue = reMap(nodepoints,jstkLongiValue);
  jstkTransValue = reMap(nodepoints,jstkTransValue);
  
  // Center value around 0
  jstkLongiValue -= 512;
  jstkTransValue -= 512;
  
  // compute the allocated value and 
  // map it to the range of the analog out:
  stbdMotorValue = map(jstkLongiValue-jstkTransValue, 0-512, 1023-512, 0, 255);
  portMotorValue = map(jstkLongiValue+jstkTransValue , 0-512, 1023-512, 0, 255);  
  
  stbdMotorValue = constrain(stbdMotorValue, 0, 255);
  portMotorValue = constrain(portMotorValue, 0, 255);

  analogWrite(stbdMotorPin, stbdMotorValue);
  analogWrite(portMotorPin, portMotorValue);   

  // print the results to the serial monitor:
  Serial.print("longi = " );                       
  Serial.println((jstkLongiValue+512)/1023.0*2-1); // Value between -1 and 1
  Serial.print("trans = " );                       
  Serial.println((jstkTransValue+512)/1023.0*2-1); // Value between -1 and 1   
  Serial.print("stbd = ");      
  Serial.println(stbdMotorValue);
  Serial.print("port = ");      
  Serial.println(portMotorValue); 

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(20);                     
}

//***************************************************************************
// Taken from http://interface.khm.de/index.php/lab/experiments/nonlinear-mapping/
int reMap(float pts[10][2], int input) {
  int rr;
  float bb,mm;

  for (int nn=0; nn < 4; nn++) {

    if (input >= pts[nn][0] && input <= pts[nn+1][0]) {
      mm= ( pts[nn][1] - pts[nn+1][1] ) / ( pts[nn][0] - pts[nn+1][0] );
      mm= mm * (input-pts[nn][0]);
      mm = mm +  pts[nn][1];
      rr = mm;
    }
  }
  return(rr);
}
