/*

Copyright (C) 2013 Jason Fehr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.


Implementation of a json parser that operates 
on a stream of json data.  There are a few 
limitations to what this parser can do.  It 
cannot parse objects, booleans, nulls, objects, 
or escaped charaters.  It also may have trouble 
handling spaces outside of string values.  It 
can parse numeric, string, numeric arrays, and 
string arrays.

Because of the arduino's limited exception handling, 
most safety checks have been removed.  For example, 
it will not complain if one of the getField functions 
is called with an argument that is greater than the 
number of fields.  Use carefully, you have been warned!

*/

#ifndef JSON_PROC
#define JSON_PROC

#include "LinkedList.h"

#define JSON_DELIMETER '"'
#define JSON_ARRAY_BEGIN '['
#define JSON_ARRAY_END ']'
#define ASCII_FIRST_NUMBER 48
#define ASCII_LAST_NUMBER 57

enum JSON_TYPE {
  JSON_STRING, 
  JSON_NUMBER, 
  JSON_STRING_ARRAY,
  JSON_NUMBER_ARRAY
};

enum JSON_PROCESSOR_INTERNAL_STATE {
  JPIS_BETWEEN_FIELDS,
  JPIS_BETWEEN_FIELD_NAME_VALUE,
  JPIS_FIELD_NAME, 
  JPIS_FIELD_VALUE_STRING,
  JPIS_FIELD_VALUE_NUMBER,
  JPIS_ARRAY_START,
  JPIS_ARRAY_STRING,
  JPIS_ARRAY_NUMBER
};

struct JSON_FIELD {
  String fieldName;
  void* fieldData;
  JSON_TYPE fieldType;
  
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
          delete (long*) fieldData;
          break;
        case JSON_STRING_ARRAY:
          delete (LinkedList<String*>*) fieldData;
          break;
        case JSON_NUMBER_ARRAY:
          delete (LinkedList<long*>*) fieldData;
          break;
      };
    }
  }
};

class JsonParser {
  
  public:
    JsonParser();
    ~JsonParser();
    void addChar(char);
    int numFields();
    String fieldName(int);
    JSON_TYPE fieldType(int);
    String getFieldString(int);
    long getFieldNumber(int);
    String* getFieldStringArray(int);
    long* getFieldNumberArray(int);
    int getArrayFieldLength(int);
    
  private:
    void addField(JSON_FIELD*);
    void appendToCurrentString(char);
    long* switchCurrentStringToNumber();
    long* stringToNumber(String*);
    int charToInt(char);
    boolean isDigit(char);
    
    JSON_PROCESSOR_INTERNAL_STATE _state;
    JSON_PROCESSOR_INTERNAL_STATE _prevState;
    LinkedList<JSON_FIELD*> _fields;
    JSON_FIELD* _currentField;
    String* _tmpValue;
    LinkedList<String*>* _arrayValuesStr;
    LinkedList<long*>* _arrayValuesNum;
    boolean _inStr;
};

JsonParser::JsonParser() {
  _fields = LinkedList<JSON_FIELD*>();
  _state = JPIS_BETWEEN_FIELDS;
  _prevState = JPIS_BETWEEN_FIELDS;
  _currentField = NULL;
  _tmpValue = NULL;
  _inStr = false;
}

JsonParser::~JsonParser() {
  for(int i=0; i<_fields.numItems(); i++){
    delete _fields[i];
  }
}

void JsonParser::addChar(char c){
  LinkedList<JSON_FIELD> test = LinkedList<JSON_FIELD>();
  
  //ignore whitespace unless in a string field value
  if(c == ' ' && _state != JPIS_FIELD_VALUE_STRING){
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
        _arrayValuesNum = new LinkedList<long*>();
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

int JsonParser::numFields() {
  return _fields.numItems();
}

String JsonParser::fieldName(int index) {
  return _fields.getValue(index)->fieldName;
}

JSON_TYPE JsonParser::fieldType(int index) {
  return _fields.getValue(index)->fieldType;
}

String JsonParser::getFieldString(int field){
  return *((String*)_fields.getValue(field)->fieldData);
}

long JsonParser::getFieldNumber(int field) {
  return *((long*)_fields.getValue(field)->fieldData);
}

String* JsonParser::getFieldStringArray(int field) {
  LinkedList<String*>* fieldData;
  String* str;
  
  fieldData = (LinkedList<String*>*)_fields.getValue(field)->fieldData;
  str = new String[fieldData->numItems()];
  
  for(int i=0; i<fieldData->numItems(); i++){
    str[i] = *fieldData->getValue(i);
  }
  
  return str;
}

long* JsonParser::getFieldNumberArray(int field) {
  LinkedList<long*>* fieldData;
  long* longArr;
  
  fieldData = (LinkedList<long*>*)_fields.getValue(field)->fieldData;
  longArr = new long[fieldData->numItems()];
  
  for(int i=0; i<fieldData->numItems(); i++){
    longArr[i] = *fieldData->getValue(i);
  }
  
  return longArr;
}

int JsonParser::getArrayFieldLength(int field) {
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
  //if(field != NULL){
    Serial.print("Added field with name'");
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
        Serial.print(*(long*)field->fieldData);
        Serial.println("'");
        break;
      case JSON_STRING_ARRAY:
        Serial.println("JSON_STRING_ARRAY'");
        break;
      case JSON_NUMBER_ARRAY:
        Serial.println("JSON_NUMBER_ARRAY'");
        break;
    };
  //}
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
 * converts a number stored in the _currentField variables field data as a string to a long
 * 
 * returns long* - pointer to a long containing the numeric value of the input string
 */
long* JsonParser::switchCurrentStringToNumber() {
  return stringToNumber((String*)_currentField->fieldData);
}

/**
 * converts a string to a number
 */
long* JsonParser::stringToNumber(String* str) {
  long* newNum = new long;
  long multiplier = 1;
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
 * provided a character that is a digit (does not verify input is a number), 
 * converts that character to an int
 */
int JsonParser::charToInt(char c) {
  return c - ASCII_FIRST_NUMBER;
}

/*
 * helper function to determine if a provided character 
 * is a digit 0 through 9
 */
boolean JsonParser::isDigit(char c) {
  return ASCII_FIRST_NUMBER <= (int)c && (int)c <= ASCII_LAST_NUMBER;
}

#endif

