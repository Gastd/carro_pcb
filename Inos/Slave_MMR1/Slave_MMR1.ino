#include <Servo.h>

#define led PC13      // Para hacer blink y testar codigos
#define cs '>'        // Caracter separador mensaje
#define re_de PA1     // Pin de activscion RS485 

//***************** Controle carro ***************//
///-----
#define ENA PA8         // ENABLE MOTOR IZQ
#define ENB PB6        // ENABLE MOTOR DER

///----- MOTOR der
#define IN1 PB15
#define IN2 PB10
///----- MOTOR izq
#define IN3 PB11
#define IN4 PB5

//************ Controle camera *********************//
#define sb PB7            // SERVO BASE
#define src PB8           // SERVO ROTACION CAMARA
//#define sa PB9            // SERVO AUXILIAR "SENSOR DE PROXIMIDADE"

// pwm zero servos
#define zero_sb 98   // zero servo base -- 10 -120
#define zero_src 84  // zero servo -- rango de valores 25-180
//SB --- min25 - max180 
#define min_sb 25   
#define max_sb 180
//SRC ---- min10 - max120
#define min_src 10
#define max_src 120


// Variables temporales
int pwm_c = 0;
int tempo = 0;
int con = 0;

bool value_led = true;

//********* Inicilizar objetos tipo servo *********//
Servo s_base;
Servo s_rot_camera;
//Servo s_aux;


void setup() {
  //*************- Inicializa comunicacion ********************/
  // porta serial para comunicacion rs485
  Serial1.begin(115200);
  Serial1.setTimeout(100);
  //Serial.begin(115200);
  //Serial.setTimeout(100);

  //******** Servos asignacion de pines *************/
  s_base.attach(sb); // base 
  s_rot_camera.attach(src); //rota camera
  //s_aux.attach(sa); // auxiliar

  s_base.write(zero_sb);
  s_rot_camera.write(zero_src);
  //s_aux.write(0);

  // For blink led or anything else
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // for RS485 activation
  pinMode(re_de, OUTPUT);
  digitalWrite(re_de, 0);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(ENA, 0);
  digitalWrite(ENB, 0);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);

  // Start blink
  blink_led(500);

  // Envio un 9 para decir que la comunicacion está funcionando
  serial_print_cs(String(9), 1);

  delay(100);

}

void loop() {
  read_input_data();
  delay(10);
}

/***** Funcion de lectura de parametros *****/
void read_input_data(){
  if(Serial1.available() != 0){
    String data_in = Serial1.readStringUntil(cs);
    int sel_action = data_in.toInt();
    data_in = Serial1.readStringUntil(cs);
    int direction_m = data_in.toInt();
    data_in = Serial1.readStringUntil(cs);
    int pwm_mov = data_in.toInt();
    data_in = Serial1.readStringUntil(cs);
    int tempo_mov = data_in.toInt();

    switch (sel_action){
      case 1:
        mover_carro(direction_m, pwm_mov, tempo_mov);
        break;
      case 2:
        // angulo *** Diferentes limites
        mover_camera(direction_m, pwm_mov, tempo_mov); 
        //blink_led(80);
        break;
      case 3: // responde
        blink_led(300);
        mover_sensor(pwm_mov, tempo_mov);
        break;
      case 4: // responde para inicio de los comando
        blink_led(tempo_mov);
        serial_print_cs("500", 0);
        //delay(2000);
        blink_led(tempo_mov);
        String msn_c = String(con);
        serial_print_cs(msn_c, 0);
        con += 10;
        break;
      }// End Switch
   }// End Serial1 available 
}// End function read input data serial


/*************************************************************************/
/***************** Mover carro -- funciones -- ***************************/
void mover_carro(int dir, int pwm_m, int time_m){
  switch (dir) {
    case 0:
        para_tudo(0);
        break;
    case 1:
        frente(pwm_m, time_m);
        break;
    case 2:
        atras(pwm_m, time_m);
        break;
    case 3:
        esquerda(pwm_m, time_m);
        break;
    case 4:
        direita(pwm_m, time_m);
        break;
    }// End Switch
}

void frente(int pwm_c, int tempo) {
  pwm_control(pwm_c);
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
  blink_led(int(tempo/3));
  para_tudo(0);
  serial_print_cs("O", 0);
  serial_print_cs("Movendo para frente", 0);
}

void atras(int pwm_c, int tempo) {
  pwm_control(pwm_c);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
  blink_led(int(tempo/3));
  para_tudo(0);
  serial_print_cs("O", 0);
  serial_print_cs("Movendo para atras", 0);
}

void esquerda(int pwm_c, int tempo) {
  pwm_control(pwm_c);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  blink_led(int(tempo/3));
  para_tudo(0);
  serial_print_cs("O", 0);
  serial_print_cs("Movendo para esquerda", 0);
}

void direita(int pwm_c, int tempo) {
  pwm_control(pwm_c);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  blink_led(tempo/3);
  para_tudo(0);
  serial_print_cs("O", 0);
  serial_print_cs("Movendo para direita", 0);
}

void para_tudo(int pwm_c) {
  pwm_control(pwm_c);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void pwm_control(int pwm) {
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

/************* Mover camara ******************/
void mover_camera(int sel_motor, int ang_sb, int ang_src){
  if(ang_sb >= min_sb && ang_sb <= max_sb){ //min10 -max120
    s_base.write(ang_sb);
  }
  if(ang_src >= min_src && ang_src <= max_src){ //min25- max180
    s_rot_camera.write(ang_src);
  }
}

/************* Mover sensor de proximidade *****************/
void mover_sensor(int time_sm, int pwm_sm){
    digitalWrite(led, !value_led);
    delay(time_sm);
}

/*********** Comunicacion mensaje de respuesta *************/
void serial_print_cs(String str_out, boolean ini_s){
  if(ini_s){
    for(int i=0; i<5; i++){
      digitalWrite(re_de, 1);
      str_out = str_out + cs;
      Serial1.print(str_out);
      Serial1.flush();
      delay(10);
      digitalWrite(re_de, 0);
      delay(500);
    }
  }else{
    digitalWrite(re_de, 1);
    str_out = str_out + cs;
    Serial1.print(str_out);
    Serial1.flush();
    delay(10);
    digitalWrite(re_de, 0);
  }
}

/************** Blink led PC13 *******************/
void blink_led(int time_b){
  digitalWrite(led, 0);
  delay(time_b);
  digitalWrite(led, 1);
  delay(time_b);
  digitalWrite(led, 0);
  delay(time_b);
  digitalWrite(led, 1);
}