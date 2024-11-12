//MOVER COSAS CON DELICADEZA
void Posicion(int g) { //Brazo
  G = Brazo.read();
  if (G <= g) {
    for (int i = G; i <= g; i++) {
      Brazo.write(i);
      delay(20);
    }
  }
  else {
    for (int i = G; i >= g; i--) {
      Brazo.write(i);
      delay(20);
    }
  }
}

void PCam(int k) { //Camara
  K = Camara.read();
  if (K <= k) {
    for (int i = K; i <= k; i++) {
      Camara.write(i);
      delay(10);
    }
  }
  else {
    for (int i = K; i >= k; i--) {
      Camara.write(i);
      delay(10);
    }
  }
}

void PClas(int k) { //Clasificador
  K = Clasificador.read();
  if (K <= k) {
    for (int i = K; i <= k; i++) {
      Clasificador.write(i);
      delay(10);
    }
  }
  else {
    for (int i = K; i >= k; i--) {
      Clasificador.write(i);
      delay(10);
    }
  }
}

void PPinza(int k) { //Pinza
  K = Pinza.read();
  if (K <= k) {
    for (int i = K; i <= k; i++) {
      Pinza.write(i);
      delay(10);
    }
  }
  else {
    for (int i = K; i >= k; i--) {
      Pinza.write(i);
      delay(10);
    }
  }
}

void PPuertaI(int k) { //Puerta izquierda
  K = PuertaI.read();
  if (K <= k) {
    for (int i = K; i <= k; i++) {
      PuertaI.write(i);
      delay(10);
    }
  }
  else {
    for (int i = K; i >= k; i--) {
      PuertaI.write(i);
      delay(10);
    }
  }
}

void PPuertaD(int k) { //Puerta derecha
  K = PuertaD.read();
  if (K <= k) {
    for (int i = K; i <= k; i++) {
      PuertaD.write(i);
      delay(10);
    }
  }
  else {
    for (int i = K; i >= k; i--) {
      PuertaD.write(i);
      delay(10);
    }
  }
}
//ACA TERMINA
void descargar(char P){  //Si P es I abro la izquierda si P es D abro la derecha
  servoizq(0);
  servoder(0);
  delay(200);
  servoizq(-30);
  servoder(-30);
  delay(1300);
  servoizq(-20);
  servoder(20);
  delay(2300);
  servoizq(-20);
  servoder(-20);
  delay(1500);
  servoizq(0);
  servoder(0);
  delay(200);
  if (P==I){
    PPuertaI(30);
    delay(2000);
    PPuertaI(90);
    }
  if (P==D){
    PPuertaD(120);
    delay(2000);
    PPuertaD(90);
    }  
}

bool obstaculo(){
  return 500>analogRead(A2);
}

void Recoleccion(int C){
   Posicion(145);
  servoizq(10);
  servoder(10);
  PPinza(80);

  Posicion(150);

Pinza.write(150);
servoizq(0);
  servoder(0); 
Posicion(20);
PClas(C);
  PPinza(0);
  
}

void Recibir(int howMany) {
  while (Wire.available() > 0) { //Mientras tengamos caracteres en el buffer
    char inChar = (char)Wire.read();
    if (inChar == '\n') {
      banderaComplete = true;
    }
    else if (inChar == '&')
      errorComplete = true;
    else if (!(errorComplete)) {
      errorString += inChar;
    }
    else {
      banderaString += inChar;
    }
  }
}

void Enviar() {
  Wire.write(n);
}

void servoizq(int v) {
  DI.write(90 + v);
  TI.write(90 + v);
}
void servoder(int v) {
  DD.write(90 - v);
  TD.write(90 - v);
}

int V;
void Esquivar (int tiempo, String donde) {
  n = 'O'; //Cambio n por O si hay obstaculo y por T al regresar
  Enviar();

  V = (tiempo / 10);
  if (donde=="derecha") {
    DD.write(90 - V);
    DI.write(90 - V);
    TD.write(90 - V);
    TI.write(90 - V);
  
    delay(tiempo);
  
    DD.write(90 - V);
    DI.write(90 + V);
    TD.write(90 - V);
    TI.write(90 + V);
  
    delay((3 * tiempo));
  
    DD.write(90 + V);
    DI.write(90 + V);
    TD.write(90 + V);
    TI.write(90 + V);
  
    delay((1.2 * tiempo));
  
    DD.write(90 - V);
    DI.write(90 + V);
    TD.write(90 - V);
    TI.write(90 + V);
  
    delay((2.8 * tiempo));
  
    DD.write(90 + V);
    DI.write(90 + V);
    TD.write(90 + V);
    TI.write(90 + V);
  
    delay(tiempo);
  
    DD.write(90 - V);
    DI.write(90 + V);
    TD.write(90 - V);
    TI.write(90 + V);
  
    delay((1.5 * tiempo));
  
    DD.write(90); //Aca frena pero debe seguir siguiendo ¿? linea
    DI.write(90);
    TD.write(90);
    TI.write(90);
    delay(200);
    n = 'T'; //Cambio n por O si hay obstaculo y por T al regresar
    Enviar();
  
    delay(tiempo);
  }
  if (donde=="derecha"){
  // Por ahora no hay programa, una pena
  }
  if (donde=="medio"){
    // Tampoco hay implementacion y probablemente pueda hacerse con uno de los otros dos
  }
}


float dis(){
  long t;
  long d; 
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger,LOW);
  t = pulseIn(Echo, HIGH);
  d = t/59;
  return d;
}

void seguirlinea(int p) {
  i = i + p;
  d = p - p_old;
  p_old = p;
  if ((p * i) < 0) i = 0; //Corrige el overshooting - integral windup
  u = kp * p + ki * i + kd * d; //Define la corrección
  //Controlar los servomotores de rotación continua
  int VelIzquierda = constrain((inicial + u),  -90, velmax); //Define la velocidad de los motores como la velocidad inicial + la corrección y las acota entre la velmax y mínima.
  int VelDerecha = constrain((inicial - u),  -90, velmax);
  //Serial.print(VelIzquierda);
  //Serial.print("....");
  //Serial.println(VelDerecha);
  //delay(100);
  servoizq(VelIzquierda);
  servoder(VelDerecha);
}

/*Funcion para verificar la bandera cada vez para asegurarnos
de actuar apropiadamente
*/

void VerBandera(){ //Funcion para verificar la bandera dentro de cada if para no quedar trancado haciendo algo incorrecto
  if (banderaComplete) { //El comando fue recibido, procedemos a compararlo  
    errorString.remove(0, 1); //Removemos 1 caracter. El caracter con index 0, caracter adicional sin utilidad que esta mandando Raspberry Pi
    error = errorString.toInt();      
    bandera = banderaString.toInt();
  
    banderaString = "";
    banderaComplete = false;
    errorString = "";//Limpiamos la cadena para poder recibir el siguiente comando
    errorComplete = false; //Bajamos la bandera para no volver a ingresar a la comparación hasta que recibamos un nuevo comando
  }
}
