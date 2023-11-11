
// Importación de librerías a utilizar
#include <MPU6050.h>
#include <Wire.h>

// Direcciones I2C para ambos sensores
MPU6050 Sensor1(0x68);  // Dirección I2C del sensor de espalda
MPU6050 Sensor2(0x69);  // Dirección I2C del sensor de cabeza

// Variables a ser utilizadas por los sensores
int16_t ax1, ay1, az1, gx1, gy1, gz1;          

// Declaración de posición inicial deseada de 90°
float d_a1 = 00.00, d_a2 = 00.00, d_a3 = 00.00;   
// Rangos de tolerancia para los sensores
float tol1 = 5.0, tol2 = 8.0, tol3 = 8.0;         
// Declaración del pin a asociar con el botón
const int boton = 5;                              
bool buttonPressed = false;
int dev_time = 0;
int cont = 0;
// "dev_2" se utilizará para la orientación lateral de la cabeza, y "dev_3" para la frontal
float dev_1 = 0, dev_2 = 0, dev_3 = 0;         
float c_a1 = 0, c_a2 = 0, c_a3 = 0;            

// * * * * *  * * Subprogramas * * * * * * *//

// Lectura de nuevas posiciones deseadas y cálculo de la desviación
void posicion(){
    Sensor1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
    Sensor2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);

    // Lectura del estado del botón y actualización de la posición deseada al presionarse     
    if (digitalRead(boton) == LOW) {               
      d_a1 = atan2(-ay1, az1) * (180.0 / PI);
      // Ángulo lateral deseado, almacenado
      d_a2 = atan2(-ax2, az2) * (180.0 / PI);     
      // Ángulo frontal deseado, almacenado
      d_a3 = atan2(-ay2, ax2) * (180.0 / PI);     

      // Impresión de las posiciones
      Serial.print("Nueva posición deseada 1: ");
      Serial.println(d_a1);
      Serial.print("Nueva posición lateral: ");
      Serial.println(d_a2);
      Serial.print("Nueva posición frontal: ");
      Serial.println(d_a3);           
      // Delay declarado para evitar múltiples lecturas al presionar el botón 
      delay(1000);                                 
    }

    // Cálculo del ángulo actual con base en los datos del giroscopio
    c_a1 = atan2(-ay1, az1) * (180.0 / PI);   
    // Cálculo de la desviación con respecto a la posición deseada
    dev_1 = c_a1 - d_a1;                      
    c_a2 = atan2(-ax2, az2) * (180.0 / PI);
    dev_2 = c_a2 - d_a2;
    c_a3 = atan2(-ay2, ax2) * (180.0 / PI);
    dev_3 = c_a3 - d_a3;    
}

// Espera a que se presione el botón
void espera() {
    while (digitalRead(boton) == HIGH) {
        // Espera de 100ms antes de verificar nuevamente
        delay(100); 
    }
}

// COnfiguración del pin del botón como entrada con una resistencia pull-up
void setup() {
  Wire.begin();
  Sensor1.initialize();
  Sensor2.initialize();
  Serial.begin(9600);
  pinMode(boton, INPUT_PULLUP);                    
  espera();
}

void loop() {

  posicion();
  
  // Verificación de si la desviación se encuentra dentro del rango
  if ((abs(dev_1) > tol1) || (abs(dev_2) > tol2) || (abs(dev_3) > tol3)) {     
    // Envío de la alerta e impresión de los valores obtenidos                
    Serial.println("¡¡¡Desviación detectada!!!");       
    Serial.print("  Valor deseado 1: ");
    Serial.println(d_a1);
    Serial.print("  Valor actual 1: ");
    Serial.println(c_a1);
    Serial.print("\n");

    Serial.print("  Valor deseado lateral: ");
    Serial.println(d_a2);
    Serial.print("  Valor actual lateral: ");
    Serial.println(c_a2);
    
    Serial.print("  Valor deseado frontal: ");
    Serial.println(d_a3);
    Serial.print("  Valor actual frontal: ");
    Serial.println(c_a3);    
    Serial.print("\n");

      // Contador de los segundos en desviación
      while ((abs(dev_1) > tol1) || (abs(dev_2) > tol2) || (abs(dev_3) > tol3)) {
        posicion();
        dev_time= dev_time + 1;                           

        // Condicionante que activará la alerta del motor vibrador
        if (dev_time > 10){
          for (cont=0; cont<5; cont++){
            Serial.println("¡¡¡¡¡Han pasado 10 segundos con mala postura!!!!!"); // Tarea a reemplazar
          } 
          cont = 0;
          dev_time = 0;
        } 
        delay(1000); 
      }
  } else {
    // Impresión de los valores obtenidos, esto para pruebas y evaluación
    Serial.println("Sigue jalando");                  
    Serial.print("  Valor deseado 1: ");                
    Serial.println(d_a1);
    Serial.print("  Valor actual 1: ");
    Serial.println(c_a1);
    Serial.print("\n");

    Serial.print("  Valor deseado lateral: ");                
    Serial.println(d_a2);
    Serial.print("  Valor actual lateral: ");
    Serial.println(c_a2);
    Serial.print("\n");

    Serial.print("  Valor deseado frontal: ");                
    Serial.println(d_a3);
    Serial.print("  Valor actual frontal: ");
    Serial.println(c_a3);
    Serial.print("\n");    
    dev_time= 0;
  }    

  // Monitoreo del contador y del estado del botón
  Serial.println(dev_time);                           
  Serial.println(digitalRead(boton));
  // Periodo de muestreo
  delay(2000);                                      
    
} 

