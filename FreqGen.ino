void setup() {
  pinMode(10, OUTPUT);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly: 
  if(!digitalRead(2))
  {
    digitalWrite(13, HIGH);
    tone(10, 500);
    delay(500);  
  }
  else
  {
    digitalWrite(13, LOW);
    tone(10, 550);
    delay(500);
  }
}
