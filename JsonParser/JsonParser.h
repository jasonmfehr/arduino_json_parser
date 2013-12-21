/*

Copyright (C) 2013 Jason Fehr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.


Partial implementation of a json parser that operations on a 
stream of json data.  This parse can only parse number and 
number array values.  Both positive (no prefixed sign) or 
negative (prefixed minus sign) are handled.  It cannot handle 
nulls, booleans, strings, objects, or spaces within field names.  

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

//if something other than an int needs to be used to store numbers, 
//change this macro to the desired type
#define NUMERIC_TYPE int

//defines the type of variable used to store the number of fields
//also determines the maximum number of fields that can be stored
#define FIELD_INDEX_TYPE byte

//defines the type of variable used to hold the number of items 
//in an array therefore determines the upper bounds on parsed arrays
#define ARRAY_LENGTH_TYPE byte

#define JSON_DELIMETER '"'
#define JSON_ARRAY_BEGIN '['
#define JSON_ARRAY_END ']'
#define ASCII_ZERO 48
#define ASCII_NINE 57
#define NEGATIVE_SIGN '-'

//definitions used for the internal tracking of types of JSON data
#define JSON_NUMBER 1
#define JSON_NUMBER_ARRAY 3

//definitions of the different internal states that the parser can be in
#define JPIS_BETWEEN_FIELDS 0
#define JPIS_BETWEEN_FIELD_NAME_VALUE 1
#define JPIS_FIELD_NAME 2
#define JPIS_FIELD_VALUE_STRING 3
#define JPIS_FIELD_VALUE_NUMBER 4
#define JPIS_ARRAY_START 5
#define JPIS_ARRAY_NUMBER 7


class JsonParser {
  
  public:
    JsonParser();
    ~JsonParser();
    void addChar(char);
    FIELD_INDEX_TYPE numFields();
    String fieldName(FIELD_INDEX_TYPE);
    NUMERIC_TYPE getFieldNumber(FIELD_INDEX_TYPE);
    NUMERIC_TYPE* getFieldNumberArray(FIELD_INDEX_TYPE);
    ARRAY_LENGTH_TYPE getArrayFieldLength(FIELD_INDEX_TYPE);
    boolean isFieldArray(FIELD_INDEX_TYPE);
    
  private:
    union FIELD_DATA {
      NUMERIC_TYPE numberData;
      NUMERIC_TYPE* numberArrayData;
    };

    struct JSON_FIELD {
      String fieldName;
      FIELD_DATA fieldData;
      byte fieldType;
      ARRAY_LENGTH_TYPE arrayLength;
    
      JSON_FIELD() {
        fieldData.numberData = NULL;
        fieldType = JSON_NUMBER;
        arrayLength = 0;
      }
    
      ~JSON_FIELD() {
        if(fieldType == JSON_NUMBER_ARRAY){
          delete fieldData.numberArrayData;
        }
      }
    };

    byte charToNum(char);
    boolean isDigit(char);
    
    byte _state;
    LinkedList<JSON_FIELD*> _fields;
    JSON_FIELD* _currentField;
    NUMERIC_TYPE* _tmpNum;
    LinkedList<NUMERIC_TYPE>* _arrayValuesNum;
    boolean _isNegative;
};

JsonParser::JsonParser() {
  _fields = LinkedList<JSON_FIELD*>();
  _state = JPIS_BETWEEN_FIELDS;
  _currentField = NULL;
}

JsonParser::~JsonParser() {
  for(FIELD_INDEX_TYPE i=0; i<_fields.numItems(); i++){
    delete _fields.getValue(i);
  }
}

void JsonParser::addChar(char c){
  LinkedList<JSON_FIELD> test = LinkedList<JSON_FIELD>();
  
  //ignore whitespace
  if(c == ' '){
    return;
  }
  
  switch(_state){
    case JPIS_BETWEEN_FIELDS:
      if(c == '"'){
        _currentField = new JSON_FIELD;
        _state = JPIS_FIELD_NAME;
        _isNegative = false;
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
      if(isDigit(c)){
        //current character being processed is a number, since there has not been a double quotes, 
        //the value of the current field being processed is numeric
        _currentField->fieldType = JSON_NUMBER;
        _currentField->fieldData.numberData = (NUMERIC_TYPE)charToNum(c);
        _state = JPIS_FIELD_VALUE_NUMBER;
      }else if(c == '['){
        //current field being processed has an array value, the next character will determine the array type, 
        _state = JPIS_ARRAY_START;
      }else if(c == NEGATIVE_SIGN){
        _isNegative = true;
      }
      break;
    case JPIS_FIELD_VALUE_NUMBER:
      if(c == ',' || c == '}'){
        if(_isNegative){
          _currentField->fieldData.numberData *= -1;
        }
        _fields.addItem(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else if(isDigit(c)){
        _currentField->fieldData.numberData = _currentField->fieldData.numberData * 10 + charToNum(c);
      }
      break;
    case JPIS_ARRAY_START:
      if(isDigit(c)){
        _state = JPIS_ARRAY_NUMBER;
        _currentField->fieldType = JSON_NUMBER_ARRAY;
        _arrayValuesNum = new LinkedList<NUMERIC_TYPE>();
        _tmpNum = new NUMERIC_TYPE;
        *_tmpNum = (NUMERIC_TYPE)charToNum(c);
      }else if(c == NEGATIVE_SIGN){
        _isNegative = true;
      }
      break;
    case JPIS_ARRAY_NUMBER:
      if(c == ','){
        if(_isNegative){
          *_tmpNum *= -1;
        }
        _arrayValuesNum->addItem(*_tmpNum);
        delete _tmpNum;
        _tmpNum = new NUMERIC_TYPE;
        *_tmpNum = 0;
      }else if(c == ']'){
        if(_isNegative){
          *_tmpNum *= -1;
        }
        _arrayValuesNum->addItem(*_tmpNum);
        _currentField->fieldData.numberArrayData = _arrayValuesNum->toArray();
        _currentField->arrayLength = _arrayValuesNum->numItems();
        delete _tmpNum;
        delete _arrayValuesNum;
        _fields.addItem(_currentField);
        _state = JPIS_BETWEEN_FIELDS;
      }else if(isDigit(c)){
        *_tmpNum = *_tmpNum * 10 + charToNum(c);
      }else if(c == NEGATIVE_SIGN){
        _isNegative = true;
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

NUMERIC_TYPE JsonParser::getFieldNumber(FIELD_INDEX_TYPE field) {
  return _fields.getValue(field)->fieldData.numberData;
}

NUMERIC_TYPE* JsonParser::getFieldNumberArray(FIELD_INDEX_TYPE field) {
  return _fields.getValue(field)->fieldData.numberArrayData;
}

ARRAY_LENGTH_TYPE JsonParser::getArrayFieldLength(FIELD_INDEX_TYPE field) {
  switch(_fields.getValue(field)->fieldType){
    case JSON_NUMBER_ARRAY:
      return _fields.getValue(field)->arrayLength;
      break;
    default:
      return 0;
      break;
  };
}

/*
 * takes a character that is a digit (does not verify input is a number), 
 * converts that character to a byte
 */
byte JsonParser::charToNum(char c) {
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

