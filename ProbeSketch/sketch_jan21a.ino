const int inPins[]  = {2,3,4,5,7,8,9};
const int outPins[] = {6,10,11,12,13};

int chainStateA = 0; // 0:待機, 1:10動作中, 2:11待ち
int chainStateB = 0; // 0:待機, 1:12動作中, 2:13待ち
int nextA = -1; // -1:なし, 0:pin10, 1:pin11
int nextB = -1; // -1:なし, 2:pin12, 3:pin13

int lastState[7];
unsigned long lastChangeTime[7];
const unsigned long DEBOUNCE_MS = 10;

const unsigned long PULSE_WIDTH = 1000;

unsigned long pulseEnd[4] = {0,0,0,0};
bool pulseActive[4] = {false,false,false,false};

void setup() {
  Serial.begin(9600);

  for (int i=0; i<7; i++) {
    pinMode(inPins[i], INPUT);
    lastState[i] = digitalRead(inPins[i]);
    lastChangeTime[i] = millis();
  }

  for (int i=0; i<4; i++) {
    pinMode(outPins[i], OUTPUT);
    digitalWrite(outPins[i], HIGH);
  }

  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);


  // 起動時通知
  for (int i=0; i<7; i++) {
    Serial.print("EVT P");
    Serial.print(inPins[i]);
    Serial.print("=");
    Serial.println(digitalRead(inPins[i]));
  }
}

void loop() {
  unsigned long now = millis();

  //ワンショット終了処理
  for (int i=0; i<4; i++) {
    if (pulseActive[i] && now >= pulseEnd[i]) {
      digitalWrite(outPins[i], HIGH);
      pulseActive[i] = false;
    }
  }

  // A系 直列
if (chainStateA == 1 && !pulseActive[0]) {
  if (nextA == 1) {
    digitalWrite(11, LOW);
    pulseEnd[1] = now + PULSE_WIDTH;
    pulseActive[1] = true;
  }
  chainStateA = 0;
  nextA = -1;
}

// B系 直列
if (chainStateB == 1 && !pulseActive[2]) {
  if (nextB == 3) {
    digitalWrite(13, LOW);
    pulseEnd[3] = now + PULSE_WIDTH;
    pulseActive[3] = true;
  }
  chainStateB = 0;
  nextB = -1;
}


  //入力変化検出
  for (int i=0; i<7; i++) {
    int v = digitalRead(inPins[i]);
    if (v != lastState[i]) {
      if (now - lastChangeTime[i] > DEBOUNCE_MS) {

        // EVT 出力
        Serial.print("EVT P");
        Serial.print(inPins[i]);
        Serial.print("=");
        Serial.println(v);

        bool enabled = (digitalRead(7) == HIGH);

        if (enabled && inPins[i] == 9 && v == HIGH) {
        bool p2 = digitalRead(2);
        bool p3 = digitalRead(3);

        if (p2 && p3) {
          digitalWrite(10, LOW);
          pulseEnd[0] = now + PULSE_WIDTH;
          pulseActive[0] = true;
          chainStateA = 1;
          nextA = 1;   // 次は pin11
        }
        else if (p2) {
          digitalWrite(10, LOW);
          pulseEnd[0] = now + PULSE_WIDTH;
          pulseActive[0] = true;
        }
        else if (p3) {
          digitalWrite(11, LOW);
          pulseEnd[1] = now + PULSE_WIDTH;
          pulseActive[1] = true;
        }
      }


        //pin8 トリガ
        if (enabled && inPins[i] == 8 && v == HIGH) {
        bool p4 = digitalRead(4);
        bool p5 = digitalRead(5);

        if (p4 && p5) {
          digitalWrite(12, LOW);
          pulseEnd[2] = now + PULSE_WIDTH;
          pulseActive[2] = true;
          chainStateB = 1;
          nextB = 3;   // 次は pin13
        }
        else if (p4) {
          digitalWrite(12, LOW);
          pulseEnd[2] = now + PULSE_WIDTH;
          pulseActive[2] = true;
        }
        else if (p5) {
          digitalWrite(13, LOW);
          pulseEnd[3] = now + PULSE_WIDTH;
          pulseActive[3] = true;
        }
      }

        lastState[i] = v;
        lastChangeTime[i] = now;
      }
    }
  }

    //シリアルコマンド処理
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();

      // 状態確認
      if (cmd == "R?") {
        Serial.print("IN ");
        for (int i=0; i<7; i++) {
          Serial.print("P");
          Serial.print(inPins[i]);
          Serial.print("=");
          Serial.print(digitalRead(inPins[i]));
          Serial.print(" ");
        }
        Serial.println();
        return;
      }

      // pin7=LOW のときのみ実行
      if (digitalRead(7) != LOW) {
        Serial.println("NG P7!=LOW");
        return;
      }

      //A系

      // pin10 のみ
      if (cmd == "P10 1" && !pulseActive[0]) {
        digitalWrite(10, LOW);
        pulseEnd[0] = now + PULSE_WIDTH;
        pulseActive[0] = true;
        Serial.println("OK P10");
      }

      // pin11 のみ
      else if (cmd == "P11 1" && !pulseActive[1]) {
        digitalWrite(11, LOW);
        pulseEnd[1] = now + PULSE_WIDTH;
        pulseActive[1] = true;
        Serial.println("OK P11");
      }

      // pin10 → pin11
      else if (cmd == "PA 1" && chainStateA == 0 && !pulseActive[0]) {
        digitalWrite(10, LOW);
        pulseEnd[0] = now + PULSE_WIDTH;
        pulseActive[0] = true;
        chainStateA = 1;
        nextA = 1;
        Serial.println("OK PA");
      }

      //B系

      // pin12 のみ
      else if (cmd == "P12 1" && !pulseActive[2]) {
        digitalWrite(12, LOW);
        pulseEnd[2] = now + PULSE_WIDTH;
        pulseActive[2] = true;
        Serial.println("OK P12");
      }

      // pin13 のみ
      else if (cmd == "P13 1" && !pulseActive[3]) {
        digitalWrite(13, LOW);
        pulseEnd[3] = now + PULSE_WIDTH;
        pulseActive[3] = true;
        Serial.println("OK P13");
      }

      // pin12 → pin13
      else if (cmd == "PB 1" && chainStateB == 0 && !pulseActive[2]) {
        digitalWrite(12, LOW);
        pulseEnd[2] = now + PULSE_WIDTH;
        pulseActive[2] = true;
        chainStateB = 1;
        nextB = 3;
        Serial.println("OK PB");
      }

      else {
        Serial.println("NG CMD");
      }
    }

    //Pin6 ステータス出力
      bool pin6High = false;

      // 条件①：pin7=HIGH かつ pin8 or pin9 が HIGH
      if (digitalRead(7) == HIGH &&
          (digitalRead(8) == HIGH || digitalRead(9) == HIGH)) {
        pin6High = true;
      }

      // 条件②：pin7=LOW かつ 出力のどれかが LOW
      if (digitalRead(7) == LOW) {
        for (int i = 1; i < 5; i++) {   // outPins[1]=10 ～ outPins[4]=13
          if (digitalRead(outPins[i]) == LOW) {
            pin6High = true;
            break;
          }
        }
      }

      // 出力反映
      digitalWrite(6, pin6High ? HIGH : LOW);

}


