#include <LinkedList.h>
#include <JsonParser.h>
#include <avr/pgmspace.h>

prog_uchar JSON_STR_ARRAY[] PROGMEM = {"{\"arr1\":[\"val 1\",\"val 2\",\"val 3\"],\"arr2\":[\"val 4\",\"val 5\",\"val 6\",\"val 7\"]}"};
prog_uchar JSON_STR_NUM[] PROGMEM = {"{\"field\":\"val here\",\"num1\",42,\"field2\":\"val2\",\"num2\",123456789}"};
prog_uchar JSON_ALL_ARRAY[] PROGMEM = {"{\"a\":\"one\",\"b\":2,\"c\":[3,4,5,6,7],\"d\":[\"eight\",\"nine\",\"ten\"]}"};
prog_uchar JSON_NUM_ARRAY[] PROGMEM = {"{\"one\":[123,673456,5,94353],\"two\":[54,78,92,34,56,82,724]}"};
prog_uint16_t ARR_LEN[] PROGMEM = {75, 63, 60, 58};

prog_uchar TEST_CASE_1_NAME[] PROGMEM = {"stringNumberFields"};
prog_uchar TEST_CASE_2_NAME[] PROGMEM = {"stringArray"};
prog_uchar TEST_CASE_3_NAME[] PROGMEM = {"numberArray"};
prog_uchar TEST_CASE_4_NAME[] PROGMEM = {"allCombined"};
prog_uint16_t TEST_CASE_NAME_LEN[] PROGMEM = {18, 11, 11, 11};


void setup() {
  Serial.begin(9600);
  Serial.println("==================================================================");
  Serial.println();
  Serial.println(F("Starting tests"));
  testStringNumberFields();
  testStringArray();
  testNumberArray();
  testAllCombined();
  Serial.println(F("Finished tests"));
  Serial.println("==================================================================");
  Serial.println();
  Serial.println();
}

void testStringNumberFields() {
  JsonParser* fixture;
  boolean success;
  
  start(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0]);
  
  fixture = buildJsonParser(JSON_STR_NUM, pgm_read_word_near(ARR_LEN + 1));
  
  success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "num fields", 4, fixture->numFields());
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "name field 1", "field", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "val field 1", "val here", fixture->getFieldString(0));
  }
  
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "name num 1", "num1", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "val num 1", 42, fixture->getFieldNumber(1));
  }
  
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "name field 2", "field2", fixture->fieldName(2));
  }
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "val field 2", "val2", fixture->getFieldString(2));
  }
  
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "name num 2", "num2", fixture->fieldName(3));
  }
  if(success){
    success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "val num 2", 123456789, fixture->getFieldNumber(3));
  }
  
  delete fixture;
  finish(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], success);
}

void testStringArray() {
  JsonParser* fixture;
  boolean success;
  String* arr;
  
  start(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1]);
  fixture = buildJsonParser(JSON_STR_ARRAY,  pgm_read_word_near(ARR_LEN));
  
  success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "num fields", 2, fixture->numFields());
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 name", "arr1", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 size", 3, fixture->getArrayFieldLength(0));
  }
  arr = fixture->getFieldStringArray(0);
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 1", "val 1", arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 2", "val 2", arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 3", "val 3", arr[2]);
  }
  delete arr;

  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 name", "arr2", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 size", 4, fixture->getArrayFieldLength(1));
  }
  arr = fixture->getFieldStringArray(1);

  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 1", "val 4", arr[0]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 2", "val 5", arr[1]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 3", "val 6", arr[2]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 4", "val 7", arr[3]);
  }
  
  delete arr;
  
  delete fixture;
  finish(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], success);
}

void testNumberArray() {
  JsonParser* fixture;
  boolean success;
  long* arr;
  
  start(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2]);
  fixture = buildJsonParser(JSON_NUM_ARRAY, pgm_read_word_near(ARR_LEN + 3));
  
  success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "num fields", 2, fixture->numFields());
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 name", "one", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f 1 len", 4, fixture->getArrayFieldLength(0));
  }
  
  arr = fixture->getFieldNumberArray(0);
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v0", 123, arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v1", 673456, arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v2", 5, arr[2]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v3", 94353, arr[3]);
  }
  delete arr;
  
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 name", "two", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 len", 7, fixture->getArrayFieldLength(1));
  }

  arr = fixture->getFieldNumberArray(1);
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v0", 54, arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v1", 78, arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v2", 92, arr[2]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v3", 34, arr[3]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v4", 56, arr[4]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v6", 82, arr[5]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v7", 724, arr[6]);
  }
  
  delete arr;
  
  delete fixture;
  finish(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], success);
}

void testAllCombined() {
  JsonParser* fixture;
  boolean success;
  
  start(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3]);
  
  fixture = buildJsonParser(JSON_ALL_ARRAY, pgm_read_word_near(ARR_LEN + 2));
  
  success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "num fields", 4, fixture->numFields());
  
  if(success){
    success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "f1 name", "a", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "f1 val", "one", fixture->getFieldString(0));
  }
  
  if(success){
    success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "f2 name", "b", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "f2 val", 2, fixture->getFieldNumber(1));
  }
  
  if(success){
    success = testAllCombined_AssertNumberArray(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], fixture);
  }
  
  if(success){
    success = testAllCombined_AssertStringArray(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], fixture);
  }
  
  delete fixture;
  finish(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], success);
}

boolean testAllCombined_AssertNumberArray(prog_uchar* testCaseName, int len, JsonParser* jp) {
  boolean success;
  long* arr;
  
  success = assertEquals(testCaseName, len, "f3 name", "c", jp->fieldName(2));
  if(success){
    success = assertEquals(testCaseName, len, "f3 len", 5, jp->getArrayFieldLength(2));
  }
  
  arr = jp->getFieldNumberArray(2);
  if(success){
    success = assertEquals(testCaseName, len, "f3 v0", 3, arr[0]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v1", 4, arr[1]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v2", 5, arr[2]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v3", 6, arr[3]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v4", 7, arr[4]);
  }
  delete arr;
  
  return success;
}

boolean testAllCombined_AssertStringArray(prog_uchar* testCaseName, int len, JsonParser* jp) {
  boolean success;
  String* arr;
  
  success = assertEquals(testCaseName, len, "f4 name", "d", jp->fieldName(3));
  if(success){
    success = assertEquals(testCaseName, len, "f4 length", 3, jp->getArrayFieldLength(3));
  }
  
  arr = jp->getFieldStringArray(3);
  if(success){
    success = assertEquals(testCaseName, len, "f4 v0", "eight", arr[0]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f4 v1", "nine", arr[1]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f4 v2", "ten", arr[2]);
  }
  delete arr;
  
  return success;
}

void start(prog_uchar* testCaseName, int len) {
  Serial.print(F("Starting test case ["));
  printProgStr(testCaseName, len);
  Serial.println(F("]"));
}

void finish(prog_uchar* testCaseName, int len, boolean success) {
  Serial.print(F("Finished test case ["));
  printProgStr(testCaseName, len);
  Serial.print("] status: ");
  Serial.println(success ? F("SUCCESS") : F("FAILURE"));
  Serial.println();
}

boolean assertEquals(prog_uchar* testCaseName, int len, String msg, long expected, long actual) {
  boolean result;
  
  if(expected != actual){
    Serial.print(F("FAIL ["));
    printProgStr(testCaseName, len); 
    Serial.print(F("] - "));
    Serial.print(msg);
    Serial.print(F(" expected ["));
    Serial.print(expected);
    Serial.print(F("] but was ["));
    Serial.print(actual);
    Serial.println(F("]"));
    result = false;
  }else{
    result = true;
  }
  
  return result;
}

boolean assertEquals(prog_uchar* testCaseName, int len, String msg, String expected, String actual) {
  boolean result;
  
  if(expected != actual){
    Serial.print(F("FAIL ["));
    printProgStr(testCaseName, len); 
    Serial.print(F("]) - "));
    Serial.print(msg);
    Serial.print(F(" expected ["));
    Serial.print(expected);
    Serial.print(F("] but was ["));
    Serial.print(actual);
    Serial.println(F("]"));
    result = false;
  }else{
    result = true;
  }
}

JsonParser* buildJsonParser(prog_uchar* str, int len) {
  JsonParser* jp = new JsonParser();
  
  Serial.print(F("JSON: "));

  for(int i=0; i<len; i++){
    jp->addChar((char)pgm_read_byte(str + i));
    Serial.print((char)pgm_read_byte_near(str + i));
  }

  Serial.println();
  
  return jp;
}

void printProgStr(prog_uchar* str, int len) {
  for(int i=0; i<len; i++){
    Serial.print((char)pgm_read_byte_near(str + i));
  }
}

void printLnProgStr(prog_uchar* str, int len) {
  printProgStr(str, len);
  Serial.println();
}

void loop() {
}

extern int __bss_end;
extern void *__brkval;

int memoryFree() {
  int freeValue;  
  if((int)__brkval == 0)
     freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);

  Serial.print("Free memory: ");
  Serial.println(freeValue);
  return freeValue;
}