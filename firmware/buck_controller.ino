
const int PWM_PIN = 9;
const int VOUT_PIN = A0;

const float V_TARGET = 5.0;
const float ADC_REF = 5.31;

const float Kp = 0.005;
const float Ki = 0.12;

const float BASE_DUTY = 0;
const float MAX_DUTY = 0.35;
const float OVERVOLTAGE = 5.6;

float integral = 0;
double duty = 0;
float vout = 0;

enum State { OFF, STARTING, RUNNING, FAULT };
State state = OFF;

unsigned long lastControl = 0;
unsigned long lastReport = 0;

void setPWM(float d) {
  d = constrain(d, 0.0, MAX_DUTY);
  duty = d;

  if (d <= 0.0) {
    TCCR1A &= ~(1 << COM1A1);
    digitalWrite(PWM_PIN, LOW);
    OCR1A = 0;
  } else {
    OCR1A = (int)(d * 800.0);
    TCCR1A |= (1 << COM1A1);
  }
}

  
float readVout() {
  int adc = analogRead(VOUT_PIN);
  return adc * (ADC_REF / 1023.0) * 2.4943; //due to Resistance variation i did not use 2.5
}
 
void setup() {
  pinMode(PWM_PIN, OUTPUT);
  digitalWrite(PWM_PIN, LOW);

  Serial.begin(9600);
  analogReference(DEFAULT);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  ICR1 = 799;
  OCR1A = 0;

  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13) |
           (1 << WGM12) |
           (1 << CS10);

  Serial.println("s = START, x = STOP");
}

void loop() {
  unsigned long now = millis();

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 's' && state == OFF) {
      integral = 0;
      setPWM(0);
      lastControl = now;
      state = STARTING;
    }

    if (cmd == 'x' && state != FAULT) {
      setPWM(0);
      state = OFF;
      integral = 0;
      Serial.println("PWM OFF");
    }
  }

  if (state == STARTING) {
      setPWM(BASE_DUTY);
      state = RUNNING;
  }

  if ((state == STARTING || state == RUNNING)
      && now - lastControl >= 5) {

     lastControl = now;
     int adcRaw = analogRead(A0);

     float va0 = adcRaw * (ADC_REF / 1023.0);

     vout = va0 * 2.5;
     
    if (vout > OVERVOLTAGE) {
    setPWM(0);
    state = FAULT;

    Serial.println("!!! OVERVOLTAGE !!!");

    Serial.print("ADC RAW = ");
    Serial.println(adcRaw);

    Serial.print("A0 = ");
    Serial.println(va0, 3);

    Serial.print("Vout = ");
    Serial.println(vout, 3);

    Serial.print("Time = ");
    Serial.println(millis());
}

    if (state == RUNNING) {
      float vref = V_TARGET;
      float error = vref - vout;
      float dt = 0.005;

      float newIntegral =
          integral + Ki * error * dt;

      float candidate =
          BASE_DUTY + Kp * error + newIntegral;

      if (!((candidate > MAX_DUTY && error > 0) ||
            (candidate < 0 && error < 0))) {
        integral = newIntegral;
      }

      float newDuty =
          BASE_DUTY + Kp * error + integral;

      setPWM(newDuty);
    }
  }

  if (state == RUNNING &&
      now - lastReport >= 1) {

      lastReport = now;

      Serial.print("Vout: ");
      Serial.print(vout, 2);
      Serial.print(" V | Duty: ");
      Serial.print(duty * 100, 4);
      Serial.println(" %");

  }
}
