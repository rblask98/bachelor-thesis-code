#include <dht.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);
dht DTH;
//POCETNA KALIBRACIJA => prvo postaviti senzor bez posude ili posudom koja nije pokrivena niti puna = odmakL(min vrijednost) |||| Zatim postaviti s posudom koja je pokrivena = odmakH(max vrijednost)
//VAŽNO => Mjerno područje senzora je prema datasheetu između 2cm - 400cm, ukoliko se postavi odmahH<=2cm moguće je da će nastupiti greška uvjetovana senzorom
#define jePrecizan true // true ako je aktivan DHT11
#define trigpin 8
#define echopin 9 //PWM
#define dhtpin A0
#define piezopin 6
#define odmakL 18.7 //cm
#define odmakH 0 //cm

float ispis_LCD(float _razina_tekucine, float _visina_posude)
{
  float _postotak;
  if(_razina_tekucine<=0.05*_visina_posude)
    _postotak=0.0;
  else if(_razina_tekucine>=0.95*_visina_posude)
    _postotak=99.0;
  else
    _postotak=(((int)(_razina_tekucine+0.5))/_visina_posude)*100; //postotak razine, zaokruzivanje _razina_tekucine
  lcd.setCursor(0,1);
  lcd.print(_postotak);
  lcd.print("%");
  lcd.print("/");
  lcd.print(_razina_tekucine); 
  lcd.print("cm");
  return _postotak;
}

void signal_Piezozvucnika(float _postotak)
{
   if(_postotak>=50&&_postotak<=60) //reakcija piezozvucnika na 50%-60%
    {
      tone(piezopin,2000);
      delay(500);
      noTone(piezopin);
      delay(500);
      tone(piezopin,2000);
      delay(500);
      noTone(piezopin);
      delay(500);
    }
   else if(_postotak>=90&&_postotak<=100) //reakcija piezozvucnika na 90%-100%
   {
      tone(piezopin,2000);
      delay(2000);
      noTone(piezopin);
      delay(1000);
   }
}

void ispisSerijskiMonitor(float _postotak, float _razina_tekucine, float _temperatura, float _D)
{
  Serial.println("RAZINA MJERENE TEKUĆINE");
  Serial.print("Ispunjenost posude/spremnika:\n");
  Serial.print(_postotak);
  Serial.print(" %\n");
  Serial.print(_razina_tekucine);
  Serial.println(" cm");
  Serial.print("Temperatura okoline mjerenja: ");
  Serial.print(_temperatura);
  Serial.println(" °C");
  Serial.print("Udaljenost od površine tekućine (za kalibraciju): ");
  Serial.print(_D);
  Serial.println(" cm");
  Serial.println("___________________________________________________________________________");
  
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //upaljen serial monitor za ispis rezultata mjerenja i kalibraciju
  delay(500);
  lcd.begin(16,2);
  lcd.print("Ultrazvučna");
  lcd.setCursor(0,1);
  lcd.print("mjerna metoda");
  Serial.println("ULTRAZVUČNA MJERNA METODA");
  Serial.println("============================================================================");
  pinMode(trigpin,OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(piezopin, OUTPUT);
  delay(1000);
  }

void loop() {
  // put your main code here, to run repeatedly:
  float t; //vrijeme je output parametar senzora u us
  float D; // udaljenost je dobivena preko vremena u cm
  float v=0;
  float temperatura=DTH.temperature;
  float postotak;
  
  postotak=0.0;
  digitalWrite(trigpin,LOW);
  delayMicroseconds(5);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  t=pulseIn(echopin, HIGH);
  DTH.read11(dhtpin);
  
  if(!jePrecizan)
    v=0.034;// brzina je 340m/s => 0.034 cm/us
  else
    v=(331.4+(0.6*temperatura))/10000.0;
    
  D=(t*v)/2; 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Razina tekucine");
  
  float razina_tekucine=odmakL-D; //odmakL je referentni nivo, a D je izmjerena udaljenost
  float visina_posude=odmakL-odmakH; //visina posude

  postotak=ispis_LCD(razina_tekucine, visina_posude);
  ispisSerijskiMonitor(postotak,razina_tekucine,temperatura,D);
  signal_Piezozvucnika(postotak);
  
  delay(2000);
 }
