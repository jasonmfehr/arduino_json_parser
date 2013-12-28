/*

Copyright (C) 2013 Jason Fehr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.


Partial implementation of a json parser that operations on a 
stream of json data.  This parse can only parse whole numbers and 
whole number array values.  Both positive (no prefixed sign) or 
negative (prefixed minus sign) are handled.  It cannot handle 
nulls, booleans, strings, objects, floats, doubles, or spaces 
within field names.  Field names must be surrounded with double quotes.

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

#define JSON_FIELDNAME_DELIMETER '"'
#define JSON_FIELD_DELIMETER ','
#define JSON_ARRAY_BEGIN '['
#define JSON_ARRAY_END ']'
#define JSON_END '}'
#define ASCII_ZERO 48
#define ASCII_NINE 57
#define NEGATIVE_SIGN '-'

//definitions used for the internal tracking of types of JSON data
#define JSON_NUMBER 1
#define JSON_NUMBER_ARRAY 3

//definitions of the different internal states that the parser can be in
#define STATE_UNDEFINED 0
#define STATE_PARSING_FIELD_NAME 1
#define STATE_PARSING_ARRAY 2
#define STATE_PARSING_NUMBER 3
#define STATE_AFTER_ARRAY 4


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
    void startCurrentNumber();
    void finishCurrentNumber();
    
    byte _state;
    LinkedList<JSON_FIELD*> _fields;
    JSON_FIELD* _currentField;
    NUMERIC_TYPE* _tmpNum;
    LinkedList<NUMERIC_TYPE>* _arrayValuesNum;
    boolean _isNegative;
};

JsonParser::JsonParser() {
  _fields = LinkedList<JSON_FIELD*>();
  _state = STATE_UNDEFINED;
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

  if(c == JSON_FIELDNAME_DELIMETER){
    //field name is either starting or stopping
    if(_state != STATE_PARSING_FIELD_NAME){
      //new field detected
      _currentField = new JSON_FIELD;
      _state = STATE_PARSING_FIELD_NAME;
      startCurrentNumber();
    }else if(_state == STATE_PARSING_FIELD_NAME){
      //field name is done being parsed, assume the field is a number
      _state = STATE_PARSING_NUMBER;
    }
  }else if(_state == STATE_PARSING_FIELD_NAME){
    _currentField->fieldName += c;
  }else if(c == JSON_ARRAY_BEGIN){
    //starting an array
    _state = STATE_PARSING_ARRAY;
    _arrayValuesNum = new LinkedList<NUMERIC_TYPE>();
    _currentField->fieldType = JSON_NUMBER_ARRAY;
  }else if(c == JSON_ARRAY_END){
    //finishing an array
    finishCurrentNumber();
    _currentField->fieldData.numberArrayData = _arrayValuesNum->toArray();
    _currentField->arrayLength = _arrayValuesNum->numItems();
    delete _arrayValuesNum;
    _fields.addItem(_currentField);
    _state = STATE_AFTER_ARRAY;
  }else if(c == JSON_FIELD_DELIMETER && _state != STATE_AFTER_ARRAY){
    finishCurrentNumber();
    if(_state == STATE_PARSING_ARRAY){
      startCurrentNumber();
    }
  }else if(c == JSON_END && _state != STATE_AFTER_ARRAY){
    finishCurrentNumber();
  }else if(c == NEGATIVE_SIGN){
    _isNegative = true;
  }else if(isDigit(c)){
    *_tmpNum = *_tmpNum * 10 + charToNum(c);
  }
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
      return -1;
      break;
  };
}

boolean JsonParser::isFieldArray(FIELD_INDEX_TYPE field) {
  return _fields.getValue(field)->fieldType == JSON_NUMBER_ARRAY;
}

/*
 * takes a character that is a digit (does not verify input is a number), 
 * converts that character to a byte equal to the character's digit value
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

void JsonParser::startCurrentNumber() {
  _isNegative = false;
  _tmpNum = new NUMERIC_TYPE;
  *_tmpNum = 0;
}

void JsonParser::finishCurrentNumber() {
  if(_isNegative){
    *_tmpNum *= -1;
  }

  if(_state == STATE_PARSING_ARRAY){
    _arrayValuesNum->addItem(*_tmpNum);
  }else{
    _currentField->fieldData.numberData = *_tmpNum;
    _fields.addItem(_currentField);
  }
 
  delete _tmpNum;
}

#endif

