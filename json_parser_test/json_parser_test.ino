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

prog_uchar JSON_NUM_ARRAY[] PROGMEM = {"{\"one\":[123,6734,5,9435],\"two\":[54,78,92,34,56,82,724]}"};
prog_uchar JSON_ONE_NUM[] PROGMEM = {"{\"numfield\":5}"};
prog_uchar JSON_ONE_NUM_NEGATIVE[] PROGMEM = {"{\"negfield\":-42}"};
prog_uchar JSON_NUM_ARRAY_NEGATIVE[] PROGMEM = {"{\"one\":[-123,-6734 , -9435]}"};
prog_uint16_t ARR_LEN[] PROGMEM = {55, 14, 16, 28};

prog_uchar TEST_CASE_1_NAME[] PROGMEM = {"numberArray"};
prog_uchar TEST_CASE_2_NAME[] PROGMEM = {"oneNumber"};
prog_uchar TEST_CASE_3_NAME[] PROGMEM = {"oneNumberNegative"};
prog_uchar TEST_CASE_4_NAME[] PROGMEM = {"numberArrayNegative"};
prog_uint16_t TEST_CASE_NAME_LEN[] PROGMEM = {11, 9, 17, 19};


void setup() {
  Serial.begin(9600);
  Serial.println(F("=================================================================="));
  Serial.println(F("Starting tests"));
  Serial.println();
  printFM();
  testNumberArray();
  printFM();
  testOneNumber();
  printFM();
  testOneNumberNegative();
  printFM();
  testNumberArrayNegative();
  printFM();
  Serial.println(F("Finished tests"));
  Serial.println(F("=================================================================="));
  Serial.println();
  Serial.println();
}

void testNumberArray() {
  JsonParser* fixture;
  boolean success;
  NUMERIC_TYPE* arr;
  
  start(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0]);
  fixture = buildJsonParser(JSON_NUM_ARRAY, pgm_read_word_near(ARR_LEN));
  
  success = assertEquals(1, (byte)2, fixture->numFields());
  if(success){
    success = assertEquals(2, "one", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(3, (byte)4, fixture->getArrayFieldLength(0));
  }
  
  if(success){
    arr = fixture->getFieldNumberArray(0);

    if(success){
      success = assertEquals(4, 123, arr[0]);
    }
    if(success){
      success = assertEquals(5, 6734, arr[1]);
    }
    if(success){
      success = assertEquals(6, 5, arr[2]);
    }
    if(success){
      success = assertEquals(7, 9435, arr[3]);
    }
  }
  
  if(success){
    success = assertEquals(8, "two", fixture->fieldName(1));
  }
  if(success){
    success = assertEquals(9, (byte)7, fixture->getArrayFieldLength(1));
  }

  if(success){
    arr = fixture->getFieldNumberArray(1);
    if(success){
      success = assertEquals(10, 54, arr[0]);
    }
    if(success){
      success = assertEquals(11, 78, arr[1]);
    }
    if(success){
      success = assertEquals(12, 92, arr[2]);
    }
    if(success){
      success = assertEquals(13, 34, arr[3]);
    }
    if(success){
      success = assertEquals(14, 56, arr[4]);
    }
    if(success){
      success = assertEquals(15, 82, arr[5]);
    }
    if(success){
      success = assertEquals(16, 724, arr[6]);
    }
  }
  
  delete fixture;
  finish(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], success);
}

void testOneNumber() {
  JsonParser* fixture;
  boolean success;
  
  start(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1]);
  
  fixture = buildJsonParser(JSON_ONE_NUM, pgm_read_word_near(ARR_LEN + 1));
  success = assertEquals(1, (byte)1, fixture->numFields());
  
  if(success){
    success = assertEquals(2, "numfield", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(3, 5, fixture->getFieldNumber(0));
  }
  
  delete fixture;
  finish(TEST_CASE_2_NAME, TEST_CASE_NAME_LEN[1], success);
}

void testOneNumberNegative() {
  JsonParser* fixture;
  boolean success;
  
  start(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2]);
  
  fixture = buildJsonParser(JSON_ONE_NUM_NEGATIVE, pgm_read_word_near(ARR_LEN + 2));
  success = assertEquals(1, (byte)1, fixture->numFields());
  
  if(success){
    success = assertEquals(2, "negfield", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(3, -42, fixture->getFieldNumber(0));
  }
  
  delete fixture;
  finish(TEST_CASE_3_NAME, TEST_CASE_NAME_LEN[2], success);
}

void testNumberArrayNegative() {
  JsonParser* fixture;
  boolean success;
  NUMERIC_TYPE* arr;
  
  start(TEST_CASE_4_NAME, TEST_CASE_NAME_LEN[3]);
  fixture = buildJsonParser(JSON_NUM_ARRAY_NEGATIVE, pgm_read_word_near(ARR_LEN + 3));
  
  success = assertEquals(1, (byte)1, fixture->numFields());
  if(success){
    success = assertEquals(2, "one", fixture->fieldName(0));
  }
  if(success){
    success = assertEquals(3, (byte)3, fixture->getArrayFieldLength(0));
  }
  
  if(success){
    arr = fixture->getFieldNumberArray(0);

    if(success){
      success = assertEquals(4, -123, arr[0]);
    }
    if(success){
      success = assertEquals(5, -6734, arr[1]);
    }
    if(success){
      success = assertEquals(6, -9435, arr[2]);
    }
  }
  
  delete fixture;
  finish(TEST_CASE_1_NAME, TEST_CASE_NAME_LEN[0], success);
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

boolean assertEquals(int stepNum, NUMERIC_TYPE expected, NUMERIC_TYPE actual) {
  boolean result;
  
  if(expected != actual){
    Serial.print(F("FAIL step "));
    Serial.print(stepNum);
    Serial.print(F(" - expected ["));
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


boolean assertEquals(int stepNum, byte expected, byte actual) {
  boolean result;
  
  if(expected != actual){
    Serial.print(F("FAIL step "));
    Serial.print(stepNum);
    Serial.print(F(" - expected ["));
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


boolean assertEquals(int stepNum, String expected, String actual) {
  boolean result;
  
  if(expected != actual){
    Serial.print(F("FAIL step "));
    Serial.print(stepNum);
    Serial.print(F(" - expected ["));
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

