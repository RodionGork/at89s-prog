unsigned char linebuf[128];

#define PIN_RESET 9
#define PIN_MOSI 10
#define PIN_MISO 11
#define PIN_SCK 12

#define SPI_DELAY 5

void setup() {
  pinMode(PIN_MOSI, INPUT); // mosi
  pinMode(PIN_MISO, INPUT); // miso
  pinMode(PIN_SCK, INPUT); // sck
  pinMode(PIN_RESET, OUTPUT); // rst
  digitalWrite(PIN_RESET, LOW);
  Serial.begin(1200);
  delay(100);
}

int hex(unsigned char c) {
  if (c <= '9') {
    return c - '0';
  }
  return c - 'A' + 10;
}

int xchbyte(int v) {
  delayMicroseconds(SPI_DELAY);
  int mask = 0x80;
  int res = 0;
  while (mask) {
    digitalWrite(PIN_MOSI, (v & mask) ? HIGH : LOW);
    delayMicroseconds(SPI_DELAY);
    digitalWrite(PIN_SCK, HIGH);
    delayMicroseconds(SPI_DELAY);
    res <<= 1;
    if (digitalRead(PIN_MISO) == HIGH) {
      res |= 1;
    }
    digitalWrite(PIN_SCK, LOW);
    delayMicroseconds(SPI_DELAY);
    mask >>= 1;
    delayMicroseconds(SPI_DELAY);
  }
  return res;
}

int xch4bytes(int cmd, int p1, int p2, int v) {
  xchbyte(cmd);
  xchbyte(p1);
  xchbyte(p2);
  return xchbyte(v);
}

int prgEnable() {
  return xch4bytes(0b10101100, 0b01010011, 0, 0);
}

long readSignature() {
  int b0 = xch4bytes(0b00101000, 0, 0, 0);
  if (b0 != 0x1E)
    return 0x0BEDA000 | b0;
  int b1 = xch4bytes(0b00101000, 0, 1, 0);
  int b2;
  if (b1 != 0x23 && b1 != 0x43) {
    b1 = xch4bytes(0b00101000, 1, 0, 0);
    b2 = xch4bytes(0b00101000, 2, 0, 0);
  } else {
    b2 = xch4bytes(0b00101000, 0, 2, 0);
  }
  return ((long) b0) << 16 | (b1 << 8) | b2;
}

void eraseChip() {
  xch4bytes(0b10101100, 0b10000000, 0, 0);
}

void writeByte(int addr, int b) {
  xch4bytes(0b01000000, (addr >> 8) & 0xFF, addr & 0xFF, b);
  delay(5);
}

int readByte(int addr) {
  return xch4bytes(0b00100000, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

void dbg(char* s) {
  Serial.println(s);
}

void dbg1(char* s, int v) {
  Serial.print(s);
  Serial.print(' ');
  Serial.println(v);
}

int readChar() {
  while (1) {
    int c = Serial.read();
    if (c >= 0) {
      return c;
    }
  }
}

void readLine() {
  int p = 0;
  while (1) {
    int c = readChar();
    if (c > ' ') {
      linebuf[p++] = c;
      break;
    }
  }
  while (1) {
    int c = readChar();
    if (c == -1) {
      continue;
    }
    if (c <= ' ') {
      linebuf[p] = 0;
      break;
    }
    linebuf[p] = c;
    if (p < sizeof(linebuf) - 1) {
      p += 1;
    }
  }
}

int byteAt(int i) {
  i = i * 2 + 1;
  return (hex(linebuf[i]) << 4) | hex(linebuf[i + 1]);
}

void writeHexLine(int sz, int addr) {
    Serial.print("W@ 0x");
    Serial.print(addr, HEX);
    Serial.print(" ");
    Serial.print(sz);
    Serial.print(" b:");
    for (int i = 0; i < sz; i++) {
      int v = byteAt(4 + i);
      writeByte(addr + i, v);
      delayMicroseconds(500);
      Serial.print(".");
    }
    Serial.println(" ok");
}

void readHexLine(int sz, int addr) {
    Serial.print("Verify at ");
    Serial.print(addr, HEX);
    Serial.print(":");
    for (int i = 0; i < sz; i++) {
      Serial.print(" ");
      Serial.print(readByte(addr + i), HEX);
    }
    Serial.println();
}

int process(void) {
  int pg, prevPg = -1;
  int offs;
  while (1) {
    readLine();
    int sz = byteAt(0);
    int addr = (byteAt(1) << 8) + byteAt(2);
    int flag = byteAt(3);
    if (flag != 0) {
      break;
    }
    if (linebuf[0] == ':')
      writeHexLine(sz, addr);
    else if (linebuf[0] == '?')
      readHexLine(sz, addr);
  }
}

void loop() {
  delay(100);
  int firstChar = Serial.peek();
  if (firstChar < 0) {
    return;
  }
  pinMode(PIN_SCK, OUTPUT);
  digitalWrite(PIN_SCK, LOW);
  delay(1);
  digitalWrite(PIN_RESET, HIGH);
  delay(4);
  pinMode(PIN_MOSI, OUTPUT);
  Serial.print("PRG EN: ");
  int pe = prgEnable();
  Serial.println(pe, HEX);
  Serial.print("SIG=");
  Serial.println(readSignature(), HEX);
  if (firstChar == '!') {
    Serial.println("ERASE");
    eraseChip();
    delay(500);
  } else {
    process();
    delay(10);
  }
  digitalWrite(PIN_RESET, LOW);
  pinMode(PIN_SCK, INPUT);
  pinMode(PIN_MOSI, INPUT);
  Serial.println("Prg mode off");
  delay(100);
  digitalWrite(PIN_RESET, HIGH);
  Serial.println("Activating reset (high)");
  delay(100);
  digitalWrite(PIN_RESET, LOW);
  Serial.println("...deactivating (low)");
  Serial.println("DONE");
  while (Serial.peek() >= 0) {
    Serial.read();
  }
}
