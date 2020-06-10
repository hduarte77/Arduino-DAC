

#include <DHT_U.h>
#include <MQUnifiedsensor.h>


// Definimos el pin digital donde se conecta el sensor DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);



///// Configuracion para el sensor MQ2
#define         Board                   ("Arduino UNO")
#define         Pin                     (A0)  //Analog input para el sensor de Gases
#define         Type                    ("MQ-2") //MQ2
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ2CleanAir        (9.83) // RS / R0 = 9.83 ppm 

MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);



void setup() {
  //Inicializar el puerto serial
  Serial.begin(9600); //Init serial port

  //Inicializar el sensor DHT
  dht.begin();
  
  //Se define el modelo para el calculo de PPM de gases
  MQ2.setRegressionMethod(1); //_PPM =  a*ratio^b
  

  /***********************************************************************************/ 
  // CALIBRACIÓN DEL SENSOR
  /************************************************************************************/ 
  MQ2.init(); 
  float calcR0 = 0;
  
  int i =0;
  
  for(i = 1; i<=10; i ++)
  {
    MQ2.update();
    calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
  }
  
  MQ2.setR0(calcR0/10);
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}

}







void loop() {

  /**********************************************/
  /******** LECTURA DEL SENSOR DHT11 ***********/
  /**********************************************/

  float TEMP = dht.readTemperature();
  float HUM = dht.readHumidity();


  /**********************************************/
  /******** LECTURAS DEL SENSOR MQ2  ***********/
  /*********************************************/

  
  MQ2.update(); 
  
    /*
      Exponential regression:
      Gas    | a      | b
      H2     | 987.99 | -2.162
      LPG    | 574.25 | -2.222
      CO     | 36974  | -3.109
      Alcohol| 3616.1 | -2.675
      Propane| 658.71 | -2.168
    */
  
    MQ2.setA(987.99); MQ2.setB(-2.162); 
    float H2 = MQ2.readSensor(); 
  
    MQ2.setA(574.25); MQ2.setB(-2.222); 
    float LPG = MQ2.readSensor(); 
  
    MQ2.setA(36974); MQ2.setB(-3.109); 
    float CO = MQ2.readSensor(); 
  
    MQ2.setA(3616.1); MQ2.setB(-2.675); 
    float AL = MQ2.readSensor(); 
  
    MQ2.setA(658.71); MQ2.setB(-2.168); 
    float PRO = MQ2.readSensor(); 
  
    Serial.println(String(TEMP)+ ","+ String(HUM)+"," +String(H2) + "," + String(LPG) + "," + String(CO) + "," + String(AL) + "," + String(PRO));
  
  
    delay(200); 


}
