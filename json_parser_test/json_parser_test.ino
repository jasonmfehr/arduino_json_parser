/*

Copyright (C) 2013 Jason Fehr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

This file contains basic tests for the JsonParser library.  These 
tests must be executed on an arduino board.  Open the serial 
monitor to see the results of the tests.  Failed tests may produce 
inconsistent results because of the SRAM filling up.  A completely 
successful test run will have the following two lines at the end 
of the serial monitor output.

Finished tests
==================================================================

Using this sketch requires having the MemoryFree library available.  
It can be downloaded from http://playground.arduino.cc/Code/AvailableMemory
*/

//TODO use flash (program) memory to store all strings to avoid issues with memory filling up
//TODO test negative numbers


#include <LinkedList.h>
#include <JsonParser.h>
#include <avr/pgmspace.h>
#include <MemoryFree.h>

//type for numbers in the json parser
#define NUMERIC_TYPE int

prog_uchar JSON_STR_ARRAY[] PROGMEM = {"{\"arr1\":[\"val 1\",\"val 2\",\"val 3\"],\"arr2\":[\"val 4\",\"val 5\",\"val 6\",\"val 7\"]}"};
prog_uchar JSON_STR_NUM[] PROGMEM = {"{\"field\":\"val here\",\"num1\",42,\"field2\":\"val2\",\"num2\",123456789}"};
prog_uchar JSON_ALL_ARRAY[] PROGMEM = {"{\"a\":\"one\",\"b\":2,\"c\":[3,4,5,6,7],\"d\":[\"eight\",\"nine\",\"ten\"]}"};
prog_uchar JSON_NUM_ARRAY[] PROGMEM = {"{\"one\":[123,6734,5,9435],\"two\":[54,78,92,34,56,82,724]}"};
prog_uchar JSON_ONE_STR[] PROGMEM = {"{\"strfield\":\"my string field value\"}"};
prog_uint16_t ARR_LEN[] PROGMEM = {75, 63, 60, 58, 37};

prog_uchar TEST_CASE_1_NAME[] PROGMEM = {"stringNumberFields"};
prog_uchar TEST_CASE_2_NAME[] PROGMEM = {"stringArray"};
prog_uchar TEST_CASE_3_NAME[] PROGMEM = {"numberArray"};
prog_uchar TEST_CASE_4_NAME[] PROGMEM = {"allCombined"};
prog_uchar TEST_CASE_5_NAME[] PROGMEM = {"oneField"};
prog_uint16_t TEST_CASE_NAME_LEN[] PROGMEM = {18, 11, 11, 11, 8};


void setup() {
  Serial.begin(9600);
  Serial.println("==================================================================");
  Serial.println(F("Starting tests"));
  Serial.println();
  printFM();
  testStringNumberFields();
  printFM();
  testStringArray();
  printFM();
  testNumberArray();
  printFM();
  testAllCombined();
  printFM();
  testOneField();
  printFM();
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
  
  success = assertEquals(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], "num fields", (byte)4, fixture->numFields());
  
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
  String** arr;
  
  start(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1]);
  fixture = buildJsonParser(JSON_STR_ARRAY,  pgm_read_word_near(ARR_LEN));
  
  success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "num fields", (byte)2, fixture->numFields());
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 name", "arr1", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 size", (byte)3, fixture->getArrayFieldLength(0));
  }
  arr = fixture->getFieldStringArray(0);
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 1", "val 1", *arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 2", "val 2", *arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 1 val 3", "val 3", *arr[2]);
  }
  delete arr;

  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 name", "arr2", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 size", (byte)4, fixture->getArrayFieldLength(1));
  }
  arr = fixture->getFieldStringArray(1);

  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 1", "val 4", *arr[0]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 2", "val 5", *arr[1]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 3", "val 6", *arr[2]);
  }
  
  if(success){
    success = assertEquals(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], "arr 2 val 4", "val 7", *arr[3]);
  }
  
  delete arr;
  
  delete fixture;
  finish(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], success);
}

void testNumberArray() {
  JsonParser* fixture;
  boolean success;
  NUMERIC_TYPE** arr;
  
  start(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2]);
  fixture = buildJsonParser(JSON_NUM_ARRAY, pgm_read_word_near(ARR_LEN + 3));
  
  success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "num fields", (byte)2, fixture->numFields());
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 name", "one", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f 1 len", (byte)4, fixture->getArrayFieldLength(0));
  }
  
  arr = fixture->getFieldNumberArray(0);
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v0", 123, *arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v1", 6734, *arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v2", 5, *arr[2]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f1 v3", 9435, *arr[3]);
  }
  delete arr;
  
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 name", "two", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 len", (byte)7, fixture->getArrayFieldLength(1));
  }

  arr = fixture->getFieldNumberArray(1);
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v0", 54, *arr[0]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v1", 78, *arr[1]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v2", 92, *arr[2]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v3", 34, *arr[3]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v4", 56, *arr[4]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v6", 82, *arr[5]);
  }
  if(success){
    success = assertEquals(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], "f2 v7", 724, *arr[6]);
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
  
  success = assertEquals(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], "num fields", (byte)4, fixture->numFields());
  
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
  NUMERIC_TYPE** arr;
  
  success = assertEquals(testCaseName, len, "f3 name", "c", jp->fieldName(2));
  if(success){
    success = assertEquals(testCaseName, len, "f3 len", (byte)5, jp->getArrayFieldLength(2));
  }
  
  arr = jp->getFieldNumberArray(2);
  if(success){
    success = assertEquals(testCaseName, len, "f3 v0", 3, *arr[0]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v1", 4, *arr[1]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v2", 5, *arr[2]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v3", 6, *arr[3]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f3 v4", 7, *arr[4]);
  }
  delete arr;
  
  return success;
}

boolean testAllCombined_AssertStringArray(prog_uchar* testCaseName, int len, JsonParser* jp) {
  boolean success;
  String** arr;
  
  success = assertEquals(testCaseName, len, "f4 name", "d", jp->fieldName(3));
  if(success){
    success = assertEquals(testCaseName, len, "f4 length", (byte)3, jp->getArrayFieldLength(3));
  }
  
  arr = jp->getFieldStringArray(3);
  if(success){
    success = assertEquals(testCaseName, len, "f4 v0", "eight", *arr[0]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f4 v1", "nine", *arr[1]);
  }
  if(success){
    success = assertEquals(testCaseName, len, "f4 v2", "ten", *arr[2]);
  }
  delete arr;
  
  return success;
}

void testOneField() {
  JsonParser* fixture;
  boolean success;
  
  start(TEST_CASE_5_NAME, TEST_CASE_NAME_LEN[4]);
  
  fixture = buildJsonParser(JSON_ONE_STR, pgm_read_word_near(ARR_LEN + 4));
  success = assertEquals(TEST_CASE_5_NAME, TEST_CASE_NAME_LEN[4], "num fields", (byte)1, fixture->numFields());
  
  if(success){
    success = assertEquals(TEST_CASE_5_NAME, TEST_CASE_NAME_LEN[4], "field name", "strfield", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(TEST_CASE_5_NAME, TEST_CASE_NAME_LEN[4], "field val", "my string field value", fixture->getFieldString(0));
  }
  
  delete fixture;
  finish(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3], success);
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

void printFM() {
  Serial.print(F("Free Memory: "));
  Serial.println(freeMemory());
  Serial.println();
}

boolean assertEquals(prog_uchar* testCaseName, int len, String msg, NUMERIC_TYPE expected, NUMERIC_TYPE actual) {
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



boolean assertEquals(prog_uchar* testCaseName, int len, String msg, byte expected, byte actual) {
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
/*
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
*/

