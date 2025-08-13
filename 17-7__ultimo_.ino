#include <Servo.h>
#include <Wire.h>

Servo I_AD; //Servo Delantero Izquierdo
Servo D_AD; //Servo Delantero Derecho
Servo I_AT; //Servo Trasero Izquierdo
Servo D_AT; //Servo Trasero Derecho
Servo C; //Servo de la camara
Servo B;

char n = 'r'; //Char para enviar la bandera a la Raspberry
String errorString = ""; //Cadena para guardar el error recibido
String banderaString = ""; //Cadena para guardar la bandera recibida
bool errorComplete = false; //Bandera boleana que nos indica cuando el comando fue recibido y podemos compararlo con los 2 comandos válidos
bool banderaComplete = false;
int bandera;
int p, d;
int u;
int K;
int direccion = 31; //Direccion asignada al Arduino, ulilizar esta direccion en el codigo de la Raspberry Pi
int error;
int Echo = 2;
int Trigger = 3;
int Contador = 1;
int Discontinua = 0;
int RND;
int SensorI = 12;
int SensorD = 8;
int SensorA = 9;
int SensorO = 13;
long dis;
long tiempo = 0;
float i = 0;
float p_old = 0;

int ZONA = 0;

int VelBase_inicial = 11;
int VelBase = VelBase_inicial; //Velocidad base
int VelMin = -90; //Velocidad minima de los motores
int VelMax = 90; //Velocidad máxima
int Vi = 0;
int Vd = 0;

float kp_inicial = 0.2; //Constante proporcional
float ki_inicial = 0.00007; //Constante integral
float kd_inicial = 0.1; //Constante derivable

float kp = kp_inicial; //Constante proporcional
float ki = ki_inicial; //Constante integral
float kd = kd_inicial; //Constante derivable

int Plateadas = 0;
int Negras = 0;
int Cargadas = 0;
bool Plateada = false;
bool Negra = false;
bool Cargada = false;
bool Inclinado = false;
bool De_costado = false;
bool Brazo_abajo = false; 
bool Zona = false;
bool VerEnPantalla = false;

void (* funcReset)() = 0;

void setup() {
  if (VerEnPantalla) {
    Serial.begin(9600);    
  }
  Wire.begin(direccion); //Nos unimos al bus i2c bus con la direccion asignada
  Wire.onReceive(Recibir); //Registramos el evento de recepcion de datos
  Wire.onRequest(Enviar);
  errorString.reserve(200); //Reservar 200 bytes para el errorString:

  I_AD.attach(4);
  D_AD.attach(7);
  I_AT.attach(5);
  D_AT.attach(6);
  
  C.attach(11);
  C.write(0);

  B.attach(10);
  B.write(180); //60 carga

  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);

  pinMode(SensorI, INPUT);
  pinMode(SensorD, INPUT);
  pinMode(SensorA, INPUT);
  pinMode(SensorO, INPUT);

  Izquierdo(0, 1);
  Derecho(0, 1);
  delay(1000);
}

void loop() {   
  /*Serial.print("Sensor Izquierdo: ");
  Serial.print(digitalRead(SensorI));
  Serial.print(" - Sensor Derecho: ");
  Serial.print(digitalRead(SensorD));
  Serial.print(" - Sensor Adelante: ");
  Serial.println(digitalRead(SensorA));*/

  if (banderaComplete) { //El comando fue recibido, procedemos a compararlo
    Actualizar();

    p = error / 50;

    Distancia();
        
    if (bandera == 0) {
      funcReset();
    }

    //VelBase = VelBase_inicial - abs(error) * 4.2;
    
    if (VerEnPantalla) {
      Serial.print("error: ");
      Serial.print(error);
      Serial.print(" bandera: ");
      Serial.println(bandera);
    }

    if ((digitalRead(SensorI)) or (digitalRead(SensorD)) or (digitalRead(SensorA))) {
      if (digitalRead(SensorA)) {
        if (VerEnPantalla) {
          Serial.println("Inclinado: Adelante");
        }
        if (tiempo == 0) {
          tiempo = millis();
        }
        if ((millis() - tiempo) > 300) {
          VelBase = 20;
        }
      }
      else if (digitalRead(SensorI)) {
        if (VerEnPantalla) {
          Serial.println("Inclinado: Izquierda");
        }
        if (tiempo == 0) {
          tiempo = millis();
        }
        if ((millis() - tiempo) > 300) {
          Vi = 7;
          Vd = 0;
          VelBase = 7;
        }
      }
      else {
        if (VerEnPantalla) {
          Serial.println("Inclinado: Derecha");
        }
        if (tiempo == 0) {
          tiempo = millis();
        }
        if ((millis() - tiempo) > 300) {
          Vi = 0;
          Vd = 7;
          VelBase = 7;
        }
      }
    }
    else {
      if (VerEnPantalla) {
        Serial.println("Plano");
      }
      Vi = 0;
      Vd = 0;
      VelBase = VelBase_inicial;
      tiempo = 0;
    }


    if ((dis < 7) and (not (Brazo_abajo)) and (not (Zona))) {
      Izquierdo(0, 1);
      Derecho(0, 1);
      delay(3000);
      if (not(digitalRead(SensorO))) {
        if (VerEnPantalla) {
          Serial.println("Obstaculo");
        }
        n = 'O';
        Enviar();
        Izquierdo(-15, 1);
        Derecho(-15, 1);
        delay(700);    
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(500);    
        Cam(60);
        delay(300);
      }
    }

    
    

    switch (bandera) {
      case 10:
        seguirlinea(p, 1, Vi, Vd);
      break;
      case 11:
        seguirlinea(p, 1, Vi, Vd);
      break;
      case 20:
        if (VerEnPantalla) {
          Serial.println("Verdes: Atras");
        }
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(500);
        Izquierdo(15, 1);
        Derecho(15, 1);
        delay(1000);
          Izquierdo(-30, 1);
          Derecho(30, 1);
          delay(900);
   
    while (not ((bandera == 10) or (bandera == 11))) {
            Izquierdo(-30, 1);
            Derecho(30, 1);
            Actualizar();
          }
          delay(300);
          Izquierdo(0, 1);
          Derecho(0, 1);
          delay(200);
      break;
      case 21:
        GirarVerde(1); //1 Izquierda
      break;
      case 22:
        //Derecha;
        GirarVerde(-1); //-1 Derecha
      break;
      case 30:
        if (Contador == 1) {
          Contador = -1;
          n = 'D';
          Enviar();
        }
        else {
          Contador = 1;
          n = 'I';
          Enviar();
        }
        Esquivar(Contador);
      break;
      case 60:
      if (dis < 7) {
        if (VerEnPantalla) {
          Serial.println("Pared de Zona");
        }
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(500);
        if (dis < 7) {
          if (not (bandera == 11)) {
            Izquierdo(-15, 1);
            Derecho(-15, 1);
            delay(500);
            Izquierdo(15, 1);
            Derecho(-15, 1);
            delay(700);
          }
          else {
            if (digitalRead(SensorO)) {
              Zona = false;
              n = 'S';
              Enviar();
            }
          }
        }
      }
      else {
      Izquierdo(15, 1);
      Derecho(15, 1);
      delay(700);
      }
    
      break;
      case 32:
        n = 'D';
        Enviar();
        Esquivar(-1); //-1 Derecha
      break;
      case 31: 
        n = 'I';
        Enviar();        
        Esquivar(1); //1 Izquierda
      break;
      case 4:
        if (not ((error < 500) and (error > -500))) {
          kp = 0.6;
          Izquierdo(0, 1);
          Derecho(0, 1);
          delay(100);
          Izquierdo(-15, 1);
          Derecho(-15, 1);
          delay(500);
          Izquierdo(0, 1);
          Derecho(0, 1);
          delay(100);
          Actualizar();
          kp = kp_inicial;
        }
        else {
          tiempo = millis();
          while (((millis() - tiempo) < 3000) and (not (bandera == 10))) {
            seguirlinea(p, 1, Vi, Vd);
            Actualizar();
          }
        }
      break;
      case 6:
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(100);
        Izquierdo(20, 1);
        Derecho(20, 1);
        delay(2000);
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(100);
        n = 'Z';
        Enviar();
        Zona = true;
      break;
      case 70:
        if (Cargadas == 0) {
          if (VerEnPantalla) {
            Serial.println("Veo Plateada, garra vacia");
          }
          Cargar();
          Cargada = true;
          Cargadas += 1;
          Plateada = true;
        }
        else {
          if (Plateada) {
            if (VerEnPantalla) {
              Serial.println("Veo Plateada, tengo Plateada");
            }
            Cargar();
            Cargada = true;
            Cargadas += 1;
            Plateada = true;
          }
        }
        n = 'P';
        Enviar();
      break;
      case 71:
        if (Cargadas == 0) {
          if (VerEnPantalla) {
            Serial.println("Veo Negra, garra vacia");
          }
          Cargar();
          Cargada = true;
          Cargadas += 1;
          Negra = true;
        }
        n = 'P';
        Enviar();
      break;
      case 80:
        if ((Cargadas != 0) and (Plateada)) {
          if (VerEnPantalla) {
            Serial.println("Zona de Evacuacion: Vivos");
          }
          Descargar();
          Cargadas = 0;
          Cargada = false;
          Plateada = false;
          Negra = false;
        }
      break;
      case 81:
        if ((Cargadas != 0) and (Negra)) {
          if (VerEnPantalla) {
            Serial.println("Zona de Evacuacion: Muertos");
          }
          Descargar();
          Cargadas = 0;
          Cargada = false;
          Plateada = false;
          Negra = false;
        }
      break;
      case 9:
        if (VerEnPantalla) {
          Serial.println("Zona: No veo nada");
        }
        tiempo = millis();
        RND = random(1000, 3000);
        while (((millis() - tiempo) < 3000) and (bandera == 9)) {
          Izquierdo(15, 1);
          Derecho(-15, 1);
          Actualizar();
          tiempo = millis();
          while (((millis() - tiempo) < RND) and (bandera == 9) and (dis > 10)) {
            Izquierdo(15, 1);
            Derecho(15, 1);
            Actualizar();
          }
        }
      break;
      case 110:
        if (VerEnPantalla) {
          Serial.println("Fin de pista");
        } 
        Izquierdo(0, 1);
        Derecho(0, 1);
        delay(10000);
      break;
    }
  }
}
