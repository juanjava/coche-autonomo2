#include <NewPing.h>
#include <Servo.h>
///declaraciones de funciones////
void MoverCoche();
void MirarSiChoco();
void MedirTodosLados();
int TomarMedida(NewPing sonar, int indice);
void CentrarCoche();
void AtrasGiro();
void MoverHaciaAdelante();
void MoverHaciaAtras();
void DejarCocheParado();
void GirarDerecha(int sumatorio1);
void GirarIzquierda(int sumatorio1);
void CalcularAngulo(int sumatorio1);
void CentrarDireccion();
void Esquivar(byte Angulo);
int ControlTraccion();
////ya lo llevamos bien no??//

//// esto para los mensajes de consola/////
String lado[] = {"Derecha ", " Delante izquierda ", "Delante derecha " , "Izquierda ", "Atras "};

////////////////variables////////////////////////
boolean ChoqueDel = false;
boolean ChoqueTras = false;
boolean ObstaculoDer = false;
boolean ObstaculoIzq = false;
int Angulo = 0;
byte Angulogiro = 45; /////defino Angulogiro aqui por el tema de los limites del mecanismo de giro del propio coche

/////MEDIDAS: las distancias en cm///
int izquierda = 0;
int derecha = 0;
int detras = 0;
int delanteder = 0;
int delanteizq = 0;

//////esto se puede ajustar segun corra el coche
byte margen = 10;///margen lateral
byte MargenGiro = 60;////aqui giramos
byte choque = 36;////aqui no nos da tiempo de girar ya

//////////////////los pines del arduino//////////////////////
////servo aqui
int PinServo = A0;///anda de lujo, aunque no te lo creas
int PulsoMin = 600;////ajustes del servo
int PulsoMax = 2400;
//////////////////////pines////////////////////////
/////delanteros
byte PingFaroder = 13;
byte EchoFaroder = 12;
byte PingFaroizq = 11;
byte EchoFaroizq = 10;
////modulo motores
byte in4 = 9;
byte in3 = 8;
////lados
byte EchoIzq = 7;
byte pingizq = 6;
byte EchoDer = 5;
byte pingder = 4;
///detras
/*byte pingdet = 3; // Arduino pin tied to trigger pin on the ultrasonic sensor.
  byte EchoDet = 2; // Arduino pin tied to echo pin on the ultrasonic sensor.
*/
byte pingdet = A1; // Arduino pin tied to trigger pin on the ultrasonic sensor.
byte EchoDet = A2; // Arduino pin tied to echo pin on the ultrasonic sensor.
byte pintraccion = 3;

#define MAX_DISTANCE 380 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonarfaroder(PingFaroder, EchoFaroder, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarfaroizq(PingFaroizq, EchoFaroizq, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarder(pingder, EchoDer, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarizq(pingizq, EchoIzq, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonardetras(pingdet, EchoDet, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
Servo servo1;

void setup() {
  Serial.begin(9600); // abrimos consola para ver qe hace el arduino.
  /////modulo motores
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(pintraccion, OUTPUT);
  pinMode(PinServo, OUTPUT);//A0 recuerda
  servo1.attach(PinServo, PulsoMin, PulsoMax);
}

void loop() {/////////////////loop///////loop//////////////loop/////////////////////////////////
  MedirTodosLados();
  while (delanteizq > MargenGiro && delanteder > MargenGiro) {
    ///mientras tenga campo libre tiro palante
    MoverHaciaAdelante();//funciona moviemiento pura y dura//
    CentrarCoche();
    MedirTodosLados();
  }
  /////////////ya no tengo campo libre/////////
  MoverCoche();
}

void MoverCoche() {
  ///delante los faros: se me acerca un obstaculo
  ChoqueDel = false;//vamos a mirar si chocamos
  ChoqueTras = false;
  ObstaculoDer = false;
  ObstaculoIzq = false;

  if (delanteder < MargenGiro) {  ///dos faros/sensores recuerda
    ObstaculoDer = true;
  }
  if (delanteizq < MargenGiro) {
    ObstaculoIzq = true;
  }
  if (ObstaculoDer || ObstaculoIzq) { ///uno U otro
    MirarSiChoco();////////comprobaciones varias
  }
  ///vengo del MirarSiChoco();
  
  ////////aqui la toma de decisiones.
  if (ChoqueDel == false) { ///no choco, solo tengo un obstaculo; puedo tirar alante

    if (ObstaculoDer && ObstaculoIzq) {///obstaculo en los dos lados: atras
      AtrasGiro();
      MoverHaciaAtras();
    } else if (ObstaculoDer) {///obstaculo en un lado solo
      MoverHaciaAdelante();
      Esquivar(90 - Angulogiro);
      Serial.println("Obstaculo derecha");
    } else if (ObstaculoIzq) {///obstaculo en un lado solo
      MoverHaciaAdelante();
      Esquivar(90 + Angulogiro);
      Serial.println("Obstaculo izquierda");
    } else {///si no hay obstaculos centro el coche;
      MoverHaciaAdelante();
      CentrarCoche();///redundante, lo se
    }
  } else { ///ChoqueDel es true
    if (ChoqueTras == true) {
      ///aqui no me puedo mover
      DejarCocheParado();
      CentrarDireccion();
      Serial.println("Quieto parao");
    } else {
      Serial.println("He chocado delante----atras voy");
      MoverHaciaAtras();
      AtrasGiro();
    }
  }
}
void MirarSiChoco() { ///miro si choco delante o detras

  //choque = 35;////aqui no nos da tiempo de girar ya
  /////miro si estoy chocando////o voy a hacerlo
  if (delanteder < choque && delanteder > 0) {
    ChoqueDel = true;
  } else if (delanteizq < choque && delanteizq > 0) {
    ChoqueDel = true;
  } else {
    ChoqueDel = false;
  }
  ////miro si choco atras///
  if (detras < choque && detras > 0) {/////////esto es una prueba
    ChoqueTras = true;
  }
}
void MedirTodosLados() {
  ////String lado[] = {" derecha", " delante izquierda "," delante derecha " , " izquierda", " atras "};
  ///orden asi para que no se interfieran, o eso intento
  izquierda = TomarMedida(sonarizq, 3);
  delanteizq = TomarMedida(sonarfaroizq, 1);
  delay(20);///estabilidad, rebotes de la señal en las paredes etc
  delanteder = TomarMedida(sonarfaroder, 2);
  derecha = TomarMedida(sonarder, 0);
  detras = TomarMedida(sonardetras, 4);
  ///Serial.println();
}
/*
  void medirdelante() {
  }
  void medirlados() {

  }*/

int TomarMedida(NewPing sonar, int indice) {
  int distancia = 0;
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print(lado[indice]);
  distancia = uS / US_ROUNDTRIP_CM;
  ///if (distancia > MAX_DISTANCE || distancia < 0) {
  if (distancia < 0) { //veremos a campo abierto que hace
    distancia = 0;
  }
  Serial.print(distancia);
  Serial.print(" cm ");
  return distancia;
}

void CentrarCoche() {
  int sumatorio = derecha - izquierda;
  if (sumatorio > 0 && sumatorio > margen) {////////derecha es mas grande
    GirarDerecha(sumatorio);
  } else if (sumatorio < 0 && sumatorio < (0 - margen)) {///izquierda es mas grande, da negativo
    GirarIzquierda(sumatorio);
  } else {
    CentrarDireccion();
  }
  /*if (derecha > izquierda && ((derecha - izquierda) > margen)) {
    GirarDerecha();
    } else if (izquierda > derecha && ((izquierda - derecha) > margen)) {
    GirarIzquierda();
    } else {
    CentrarDireccion();
    }*/
}
void AtrasGiro() { /// para cuando vaya hacia atras gire hacia area libre
  int sumatorio = derecha - izquierda;
  if (sumatorio > 0 && sumatorio > margen) {////////derecha es mas grande
    Esquivar(90 - Angulogiro);
  } else if (sumatorio < 0 && sumatorio < (0 - margen)) {///izquierda es mas grande, da negativo
    Esquivar(90 + Angulogiro);
  } else {
    CentrarDireccion();
  }
}
/*///autoguia    
 * } else if (ObstaculoDer) {///obstaculo en un lado solo
      MoverHaciaAdelante();
      Esquivar(90 - Angulogiro);
      Serial.println("Obstaculo derecha");
    } else if (ObstaculoIzq) {///obstaculo en un lado solo
      MoverHaciaAdelante();
      Esquivar(90 + Angulogiro);
      Serial.println("Obstaculo izquierda");
 */

//////////////////actuo sobre los pines aqui////////////////////////////
void MoverHaciaAdelante() {
  analogWrite(pintraccion, ControlTraccion());
  Serial.print("Adelante");
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void MoverHaciaAtras() {
  int aux = detras * 2;
  if (aux > 255) {
    aux = 255;
  }
  analogWrite(pintraccion, aux);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  Serial.println("Atras");
}
void DejarCocheParado() {
  //digitalWrite(ena, LOW);///ena es motor principal de movimiento
  analogWrite(pintraccion, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  Serial.println("paro");
}

//////////con servo//// estas funciones centran el coche,
////no tienen en cuenta los dos sensores delanteros
void GirarDerecha(int sumatorio1) {
  CalcularAngulo(sumatorio1);
  servo1.write(90 + Angulo); ///90 es centro,
  Serial.print("derecha ");
  Serial.println(Angulo);
}
void GirarIzquierda(int sumatorio1) {
  CalcularAngulo(sumatorio1);
  servo1.write(90 - Angulo); ///90 es centro,
  Serial.print("izquierda ");
  Serial.println(Angulo);
}
void CalcularAngulo(int sumatorio1) {
  Angulo = abs(sumatorio1);//abs()[Math] Description  Calculates the absolute value of a number.
  Angulo = Angulo / 2; ////asi deberia girar mas suave
  if (Angulo > Angulogiro) {   ///Angulogiro = 45
    Angulo = Angulogiro;
  }
}
void CentrarDireccion() {
  Serial.println("centro");
  servo1.write(90);
}
void Esquivar(byte Angulo) {
  servo1.write(Angulo);
  Serial.print("Esquivando ");
  Serial.println(Angulo);
}
int ControlTraccion() {////para limitarle un poco la potencia a los motores y no vaya tan loco
  int media = ( delanteder + delanteizq);// / 2;
  if (media > 255) {
    media = 255;
  }
  return media;
}
