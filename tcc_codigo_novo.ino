#include <Servo.h>
#include<Stepper.h>

#define servo 52   //servo(garra)

#define chaveOnOff 53    // chave on/off 
#define fimDeCursoVertical 23   // fim de curso vertical
#define fimDeCursoBraco 22   // fim de curso horizontal
#define led 50   // led on/off

#define s0 48 //pinos do tcs3200(sensor de cor)
#define s1 49
#define s2 46
#define s3 47
#define out 45
#define led_tcs 44

#define passo_m2 35 //pinos do driver A4988(motor vertical)
#define dir 34
#define ms1 39 
#define ms2 37
#define ms3 36
#define en 38

#define baixo LOW //sentido do motor vertical
#define cima HIGH

#define esquerda -1 //sentido do motor do braço
#define direita 1

int passoPorVolta = 32; //numero de passos por volta do motor do braço

unsigned int vermelho = 0; //filtros tcs3200(sensor de cor)
unsigned int verde = 0;
unsigned int azul = 0;
unsigned int semfiltro = 0;

Servo garra; //servo (garra)

Stepper braco(passoPorVolta, 8, 10, 9, 11); //configuração de pinos do motor do braço(28byj-48)

int posicaoInicial; //variavel para saber se a maquina esta na poisção inicial
#define sim 0
#define nao 1


void setup ()
{
  
Serial.begin(9600);

pinMode(chaveOnOff , INPUT); 
pinMode(fimDeCursoVertical, INPUT);
pinMode(fimDeCursoBraco , INPUT);
pinMode(led, OUTPUT);

pinMode(s0 , OUTPUT);
pinMode(s1 , OUTPUT);
pinMode(s2 , OUTPUT);
pinMode(s3 , OUTPUT);
pinMode(led_tcs , OUTPUT);
pinMode(out , INPUT);

pinMode(passo_m2 , OUTPUT);
pinMode(dir , OUTPUT);
pinMode(ms1 , OUTPUT);
pinMode(ms2 , OUTPUT);
pinMode(ms3 , OUTPUT);
pinMode(en , OUTPUT);

braco.setSpeed(1250);

garra.attach(servo);
garra.write(90);

digitalWrite(ms1 , LOW);      
digitalWrite(ms2 , HIGH);  
digitalWrite(ms3 , LOW);
digitalWrite(en , HIGH);  

digitalWrite(led_tcs , LOW);
digitalWrite(s0, HIGH);
digitalWrite(s1, LOW);

posicaoInicial = nao;

}

void loop()
{ 

  if(digitalRead(chaveOnOff) == LOW)  // QUANDO DESLIGADO
  {
    
    digitalWrite(led , LOW); 

    posicaoInicial = nao;

    garra.write(90);

    digitalWrite(led_tcs, LOW);

    digitalWrite(en, HIGH);

  }//if(digitalRead(chaveOnOff) == LOW)
  
  
  if(digitalRead(chaveOnOff) == HIGH)  //QUANDO LIGADO
  {
  
    digitalWrite(led , HIGH);
    
//////////////////////////////////////////////////////////////////////// COLOCAR A MÁQUINA NA POSIÇÃO INICIAL /////////////////////////////////////////////////////////////////////////////

    if(posicaoInicial == nao) 
    {
      
      while (true)
      {
        
        motor_braco(1, esquerda);
      
        if(digitalRead(fimDeCursoBraco) == HIGH)
        {
          
          break;
        
        }//if(digitalRead(fimDeCursoBraco) == HIGH)
      }//while tru
        
      while (true)
      {
          
        motor_vertical(1, baixo);
        
        if(digitalRead(fimDeCursoVertical) == HIGH)
        {

          break;
          
        }// if(digitalRead(fimDeCursoVertical) == HIGH)
      }//while ture
    
    motor_vertical(10000, cima);
    digitalWrite(en, HIGH);
    motor_braco(2000, direita); 
    posicaoInicial = sim;

    }// if (posInicial == nao)

//////////////////////////////////////////////////////////////////////// LEITURA E SEPARAÇÃO DAS PEÇAS POR COR /////////////////////////////////////////////////////////////////////////////    

    if( posicaoInicial == sim)
    {
    
      digitalWrite(led_tcs, HIGH);

      cor();
      Serial.print("Vermelho :");
      Serial.print(vermelho);

      Serial.print(" Verde : ");
      Serial.print(verde);

      Serial.print(" Azul : ");
      Serial.print(azul);

      Serial.print(" Branco : ");
      Serial.print(semfiltro);
      Serial.println();
        
      if((vermelho < azul) && (vermelho < verde) && (semfiltro < 100)) //se a peça for vermelha
      {
          
        Serial.println("Vermelho");
        motor_braco(500, direita);
        motor_vertical(8000, baixo);
        garra.write(180);
        motor_vertical(8000, cima);
        motor_braco(1900, esquerda);
        motor_vertical(10000, baixo);
        garra.write(90);
        motor_vertical(10000, cima);
        motor_braco(1400, direita);
           
      }// if((vermelho < azul) && (vermelho < verde) && (sem filtro < 100))

      if((azul < vermelho) && (azul < verde) && (semfiltro < 100)) //se a peça for azul
      {
          
        Serial.println("Azul");
        motor_braco(500, direita);
        motor_vertical(8000, baixo);
        garra.write(180);
        motor_vertical(8000, cima);
        motor_braco(1200, esquerda);
        motor_vertical(9000, baixo);
        garra.write(90);
        motor_vertical(9000, cima);
        motor_braco(700, direita);
                
      }// if((azul < vermelho) && (azul < verde) && (semfiltro < 100))

      if((verde < azul) && (verde < vermelho) && (semfiltro < 100)) //se a peça for verde
      {

        Serial.println("Verde");
        motor_braco(500, direita);
        motor_vertical(8000, baixo);
        garra.write(180);
        motor_vertical(8000, cima);
        motor_braco(500, esquerda);
        motor_vertical(7000, baixo);
        garra.write(90);
        motor_vertical(7000, cima);
        
      }// if((verde < azul) && (verde < vermelho) && (semfiltro < 100)) 
    }//if(posInicial == sim)
  }//if(digitalRead(chaveOnOff == HIGH)
}//void loop()

//////////////////////////////////////////////////////////////////////// FUNÇÃO PARA DETECTAÇÃO DAS CORES /////////////////////////////////////////////////////////////////////////////////

void cor() 
{
  
  //Vermelho
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  vermelho = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Sem filtro
  digitalWrite(s2, HIGH);
  semfiltro = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Azul
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  azul = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);

  //Verde
  digitalWrite(s2, HIGH);
  verde = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  
 }
 
//////////////////////////////////////////////////////////////////////// FUNÇÃO PARA MOVIMENTAÇÃO VERTICAL ////////////////////////////////////////////////////////////////////////////////

 void motor_vertical(int passo, bool direcao) //função para acionar o motor vertical 
 {

  for(int i = 0; i >= passo; i++)
  {

    digitalWrite(dir , direcao);
    digitalWrite(en , LOW);

    digitalWrite(passo_m2 , HIGH);
    delayMicroseconds(500);
    digitalWrite(passo_m2 , LOW);
    delayMicroseconds(500);
    
  }
}

//////////////////////////////////////////////////////////////////////// FUNÇÃO PARA MOVIMENTAÇÃO DO BRAÇO ////////////////////////////////////////////////////////////////////////////////


 void motor_braco(int passo, int direcao) //função para acionar o motor vertical 
 {

  passo = passo*direcao;

  for(int i = 0; i >= passo; i++)
  {

    braco.step(passo);
    
  }
}
