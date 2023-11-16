//I2C KEYBOARD MATRIX WITH PULLDOWN RESISTORS
 
 #include <Wire.h>

byte rows[] = {2,3,4,5}; //HIGH
const int rowCount = sizeof(rows)/sizeof(rows[0]);

byte cols[] = {6,7,8,9};
const int colCount = sizeof(cols)/sizeof(cols[0]);

byte keys[colCount][rowCount];

void setup() {
  Serial.begin(115200);
  Wire.begin(25);                // join I2C bus with address #25
  Wire.onRequest(requestEvent); // register event

  for(int x=0; x<rowCount; x++) {
    Serial.print(rows[x]); Serial.println(" as output");
    pinMode(rows[x], OUTPUT);
  }

  for (int x=0; x<colCount; x++) {
    Serial.print(cols[x]); Serial.println(" as input");
    pinMode(cols[x], INPUT);
  }
    
}

void readMatrix() {

  for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
    byte curRow = rows[rowIndex];
    digitalWrite(curRow, HIGH);
    
    for (int colIndex=0; colIndex < colCount; colIndex++) {
      byte curCol = cols[colIndex];
      if (keys[rowIndex][colIndex] != 1) {
        keys[rowIndex][colIndex] = digitalRead(curCol);
      }
    }

    digitalWrite(curRow, LOW);
  }
}

void printMatrix() {
  for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
    if (rowIndex < 10)
      Serial.print(F("0"));
    Serial.print(rowIndex); Serial.print(F(": "));

    for (int colIndex=0; colIndex < colCount; colIndex++) { 
      Serial.print(keys[rowIndex][colIndex]);
      if (colIndex < colCount)
        Serial.print(F(", "));
    } 
    Serial.println("");
  }
  Serial.println("");
}

void loop() {
  readMatrix();
  if (Serial.read()=='!')
    printMatrix();
}

uint16_t matrixToBitmap(byte keys[colCount][rowCount]) {
    uint16_t bitmap = 0;
    for (int i = 0; i < colCount; i++) {
        for (int j = 0; j < rowCount; j++) {
            if (keys[i][j] == 1) {
                // Set the bit at the correct position
                bitmap |= (1 << (i * rowCount + j));
            }
        }
    }
    return bitmap;
}

void convertUint16ToUint8(uint16_t value, uint8_t output[2]) {
    output[0] = (value >> 8) & 0xFF; // High byte
    output[1] = value & 0xFF;        // Low byte
}

void requestEvent() {

  uint16_t bitmap = matrixToBitmap(keys);
  uint8_t bytes[2];

  convertUint16ToUint8(bitmap, bytes);

  Wire.write(bytes, 2); // respond with message of 2 bytes
  // as expected by master
  memset(keys, 0, sizeof(keys));
}