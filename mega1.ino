#define D1 7          
#define M1 8         
#define D2 4         
#define M2 2         
#define ENA 3
#define ENB 5

#define Trig2 58
#define Echo2 59
#define Trig1 54
#define Echo1 55

#define SERVO1 9 
#define SERVO2 11 
#define SERVO3 6
#define SERVO4 13
#define SERVO5 12
#include <Servo.h>
#include <Wire.h>
Servo head;
Servo claw;
Servo claw2;
Servo claw3;
Servo claw4;
int arraysize = 9;
int  rangevalue[]= {0, 0, 0, 0, 0, 0, 0, 0, 0};
int  rangevalueUP[]= {0, 0, 0, 0, 0, 0, 0, 0, 0};
char WHO_AM_I = 0x00;
char SMPLRT_DIV= 0x15;
char DLPF_FS = 0x16;
char GYRO_XOUT_H = 0x1D;
char GYRO_XOUT_L = 0x1E;
char GYRO_YOUT_H = 0x1F;
char GYRO_YOUT_L = 0x20;
char GYRO_ZOUT_H = 0x21;
char GYRO_ZOUT_L = 0x22;
char DLPF_CFG_0 = (1<<0);
char DLPF_CFG_1 = (1<<1);
char DLPF_CFG_2 = (1<<2);
char DLPF_FS_SEL_0 = (1<<3);
char DLPF_FS_SEL_1 = (1<<4);
char itgAddress = 0x69;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  char id=0; 
  id = itgRead(itgAddress, 0x00);  
  Serial.print("ID: ");
  Serial.println(id, HEX);
  
  //Configure the gyroscope
  //Set the gyroscope scale for the outputs to +/-2000 degrees per second
  itgWrite(itgAddress, DLPF_FS, (DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0));
  //Set the sample rate to 100 hz
  itgWrite(itgAddress, SMPLRT_DIV, 9);
  pinMode(D1, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(M2, OUTPUT);

  pinMode(Trig1, OUTPUT); 
  pinMode(Echo1, INPUT);
  pinMode(Trig2, OUTPUT); 
  pinMode(Echo2, INPUT);  

  head.attach(SERVO1);
  claw.attach(SERVO2);
  claw2.attach(SERVO3);
  claw3.attach(SERVO4);
  claw4.attach(SERVO5);

  Serial.println("start");

  head.write(77); // подровнять голову
   delay(1000);
   claw4.write(93);
   delay(1000);
   claw2.write(115); 
   delay(1000);
    claw.write(45); 
  delay(1000);
  claw3.write(143);

  delay(3000);
}

//константы:
unsigned int  dead_zone=45;
unsigned int  see_zone=10;
unsigned int object_zone=4;
unsigned int food_zone=2;

unsigned int ORight_zone=5;
unsigned int OLeft_zone=5;
unsigned int OHight_zone=2;

unsigned int foodWin_zone = 8;
//функции зрения:
//сонар1



void loop() 
{ 

  int resultUP = sonar1();
  int resultDOWN = sonar2();
  
if (resultUP > dead_zone && resultDOWN > see_zone) {goUP();}

else if (resultUP < dead_zone) {
STOP();
    Serial.println("srabotal sonar1, pereproverka:");
    resultUP = supersonar1();
    Serial.println(resultUP);
    if (resultUP  < dead_zone){
whereToTurn ();}}
 
else if (resultDOWN  < see_zone) 
  { 
    STOP();
    Serial.println("srabotal sonar2, pereproverka:");
    resultDOWN = supersonar2();
    Serial.println(resultDOWN);
    
    if (resultDOWN  < see_zone) {
    Serial.println("PEREPROVERKA: resultDOWN  < see_zone");
    delay(3000);
    int objectStart = supersonar2(); 
    delay(1000);
    
    if (objectStart < see_zone && objectStart < object_zone) 
    { 
      Serial.println("objectStart < object_zone \\ ytochnenie dla otdalenja:");  
      Serial.println(objectStart);
      departure	(objectStart, object_zone); 
      goto metka1;
      Serial.println("metka1"); 
    }
    
    else if (objectStart < see_zone && objectStart > object_zone)

    { Serial.println("objectStart > object_zone \\ ytochnenie dla pribligenya:");  
      Serial.println(objectStart);
      approach (objectStart, object_zone);
      goto metka1;
      Serial.println("metka1");
    }
    
    if (objectStart == object_zone )
    { metka1:
      Serial.println("objectStart == object_zone // win ");
      int objectHeight = frozeHeight(); 
      Serial.println("objectHeight:");
      Serial.println(objectHeight);
      int objectRight = frozeRight();
      Serial.println("objectRight:");
      Serial.println(objectRight);
      int objectLeft = frozeLeft();
      Serial.println("objectLeft:");
      Serial.println(objectLeft);
      //Если маленький по высоте)
      if (objectHeight >= OHight_zone && objectRight >= ORight_zone && objectLeft >= OLeft_zone) 
      {
            int objectCatch = supersonar2();
            if (objectCatch > food_zone)  
            {
            Serial.println("objectCatch:");
            Serial.println(objectCatch);   
            approach(objectCatch, food_zone);
            goto metka2;
            Serial.println("metka2");
            }
           else if (objectCatch <= food_zone){
             metka2:
                   clawfood();
                   // проверка верхним сонаром
                   int x_food = foodBase();
                   if (x_food == 1) {
                    Serial.println("YEEES:");
                     claw.write(180); 
                     delay(1000);
                     claw2.write(30); 
                     delay(1000);
                     delay(10000000);
                   }
                   else if (x_food == 0) {Serial.println("FAILLL:");
                 }
              }
            }
          //если большой по высоте)
      else if (objectHeight < OHight_zone)
      {
        Serial.println("bolshoi v visoty sdaemsa");
        claw3.write(69);
        delay(5000);
      }
      
       //Если сторона А длинная 
        else if (objectRight < ORight_zone)
        {
          Serial.println("fail storona A dlinnaya");
        claw3.write(69);
        delay(5000);
        }
          //Если сторона Б длинная
          else if (objectLeft < OLeft_zone)
          {
            Serial.println("fail storona b dlinnaja");
            claw3.write(69);
            delay(5000);
          }    
    }    
  }}
  

}

void itgWrite(char address, char registerAddress, char data)
{
  //Initiate a communication sequence with the desired i2c device
  Wire.beginTransmission(address);
  //Tell the I2C address which register we are writing to
  Wire.write(registerAddress);
  //Send the value to write to the specified register
  Wire.write(data);
  //End the communication sequence
  Wire.endTransmission();
}

//This function will read the data from a specified register on the ITG-3200 and return the value.
//Parameters:
//  char address: The I2C address of the sensor. For the ITG-3200 breakout the address is 0x69.
//  char registerAddress: The address of the register on the sensor that should be read
//Return:
//  unsigned char: The value currently residing in the specified register
unsigned char itgRead(char address, char registerAddress)
{
  //This variable will hold the contents read from the i2c device.
  unsigned char data=0;
  
  //Send the register address to be read.
  Wire.beginTransmission(address);
  //Send the Register Address
  Wire.write(registerAddress);
  //End the communication sequence.
  Wire.endTransmission();
  
  //Ask the I2C device for data
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 1);
  
  //Wait for a response from the I2C device
  if(Wire.available()){
    //Save the data sent from the I2C device
    data = Wire.read();
  }
  
  //End the communication sequence.
  Wire.endTransmission();
  
  //Return the data read during the operation
  return data;
}

//This function is used to read the X-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int xRate = readX();
int readX(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_XOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_XOUT_L);  
  
  return data;
}

//This function is used to read the Y-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int yRate = readY();
int readY(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_YOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_YOUT_L);  
  
  return data;
}

//This function is used to read the Z-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int zRate = readZ();
int readZ(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_ZOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_ZOUT_L);  
  
  return data;
}
int gyro() {
//Create variables to hold the output rates.
  int xRate, yRate, zRate;

  //Read the x,y and z output rates from the gyroscope.
  xRate = readX();
  yRate = readY();
  zRate = readZ();
delay(10);  


return xRate, yRate,  zRate;
  //Wait 10ms before reading the values again. (Remember, the output rate was set to 100hz and 1reading per 10ms = 100hz.)
  }

void headWrite() // крутить верхним сонаром
{
 head.write(0);
 head.write(180);
 head.write(77); 
 head.write(180);
 head.write(0);
 head.write(77);
}

//функции движения:
void goUP(){
  digitalWrite(D1, 1);
  digitalWrite(M1, 0);
  digitalWrite(D2, 1);
  digitalWrite(M2, 0);
  analogWrite(ENA, 254);
  analogWrite(ENB, 254);
}

void goDOWN() {
  digitalWrite(D1, 0);
  digitalWrite(M1, 1);
  digitalWrite(D2, 0);
  digitalWrite(M2, 1);
  analogWrite(ENA, 254);
  analogWrite(ENB, 254);
}

void STOP() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void downRIGHT() {
  analogWrite(ENA, 254);
  digitalWrite(D1, 0);
  digitalWrite(M1, 1);
}

void goRIGHT() {       
  analogWrite(ENA, 254);
  digitalWrite(D1, 1);
  digitalWrite(M1, 0);
}

void downLEFT() {       
  analogWrite(ENB, 254);
  digitalWrite(D2, 0);
  digitalWrite(M2, 1);
}

void goLEFT() {
  analogWrite(ENB, 254);
  digitalWrite(D2, 1);
  digitalWrite(M2, 0);
}

int sonar1(){
  int impulseTime;
  int distanceUP_sm;
  int contr = 0;
  metka3:
  impulseTime = 0;
  distanceUP_sm = 0;
  digitalWrite(Trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig1, LOW);
  impulseTime = pulseIn(Echo1, HIGH,50000);
  distanceUP_sm = impulseTime/58;
  delay(55);
  if (distanceUP_sm != 0) {return distanceUP_sm;}
  else if (distanceUP_sm == 0 && contr <= 10) {contr++; goto metka3;}
  else if (distanceUP_sm == 0 && contr > 10) {contr = 0; headWrite(); goto metka3;}
}
//сонар2
int sonar2(){
  int impulseTime;
  int distanceDOWN_sm;
  int contr = 0;
  metka4:
  impulseTime = 0;
  distanceDOWN_sm = 0;
  digitalWrite(Trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig2, LOW);
  impulseTime = pulseIn(Echo2, HIGH,50000);
  distanceDOWN_sm = impulseTime/58;
  delay(55);
  if (distanceDOWN_sm != 0) {return distanceDOWN_sm;}
  else if (distanceDOWN_sm == 0 && contr <= 10) {contr++; goto metka4;}
  else if (distanceDOWN_sm == 0 && contr > 10) {contr = 0; 
goDOWN(); 
delay(100);
STOP();
goto metka4;}
}

int find_similar(int *buf, int size_buff, int range) 
{
 int maxcomp=0; //счётчик максимального колличества совпадений
 int mcn;    //максимально часто встречающийся элемент массива
 int comp;    //временная переменная
 range++;    //допустимое отклонение

    for (int i=0; i<size_buff; i++) 
    {
        comp=buf[i];    //кладем элемент массива в comp
        int n=0;    //счётчик совпадении
        for (int j=0; j<size_buff; j++)    { if (buf[j]>comp-range && buf[j]<comp+range) n++;} // ищем повторения элемента comp в массиве buf    
        if (n > maxcomp) //если число повторов больше чем было найдено ранее
        {
            maxcomp=n; //сохраняем счетяик повторов
            mcn=comp; //сохраняем повторяемый элемент
        }        
    }
    
 return mcn;
}  

int supersonar1()

{for (int i = 0; i < arraysize; ++i) { rangevalueUP[i]=sonar1();
}//производим несколько замеров
     int cm = find_similar(rangevalueUP, arraysize, 0); //фильтруем показания датчика}
          return cm;}
 
 int supersonar2()

{for (int i = 0; i < arraysize; ++i) { rangevalue[i]=sonar2();
            }//производим несколько замеров
     int cm = find_similar(rangevalue, arraysize, 0); //фильтруем показания датчика}
          return cm;}




// функции клешни:

void clawStart() 
{
   delay(1000);
   claw2.write(115); 
   delay(1000);
    claw.write(45); 
  delay(1000);
  claw3.write(143);
}
// захват еды
void clawfood(){
  Serial.println("clawfood");
  void goUP();
  delay(100);
  delay(1000);
   claw2.write(102);
delay(1000);
  claw3.write(69);
   delay(1000);
  claw2.write(155);
   delay(1000);
   claw.write(180);
delay(1000);
claw3.write(143);
delay(1000);
clawStart();

}

void clawFinal() {
claw3.write(69);
  delay(1000);
  claw.write(45); 
  delay(1000);
  claw2.write(115); 
   delay(1000);
    claw2.write(102);
delay(1000);
   claw3.write(143);
   delay(5000);}
// распознавание:

void approach(int sonar, int zone) {
  int i = 0;
  while (sonar > zone && i<5){ 
    i++;
    goUP();
    delay(100);
    STOP();
    sonar = supersonar2();
    delay(1000);
  }}
  
void departure	(int sonar, int zone) {
int i = 0;
  while (sonar < zone && i<5) {
    i++;
    goDOWN();
    delay(100);
    STOP();
    sonar = supersonar2();
    delay(1000);}
}
int  foodBase(){
  delay(1000);
  int foodWinCentr = supersonar1();
  Serial.println("foodWinCentr:");
  Serial.println(foodWinCentr);
  head.write(30);
  delay(1000);
  int  foodWinRight = supersonar1();
   Serial.println("foodWinRight:");
  Serial.println(foodWinRight);
  head.write(130);
  delay(1000);
  int foodWinLeft = supersonar1();
  Serial.println("foodWinLeft:");
  Serial.println(foodWinLeft);
  head.write(77);
  if (foodWinCentr < foodWin_zone || foodWinRight < foodWin_zone || foodWinLeft < foodWin_zone) {
     return 1;}
  else {return 0;}
}

int frozeHeight() // zamer visoti
{     claw2.write(150); //смотрит на высоту предмета и замеряет (возможно потребуется подстройка сервы)
      delay(1000);
      int Height = supersonar2();
      claw2.write(100);
      delay(1000);
    return Height;}
    
int frozeRight() {
        claw2.write(150);
        delay(1000);
        downRIGHT();
        delay(200);
        STOP();
        claw2.write(100);
        delay(1000);
        int Right = supersonar2();
       claw2.write(150);
       delay(1000);
        goRIGHT();
        delay(200); // ПРОВЕРИТЬ НА КОРРЕКТНОСТЬ
        STOP();
        claw2.write(100);
        delay(1000);
      return Right;}
      
int frozeLeft() 
{         claw2.write(150);
          delay(1000);
          downLEFT();
          delay(200); // ПРОВЕРИТЬ НА КОРРЕКТНОСТЬ
          STOP();
          claw2.write(100);
          delay(1000);
          int Left = supersonar2();
          claw2.write(150);
          delay(1000);
          goLEFT();
          delay(200);
          STOP();
          claw2.write(100);
          delay(1000);
        return Left;}
        
void whereToTurn () {
    head.write(0);
    int distanceR_sm = supersonar1();
    delay(1000);
    head.write(160);
    int distanceL_sm = supersonar1();
    delay(1000);
    head.write(77);
    delay(1000);
    if (distanceR_sm > distanceL_sm) {
      downRIGHT();
      delay(1340); 
      STOP();
      Serial.println("distanceR_sm > distanceL_sm;");
      Serial.println(distanceR_sm);
      Serial.println(distanceL_sm);
    }
    else if (distanceR_sm < distanceL_sm){
      downLEFT();
      delay(1190); 
      STOP();
      Serial.println("distanceR_sm < distanceL_sm");
      Serial.println(distanceR_sm);
      Serial.println(distanceL_sm);
    }
}
