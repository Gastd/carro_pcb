#include <Wire.h>
#include <SoftwareSerial.h>

#define RxD 2
#define TxD 3

#define IN1_5 5
#define IN2_6 6
#define IN3_7 7
#define IN4_8 8

#define ENA 10
#define ENB 11


SoftwareSerial BTSerial(RxD, TxD);
char command = '\0';

void setup()
{
    pinMode(IN1_5, OUTPUT);
    pinMode(IN2_6, OUTPUT);
    pinMode(IN3_7, OUTPUT);
    pinMode(IN4_8, OUTPUT);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);

    Serial.begin(9600);

    BTSerial.begin(115200);
    BTSerial.flush();
}

void loop()
{
    bool ready = false;
    if(BTSerial.available()){
        while(BTSerial.available()){
            command = BTSerial.read();
        }
        ready = true;
    }
    // int c = ;
    if(ready)
    {
        // c = Serial.read();
        Serial.println(command);
        switch (command)
        {
            case 'F':
                Serial.println("Indo para frente");
                BTSerial.println("Indo para frente");
                frente(4e3);
                break;
            case 'A':
                Serial.println("Indo para tras");
                BTSerial.println("Indo para tras");
                atras(4e3);
                break;
            case 'E':
                Serial.println("Indo para esquerda");
                BTSerial.println("Indo para esquerda");
                esquerda(4e3);
                break;
            case 'D':
                Serial.println("Indo para direita");
                BTSerial.println("Indo para direita");
                direita(4e3);
                break;
            default:
                command = '\0';
                break;
        }
    }
}

void frente(int time)
{
    analogWrite(ENA, 200);
    analogWrite(ENB, 200);

    digitalWrite(IN1_5, HIGH);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, HIGH);
    digitalWrite(IN4_8, LOW);

    delay(time);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void atras(int time)
{
    analogWrite(ENA, 200);
    analogWrite(ENB, 200);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, HIGH);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, HIGH);

    delay(time);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void esquerda(int time)
{
    analogWrite(ENA, 200);
    analogWrite(ENB, 200);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, HIGH);
    digitalWrite(IN3_7, HIGH);
    digitalWrite(IN4_8, LOW);

    delay(time);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void direita(int time)
{
    analogWrite(ENA, 200);
    analogWrite(ENB, 200);

    digitalWrite(IN1_5, HIGH);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, HIGH);

    delay(time);

    digitalWrite(IN1_5, LOW);
    digitalWrite(IN2_6, LOW);
    digitalWrite(IN3_7, LOW);
    digitalWrite(IN4_8, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}
