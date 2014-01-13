int laserInput = 2;

void setup() {
  Serial.begin(9600);
  pinMode(laserInput, INPUT);
}

void loop() {
  int laserState = digitalRead(laserInput);
  int sensorValue = analogRead(A0);
  
  Serial.print(laserState);
  Serial.print(' ');
  Serial.println(sensorValue);
  delay(1);
}
