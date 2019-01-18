#define led PC13
#define cs '>'
#define re_de PA1
#define relay_slave_ctrl PB9

// leds para visualizar
#define arriba PB7
#define der PB8
#define izq PB5
#define abajo PB6

int c_s = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  Serial1.begin(115200);
  Serial1.setTimeout(100);

  // For blink led or anything else
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  pinMode(re_de, OUTPUT);
  digitalWrite(re_de, 0);

  // configuracion de los pinos
  pinMode(arriba, OUTPUT);
  pinMode(der, OUTPUT);
  pinMode(izq, OUTPUT);
  pinMode(abajo, OUTPUT);
  all_off(0);


  pinMode(relay_slave_ctrl, OUTPUT);
  digitalWrite(relay_slave_ctrl, 0);

  blink_led(300);

}// end Setup

/************************ Loooooppppp *********************/
void loop() {
  read_input_data();
  delay(10);
}

/************************************************************************/
/****************** Leer y pasar datos ***********************/
/************************************************************************/
void read_input_data(){
  if(Serial.available() != 0){
    String data_in = Serial.readStringUntil(cs);
    int sel_action = data_in.toInt();
    data_in = Serial.readStringUntil(cs);
    int direction_m = data_in.toInt();
    data_in = Serial.readStringUntil(cs);
    int pwm_mov = data_in.toInt();
    data_in = Serial.readStringUntil(cs);
    int tempo_mov = data_in.toInt();

    Serial.readString();

    switch (sel_action) {
      case 1:
        Serial.println("1 opcion: Mover Carro");
        mover_carro(sel_action, direction_m, pwm_mov, tempo_mov);
        break;
      case 2:
        Serial.println("2 opcion: Mover Camara");
        mover_camera(sel_action, direction_m, pwm_mov, tempo_mov);
        break;
      case 3: // responde
        Serial.println("3 opcion: Mover Sensor");
        //mover_sensor(sel_action, pwm_mov, tempo_mov);
        break;
      case 4: // responde
        // Encender slave
        if(direction_m == 1){
          digitalWrite(relay_slave_ctrl, 1);
          all_off(1);
          Serial.println("Intentando Power On - Relay");
          // Compueba conexion
          delay(500);
          comprobar_conexion_inicial();

          blink_led(200);
        }else if(direction_m == 0){
          digitalWrite(relay_slave_ctrl, 0);
          all_off(0);
          Serial.println("Power Off");
        }
        break;
      case 5:
          digitalWrite(relay_slave_ctrl, 1);
          Serial.println("Teste Ok");
          blink_all_off(10, 200);
          break;

    }// End Switch 
      
      Serial.println("Send a new commandd...");
      Serial.println("O");
      blink_led(150);
  }// End Serial_1 available 
}// End function read input data serial



/************************************************************************/
/****************** manda datos para slave con valores ******************/
/************************************************************************/
void mover_carro(int sa, int dir, int pwm_m, int time_m){
  switch (dir) {
    case 1:
      digitalWrite(arriba, 1);
      break;
    case 2:
      digitalWrite(der, 1);
      break;
    case 3:
      digitalWrite(izq, 1);
      break;
    case 4:
      digitalWrite(abajo, 1);
      break;
  }
  
  Serial.print("mover caro >>>>  ");
  String msg = concat_msg(sa, dir, pwm_m, time_m);
  Serial.print("Mensaje a enviar: ");
  Serial.println(msg);
  serial_print(msg);
  read_responce();
  all_off(0);
}

/************************************************************************/
/***************** Fx camera ****************/
/************************************************************************/
void mover_camera(int sa,int sel_motor, int sb_ang1, int src_ang2){
  Serial.print("mover camera >>>>  ");
  String msg = concat_msg(sa, sel_motor, sb_ang, src_ang);
  Serial.print("Mensaje a enviar: ");
  Serial.println(msg);
  serial_print(msg);
  read_responce();
  all_off(0);
}



void mover_sensor(int sa, int time_sm, int pwm_sm){
  Serial.println("mover sensor");
  blink_led(200);
  delay(time_sm);
}


/************************************************************************/
/************* Fx enviar dato ************************/
/************************************************************************/
void serial_print(String str_out){
  digitalWrite(re_de, 1);
  str_out = str_out;
  Serial1.println(str_out);
  Serial1.flush();
  delay(10);
  digitalWrite(re_de, 0);
}

/************************************************************************/
/************* Fx para recibir respuesta de entrega de dato ****************/
/************************************************************************/
void read_responce(){
  unsigned long t_finr = micros() + 2e6;
  bool exit_read_responce = true;

  while (exit_read_responce){
    if (micros() < t_finr){
      if(Serial1.available() != 0){
        String data_inr = Serial1.readStringUntil(cs);
        String data_in_string = Serial1.readStringUntil(cs);
        if(data_inr == "O"){
          Serial.println("O");
          Serial.print("Retorno: ");
          Serial.println(data_in_string);
          exit_read_responce = false;
          t_finr = micros();
        }
      }
    }else{
      Serial.println("F");
      exit_read_responce = false;
    }
  }// end while config ok!
}// end fx read_responce()


/************************************************************************/
/*************** Funcion para concatenar mensaje *************/
/************************************************************************/
String concat_msg(int sa, int dir, int pwm_m, int time_m){
  String msg_send = String(sa) + cs + String(dir) + cs + String(pwm_m);
  msg_send = msg_send + cs + String(time_m) + cs;
  return msg_send;
}


/************************************************************************/
/***************** Fx blincar led **********************/
/************************************************************************/
void blink_led(int time_b){
  digitalWrite(led, 0);
  delay(time_b);
  digitalWrite(led, 1);
  delay(time_b);
  digitalWrite(led, 0);
  delay(time_b);
  digitalWrite(led, 1);
}


void comprobar_conexion_inicial(){
  bool exit_read = true;
  bool exit_read_fail = true;

  unsigned long t_fin = micros() + 5e6;
  digitalWrite(re_de, 0);

  while (exit_read){
    if (micros() < t_fin){
      if(Serial1.available() != 0){
        String data_in1 = Serial1.readStringUntil(cs);
        int ini_ok = data_in1.toInt();
        if(ini_ok == 9){
          Serial.println("Configuracion OK!!!");
          Serial.println("K");
          exit_read = false;
          exit_read_fail = false;
        }
      }
    }else{
      Serial.println("Falha na Configuracion!!!");
      Serial.println("F1");
      exit_read = false;
      blink_all_off(10, 150);
    }
  }// end while config ok!

  while(exit_read_fail){
    blink_led(250);
    Serial.println("Falha na Configuracion!!!");
    exit_read_fail = false;
  }
}

void all_off(boolean on_off){
  if(on_off){
    digitalWrite(arriba, 1);
    digitalWrite(der, 1);
    digitalWrite(izq, 1);
    digitalWrite(abajo, 1);
  }else{
    digitalWrite(arriba, 0);
    digitalWrite(der, 0);
    digitalWrite(izq, 0);
    digitalWrite(abajo, 0);
  }
}

void blink_all_off(int for_b, int time_bao){
  for(int i=0; i<for_b; i++){
    all_off(1);
    delay(time_bao);
    all_off(0);
    delay(time_bao);
  }
}