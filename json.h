#ifndef JSON_HPP
#define JSON_HPP

#include<iostream>
#include<vector>
#include<string>

#define _STD ::std::

constexpr int JSON_PARSE_OK = 0;
constexpr int JSON_PARSE_ERR = 1;
constexpr int JSON_PARSE_STRING_ERR = 2;

enum JsonType
{
	JSON_STRING,
	JSON_NUMBER,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL,
	JSON_UK
};

struct JsonArray;
struct JsonObject;
struct JsonMember;

struct JsonValue
{

	JsonType type = JSON_NULL;
	union
	{
		_STD string* str;
		double n;
		JsonArray* arr;
		JsonObject* obj;
	};

	int Parse(const char* str);
	int _Pares(const char* str, const char*& end);
	void _Reset();
	_STD string stringgenerate(_STD string&) const;
	_STD string Generate() const;
	_STD string _GenerateString() const;
	_STD string _GenerateArray() const;
	_STD string _GenerateObject() const;

	JsonType GetType();
	bool GetBool();
	double& GetNum();
	_STD string& GetString();
	int GetStringSize();
	JsonArray& GetArray();

	int GetArraySize();
	int GetObjectSize();
	JsonObject& GetObject();
};

struct JsonArray
{
	_STD vector<JsonValue> valarray;

	int arrParse(const char*& str);
	JsonValue& operator[](int pos);
	const JsonValue& operator[](int pos)const;
};

struct JsonMember
{
	_STD string key;
	JsonValue v;
};

struct JsonObject
{
	_STD vector<JsonMember> memberarray;

	int objParse(const char*& c);
	JsonMember& operator[](int pos);
	const JsonMember& operator[](int pos)const;
};

#endif // !JSON_HPP
