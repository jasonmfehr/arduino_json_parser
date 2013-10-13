/*

Copyright (C) 2013 Jason Fehr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.


Implementation of a json parser that operates on a stream of json 
data.  There are a few limitations to what this parser can do.  It 
cannot parse objects, booleans, nulls, objects, or escaped 
charaters.  It also may have trouble handling spaces outside of 
string values.  Spaces within field names are not supported.  It 
can parse numeric, string, numeric arrays, and string arrays.

Because of the arduino's limited exception handling, most safety 
checks have been removed.  For example, it will not complain if 
one of the getField functions is called with an argument that is 
greater than the number of fields.  Use carefully, you have 
been warned!

*/


#ifndef JSON_PARSE
#define JSON_PARSE

#include "LinkedList.h"
#include "Arduino.h"

//if something other than an int needs to be used to store numerbs, 
//change this macro to the desired type
#define NUMERIC_TYPE int

//defines the type of variable used to store the number of fields
//also determines the maximum number of fields that can be stored
#define FIELD_INDEX_TYPE byte

//defines the type of variable used to hold the number of items 
//in an array therefore determines the upper bounds on parsed arrays
#define ARRAY_LENGTH_TYPE byte

//TODO: handle negative numbers

#define JSON_DELIMETER '"'
#define JSON_ARRAY_BEGIN '['
#define JSON_ARRAY_END ']'
#define ASCII_ZERO 48
#define ASCII_NINE 57

//definitions used for the internal tracking of types of JSON data
#define JSON_STRING 0
#define JSON_NUMBER 1
#define JSON_STRING_ARRAY 2
#define JSON_NUMBER_ARRAY 3

//definitions of the different internal states that the parser can be in
#define JPIS_BETWEEN_FIELDS 0
#define JPIS_BETWEEN_FIELD_NAME_VALUE 1
#define JPIS_FIELD_NAME 2
#define JPIS_FIELD_VALUE_STRING 3
#define JPIS_FIELD_VALUE_NUMBER 4
#define JPIS_ARRAY_START 5
#define JPIS_ARRAY_STRING 6
#define JPIS_ARRAY_NUMBER 7


class JsonParser {
  
  public:
    JsonParser();
    ~JsonParser();
    void addChar(char);
    FIELD_INDEX_TYPE numFields();
    String fieldName(FIELD_INDEX_TYPE);
    byte fieldType(FIELD_INDEX_TYPE);
    String getFieldString(FIELD_INDEX_TYPE);
    NUMERIC_TYPE getFieldNumber(FIELD_INDEX_TYPE);
    String* getFieldStringArray(FIELD_INDEX_TYPE);
    NUMERIC_TYPE* getFieldNumberArray(FIELD_INDEX_TYPE);
    ARRAY_LENGTH_TYPE getArrayFieldLength(FIELD_INDEX_TYPE);
    
  private:
    struct JSON_FIELD {
      String fieldName;
      void* fieldData;
      byte fieldType;
    
      JSON_FIELD() {
        fieldData = NULL;
      }
    
      ~JSON_FIELD() {
        if(fieldData != NULL){
          switch(fieldType){
            case JSON_STRING:
              delete (String*) fieldData;
              break;
            case JSON_NUMBER:
              delete (NUMERIC_TYPE*) fieldData;
              break;
            case JSON_STRING_ARRAY:
              for(int i=0; i<((LinkedList<String*>*)fieldData)->numItems(); i++){
                delete (String*)((LinkedList<String*>*)fieldData)->getValue(i);
              }

              delete (LinkedList<String*>*) fieldData;
              break;
            case JSON_NUMBER_ARRAY:
              for(int i=0; i<((LinkedList<NUMERIC_TYPE*>*)fieldData)->numItems(); i++){
                delete (NUMERIC_TYPE*)((LinkedList<NUMERIC_TYPE*>*)fieldData)->getValue(i);
              }

              delete (LinkedList<NUMERIC_TYPE*>*) fieldData;
              break;
          };
        }
      }
    };

    void addField(JSON_FIELD*);
    void appendToCurrentString(char);
    NUMERIC_TYPE* switchCurrentStringToNumber();
    NUMERIC_TYPE* stringToNumber(String*);
    byte charToInt(char);
    boolean isDigit(char);
    
    byte _state;
    LinkedList<JSON_FIELD*> _fields;
    JSON_FIELD* _currentField;
    String* _tmpValue;
    LinkedList<String*>* _arrayValuesStr;
    LinkedList<NUMERIC_TYPE*>* _arrayValuesNum;
    boolean _inStr;
};

JsonParser::JsonParser() {
  _fields = LinkedList<JSON_FIELD*>();
  _state = JPIS_BETWEEN_FIELDS;
  _currentField = NULL;
  _tmpValue = NULL;
  _inStr = false;
}

JsonParser::~JsonParser() {
  for(FIELD_INDEX_TYPE i=0; i<_fields.numItems(); i++){
    delete _fields.getValue(i);
  }
}

void JsonParser::addChar(char c){
  LinkedList<JSON_FIELD> test = LinkedList<JSON_FIELD>();
  
  //ignore whitespace unless in a string field value
  if(c == ' ' && _state != JPIS_FIELD_VALUE_STRING && !_inStr){
    return;
  }
  
  switch(_state){
    case JPIS_BETWEEN_FIELDS:
      if(c == '"'){
        _currentField = new JSON_FIELD;
        _state = JPIS_FIELD_NAME;
      }
      break;
    case JPIS_FIELD_NAME:
      if(c == '"'){
        _state = JPIS_BETWEEN_FIELD_NAME_VALUE;
      }else{
        _currentField->fieldName += c;
      }
      break;
    case JPIS_BETWEEN_FIELD_NAME_VALUE:
      if(c == '"'){
        //double quotes indicates the value of the current field being processed is a string
        _currentField->fieldType = JSON_STRING;
        _currentField->fieldData = new String;
        _state = JPIS_FIELD_VALUE_STRING;
      }else if(isDigit(c)){
        //current character being processed is a number, since there has not been a double quotes, 
        //the value of the current field being processed is numeric
        _currentField->fieldType = JSON_NUMBER;
        _currentField->fieldData = new String;
        appendToCurrentString(c);
        _state = JPIS_FIELD_VALUE_NUMBER;
      }else if(c == '['){
        //current field being processed has an array value, the next character will determine the array type, 
        _state = JPIS_ARRAY_START;
      }
      break;
    case JPIS_FIELD_VALUE_STRING:
      if(c == '"'){
        addField(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else{
        appendToCurrentString(c);
      }
      break;
    case JPIS_FIELD_VALUE_NUMBER:
      if(c == ',' || c == '}'){
        _currentField->fieldData = switchCurrentStringToNumber();
        addField(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else if(isDigit(c)){
        appendToCurrentString(c);
      }
      break;
    case JPIS_ARRAY_START:
      if(c == '"'){
        _state = JPIS_ARRAY_STRING;
        _currentField->fieldType = JSON_STRING_ARRAY;
        _tmpValue = new String();
        _arrayValuesStr = new LinkedList<String*>();
        _inStr = true;
      }else if(isDigit(c)){
        _state = JPIS_ARRAY_NUMBER;
        _currentField->fieldType = JSON_NUMBER_ARRAY;
        _tmpValue = new String();
        *(String*)_tmpValue += c;
        _arrayValuesNum = new LinkedList<NUMERIC_TYPE*>();
      }
      break;
    case JPIS_ARRAY_STRING:
      if(c == ','){
        _arrayValuesStr->addItem(_tmpValue);
        _tmpValue = new String();
      }else if(c == ']'){
        _arrayValuesStr->addItem(_tmpValue);
        _currentField->fieldData = _arrayValuesStr;
        addField(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else if(c != '"' && _inStr){
        *(String*)_tmpValue += c;
      }else if(c == '"'){
        _inStr = !_inStr;
      }
      break;
    case JPIS_ARRAY_NUMBER:
      if(c == ','){
        _arrayValuesNum->addItem(stringToNumber(_tmpValue));
        delete _tmpValue;
        _tmpValue = new String();
      }else if(c == ']'){
        _arrayValuesNum->addItem(stringToNumber(_tmpValue));
        delete _tmpValue;
        _currentField->fieldData = _arrayValuesNum;
        addField(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else if(isDigit(c)){
        *(String*)_tmpValue += c;
      }
      break;
  };
  
}

FIELD_INDEX_TYPE JsonParser::numFields() {
  return _fields.numItems();
}

String JsonParser::fieldName(FIELD_INDEX_TYPE index) {
  return _fields.getValue(index)->fieldName;
}

byte JsonParser::fieldType(FIELD_INDEX_TYPE index) {
  return _fields.getValue(index)->fieldType;
}

String JsonParser::getFieldString(FIELD_INDEX_TYPE field){
  return *((String*)_fields.getValue(field)->fieldData);
}

NUMERIC_TYPE JsonParser::getFieldNumber(FIELD_INDEX_TYPE field) {
  return *((NUMERIC_TYPE*)_fields.getValue(field)->fieldData);
}

/*
 * returns an array field as a string array
 * since the return value is created by making a 
 * deep copy of the array, the consumer is 
 * responsible for deleting the returned array
 */
String* JsonParser::getFieldStringArray(FIELD_INDEX_TYPE field) {
  LinkedList<String*>* fieldData;
  String* str;
  
  fieldData = (LinkedList<String*>*)_fields.getValue(field)->fieldData;
  str = new String[fieldData->numItems()];
  
  for(FIELD_INDEX_TYPE i=0; i<fieldData->numItems(); i++){
    str[i] = *fieldData->getValue(i);
  }
  
  return str;
}

/*
 * returns an array field as a NUMERIC_TYPE array
 * since the return value is created by making a 
 * deep copy of the array, the consumer is 
 * responsible for deleting the returned array
 */
NUMERIC_TYPE* JsonParser::getFieldNumberArray(FIELD_INDEX_TYPE field) {
  LinkedList<NUMERIC_TYPE*>* fieldData;
  NUMERIC_TYPE* arr;
  
  fieldData = (LinkedList<NUMERIC_TYPE*>*)_fields.getValue(field)->fieldData;
  arr = new NUMERIC_TYPE[fieldData->numItems()];
  
  for(FIELD_INDEX_TYPE i=0; i<fieldData->numItems(); i++){
    arr[i] = *fieldData->getValue(i);
  }
  
  return arr;
}

ARRAY_LENGTH_TYPE JsonParser::getArrayFieldLength(FIELD_INDEX_TYPE field) {
  switch(_fields.getValue(field)->fieldType){
    case JSON_STRING_ARRAY:
    case JSON_NUMBER_ARRAY:
      return ((LinkedList<void*>*)_fields.getValue(field)->fieldData)->numItems();
      break;
    default:
      return 0;
      break;
  };
}

void JsonParser::addField(JSON_FIELD* field) {
    /*Serial.print("Added field with name'");
    Serial.print(field->fieldName);
    Serial.println("'");
    Serial.print("And type/value: '");
    switch(field->fieldType){
      case JSON_STRING:
        Serial.print("JSON_STRING'/'");
        Serial.print(*(String*)field->fieldData);
        Serial.println("'");
        break;
      case JSON_NUMBER:
        Serial.print("JSON_NUMBER'/'");
        Serial.print(*(NUMERIC_TYPE*)field->fieldData);
        Serial.println("'");
        break;
      case JSON_STRING_ARRAY:
        Serial.println("JSON_STRING_ARRAY'");
        break;
      case JSON_NUMBER_ARRAY:
        Serial.println("JSON_NUMBER_ARRAY'");
        break;
    };
  */
  _fields.addItem(field);
}

/**
 * appends a single char to the current field's data, assumes the current field data points to a String
 * 
 * c - char to append
 */
void JsonParser::appendToCurrentString(char c) {
  *((String*)_currentField->fieldData) += c;
}

/**
 * converts a number stored in the _currentField variables field data as a string to a NUMERIC_TYPE
 * 
 * returns NUMERIC_TYPE* - pointer to a NUMERIC_TYPE containing the numeric value of the input string
 */
NUMERIC_TYPE* JsonParser::switchCurrentStringToNumber() {
  return stringToNumber((String*)_currentField->fieldData);
}

/**
 * converts a string to a number
 */
NUMERIC_TYPE* JsonParser::stringToNumber(String* str) {
  NUMERIC_TYPE* newNum = new NUMERIC_TYPE;
  NUMERIC_TYPE multiplier = 1;
  boolean isNegative = false;
  int i;
  
  *newNum = 0;
  
  if(str->length() > 0){
    i = 0;
    
    if(str->charAt(0) == '-'){
      isNegative = true;
      i++;
    }
    
    for(i=str->length()-1; i>=0; i--){
      *newNum += charToInt(str->charAt(i)) * multiplier;
      multiplier *= 10;
    }
    
    if(isNegative){
      *newNum *= -1;
    }
  }
  
  return newNum;
}

/*
 * takes a character that is a digit (does not verify input is a number), 
 * converts that character to a byte
 */
byte JsonParser::charToInt(char c) {
  return c - ASCII_ZERO;
}

/*
 * helper function to determine if a provided character 
 * is a digit 0 through 9
 */
boolean JsonParser::isDigit(char c) {
  return ASCII_ZERO <= (byte)c && (byte)c <= ASCII_NINE;
}

#endif

