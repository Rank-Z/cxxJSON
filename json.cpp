#include"json.hpp"
#include<cassert>
#include<cctype>

void pass_whitespace(const char*& c)
{
	if (c == nullptr)
		return;
	const char* p = c;
	while (_STD isspace(*p))
		++p;
	c = p;
}

int base_parse(const char*& c, _STD string type)
{
	for (auto&t : type)
		if (t != *c)
			return JSON_PARSE_ERR;
		else
			++c;
	return JSON_PARSE_OK;
}

inline bool is1to9(const char ch)
{
	return (ch!='0'&& _STD isdigit(ch));
}

inline bool is0to9(const char ch)
{
	return _STD isdigit(ch);
}

int parse_number(const char*&c, double& result)
{
	const char* p = c;
	if (*p == '-')
		++p;
	if (*p == '0')
		++p;
	else
	{
		if (!is1to9(*p))
			return JSON_PARSE_ERR;
		for (++p; is0to9(*p); ++p);
	}
	if (*p == '.')
	{
		++p;
		if (!is0to9(*p))
			return JSON_PARSE_ERR;
		for (++p; is0to9(*p); ++p);
	}
	if (*p == 'e' || *p == 'E')
	{
		++p;
		if (*p == '+' || *p == '-')
			++p;
		if (!is0to9(*p))
			return JSON_PARSE_ERR;
		for (++p; is0to9(*p); ++p);//i don't know why it can be 0 at first number after e/E ,but JSON.org do this.
	}
	_STD string s{ c,static_cast<unsigned>(p - c) };
	result = stod(s, nullptr);
	c = p;
	return JSON_PARSE_OK;
}

int parse_array(const char*& c,JsonArray*& arr)
{
	arr = new JsonArray;
	return (*arr).arrParse(c);
}

int parse_object(const char*& c, JsonObject*& obj)
{
	obj = new JsonObject;
	return (*obj).objParse(c);
}

int parse_utf8(const char*& p, _STD string* str)
{
	unsigned int u1= 0;
	for (int i = 0; i != 4; ++i)
	{
		++p;
		u1 <<= 4;
		if (is0to9(*p))
			u1 |= *p - '0';
		else if (*p >= 'a'&&*p <= 'f')
			u1 |= *p - 'a' + 10;
		else if (*p >= 'A'&&*p <= 'F')
			u1 |= *p - 'A' + 10;
		else return JSON_PARSE_ERR;
	}

	if (u1 >= 0xD800 && u1 <= 0xDBFF)
	{
		if (*(p + 1) == '\\'&&*(p + 2) == 'u')
		{
			p += 2;
			unsigned int u2 = 0;
			for (int i = 0; i != 4; ++i)
			{
				++p;
				u2 <<= 4;
				if (is0to9(*p))
					u2 |= *p - '0';
				else if (*p >= 'a'&&*p <= 'f')
					u2 |= *p - 'a' + 10;
				else if (*p >= 'A'&&*p <= 'F')
					u2 |= *p - 'A' + 10;
				else return JSON_PARSE_ERR;
			}
			u1 = 0x10000 + ((u1 - 0xD800) <<10) + (u2 - 0xDC00);
		}
	}

	if (u1 <= 0x7F)
	{
		(*str).push_back(static_cast<char>(u1&0xFF));
	}
	else if (u1 < 0x800)
	{
		(*str).push_back(static_cast<char>(0xC0 | (u1 >> 6) & 0x1F));
		(*str).push_back(static_cast<char>(0x80 |  u1       & 0x3F));
	}
	else if (u1 < 0x10000)

	{
		(*str).push_back(static_cast<char>(0xE0 | (u1 >> 12) & 0xF));
		(*str).push_back(static_cast<char>(0x80 | (u1 >> 6)  & 0x3F));
		(*str).push_back(static_cast<char>(0x80 |  u1        & 0x3F));
	}
	else if (u1 < 0x110000)
	{
		(*str).push_back(static_cast<char>(0xF0 | (u1 >> 18) & 0x7));
		(*str).push_back(static_cast<char>(0x80 | (u1 >> 12) & 0x3F));
		(*str).push_back(static_cast<char>(0x80 | (u1 >> 6 ) & 0x3F));
		(*str).push_back(static_cast<char>(0x80 |  u1        & 0x3F));
	}
	else return JSON_PARSE_ERR;

	return JSON_PARSE_OK;
}

int parse_string(const char*& c, _STD string*& str)
{
	const char* p = c;
	str = new _STD string;
	++p;
	for (;;)
	{
		switch (*p)
		{

		case '\\':
			switch(*++p)
			{
			case '"':(*str).push_back('\"'); break;
			case '\\':(*str).push_back('\\'); break;
			case '/':(*str).push_back('/'); break;
			case 'b':(*str).push_back('\b'); break;
			case 'f':(*str).push_back('\f'); break;
			case 'n':(*str).push_back('\n'); break;
			case 'r':(*str).push_back('\r'); break;
			case 't':(*str).push_back('\t'); break;
			case 'u':
				if (int ret = parse_utf8(p, str))
					if (ret != JSON_PARSE_OK)
						return ret;
			}
			++p;
			break;

		case '"':
			c = p;
			return JSON_PARSE_OK;

		case '\0':return JSON_PARSE_ERR;

		default:
			if (static_cast<unsigned char>(*p) < 32)
				return JSON_PARSE_ERR;
			else
				(*str).push_back(*p);
			++p;
		}
	}
	return JSON_PARSE_OK;
}

int parse_string2(const char*& c, _STD string& str)
{
	const char* p = c;
	
	++p;
	for (;;)
	{
		switch (*p)
		{

		case '\\':
			switch (*++p)
			{
			case '"':(str).push_back('\"'); break;
			case '\\':(str).push_back('\\'); break;
			case '/':(str).push_back('/'); break;
			case 'b':(str).push_back('\b'); break;
			case 'f':(str).push_back('\f'); break;
			case 'n':(str).push_back('\n'); break;
			case 'r':(str).push_back('\r'); break;
			case 't':(str).push_back('\t'); break;
			case 'u':
				_STD string* sp = &str;
				if (int ret = parse_utf8(p, sp))
					if (ret != JSON_PARSE_OK)
						return ret;
			}
			++p;
			break;

		case '"':
			c = p;
			return JSON_PARSE_OK;

		case '\0':return JSON_PARSE_ERR;

		default:
			if (static_cast<unsigned char>(*p) < 32)
				return JSON_PARSE_ERR;
			else
				(str).push_back(*p);
			++p;
		}
	}
	return JSON_PARSE_OK;
}

int JsonValue::_Pares(const char* c, const char*& end)
{
	_Reset();
	if (c == nullptr)
		return JSON_PARSE_ERR;
	int ret;
	pass_whitespace(c);
	switch (*c)
	{

	case 'n':
		if ((ret = base_parse(c, "null")) == JSON_PARSE_OK)
			type = JSON_NULL;
		break;

	case 'f':
		if ((ret = base_parse(c, "false")) == JSON_PARSE_OK)
			type = JSON_FALSE;
		break;

	case 't':

		if ((ret = base_parse(c, "true")) == JSON_PARSE_OK)
			type = JSON_TRUE;
		break;

	case '"':
		if ((ret = parse_string(c, str)) == JSON_PARSE_OK)
		{
			type = JSON_STRING;
		}
		++c;
		break;

	case '[':
		if ((ret = parse_array(c, arr)) == JSON_PARSE_OK)
			type = JSON_ARRAY;
		break;

	case '{':
		if ((ret = parse_object(c, obj)) == JSON_PARSE_OK)
			type = JSON_OBJECT;
		break;

	default:
		if (*c == '-' || ((*c) >= '0' && (*c) <= '9'))
		{
			if ((ret = parse_number(c, n)) == JSON_PARSE_OK)
				type = JSON_NUMBER;
		}
		else ret = JSON_PARSE_ERR;
	}

	if (ret == JSON_PARSE_OK)
	{
		pass_whitespace(c);
		if (*c != '\0' && *c != ',' && *c != ']' && *c != '}')
		{
			ret = JSON_PARSE_ERR;
			type = JSON_UK;
			_Reset();
		}
	}
	else _Reset();
	end = c;
	return ret;
}

int JsonValue::Parse(const char* c)
{
	const char* end = nullptr;
	return _Pares(c, end);
}

JsonType JsonValue::GetType()
{
	return type;
}

void JsonValue::_Reset()
{
	if (type == JSON_STRING)
		delete str;
	else if (type == JSON_ARRAY)
		delete arr;
	else if (type == JSON_OBJECT)
		delete obj;

	type = JSON_UK;
}

_STD string JsonValue::stringgenerate(_STD string& str) const
{
	_STD string ret;
	ret.push_back('"');
	for (auto&t : str)
	{
		switch (t)
		{
		case '"':
			ret.push_back('\\');
			ret.push_back('"');
			break;
		case '\\':
			ret.push_back('\\');
			ret.push_back('\\');
			break;
		case '\b':
			ret.push_back('\\');
			ret.push_back('b');
			break;
		case '\f':
			ret.push_back('\\');
			ret.push_back('f');
			break;
		case '\n':
			ret.push_back('\\');
			ret.push_back('n');
			break;
		case '\r':
			ret.push_back('\\');
			ret.push_back('r');
			break;
		case '\t':
			ret.push_back('\\');
			ret.push_back('t');
			break;

		default:
			if (t < 32)
			{
				ret += "\\u00";
				unsigned tmp = t;
				if (tmp > 15)
				{
					ret.push_back('1');
					tmp -= 16;
				}
				else
					ret.push_back('0');
				static char carr[16]= { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
				ret.push_back(carr[tmp]);
			}
			else
				ret.push_back(t);
		}
	}
	ret.push_back('"');
	return ret;
}

_STD string JsonValue::Generate() const
{
	if (type == JSON_UK)
		return "";
	if (type == JSON_NULL)
		return "null";
	if (type == JSON_TRUE)
		return "true";
	if (type == JSON_FALSE)
		return "false";
	if (type == JSON_NUMBER)
		return _STD to_string(n);
	if (type == JSON_STRING)
		return _GenerateString();
	if (type == JSON_ARRAY)
		return _GenerateArray();
	else
		return _GenerateObject();
}

_STD string JsonValue::_GenerateString() const
{
	return stringgenerate(*str);
}

_STD string JsonValue::_GenerateArray() const
{
	_STD string ret;
	ret.push_back('[');
	int size = ((*arr).valarray).size();
	for (int i = 0; i != size; ++i)
	{
		if (i > 0)
			ret.push_back(',');
		ret += ((*arr).valarray[i]).Generate();
	}
	ret.push_back(']');
	return ret;
}

_STD string JsonValue::_GenerateObject() const
{
	_STD string ret;
	ret.push_back('{');
	int size = ((*obj).memberarray).size();
	for (int i = 0; i != size; ++i)
	{
		if (i > 0)
			ret.push_back(',');
		ret += stringgenerate((*obj).memberarray[i].key);
		ret.push_back(':');
		ret += (*obj).memberarray[i].v.Generate();
	}
	ret.push_back('}');
	return ret;
}



double& JsonValue::GetNum()
{
	assert(type == JSON_NUMBER);
	return n;
}

_STD string& JsonValue::GetString()
{
	assert(type == JSON_STRING);
	return *str;
}

int JsonValue::GetStringSize()
{
	assert(type == JSON_STRING);
	return (*str).size();
}

int JsonArray::arrParse(const char*& c)
{
	int ret;
	const char* p = c;
	++p;
	for (;;)
	{
		pass_whitespace(p);

		if (*p == ']')
		{
				c = ++p;
				return JSON_PARSE_OK;
		}
		else if (*p == ',')//if there is " [ 1 , , ,2] " ,ignore this json-data error
		{
			++p;
			pass_whitespace(p);
		}
		valarray.emplace_back();
		if ((ret = valarray.back()._Pares(p, p)) != JSON_PARSE_OK)
		{
			valarray.pop_back();
			return ret;
		}
	}
}

int JsonObject::objParse(const char*& c)
{
	int ret;
	const char* p = c;
	++p;
	pass_whitespace(p);
	if (*p == '}')
	{
		c = ++p;
		return JSON_PARSE_OK;
	}
	for (;;)
	{
		pass_whitespace(p);
		if (*p != '"')
		{
			return JSON_PARSE_ERR;
		}
		else
		{
			memberarray.emplace_back();
			if ((ret = parse_string2(p, memberarray.back().key)) != JSON_PARSE_OK)
			{
				memberarray.pop_back();
				return ret;
			}
			++p;
			pass_whitespace(p);
			if (*p != ':')
			{
				memberarray.pop_back();
				return JSON_PARSE_ERR;
			}
			else
			{
				++p;
				pass_whitespace(p);
				if ((ret = memberarray.back().v._Pares(p, p)) != JSON_PARSE_OK)
				{
					memberarray.pop_back();
					return ret;
				}
				pass_whitespace(p);
			}
		}
		if (*p == ',')
		{
			++p;
			continue;
		}
		else if (*p == '}')
		{
			c = ++p;
			return JSON_PARSE_OK;
		}
		else
		{
			return JSON_PARSE_ERR;
		}
	}
}

bool JsonValue::GetBool()
{
	assert(type == JSON_TRUE || type == JSON_FALSE);
	return type == JSON_TRUE;
}

int JsonValue::GetArraySize()
{
	assert(type == JSON_ARRAY);
	return (*arr).valarray.size();
}

JsonArray& JsonValue::GetArray()
{
	assert(type == JSON_ARRAY);
	return *arr;
}

int JsonValue::GetObjectSize()
{
	assert(type == JSON_OBJECT);
	return (*obj).memberarray.size();
}

JsonObject& JsonValue::GetObject()
{
	assert(type == JSON_OBJECT);
	return *obj;
}

JsonValue& JsonArray::operator[](int pos)
{
	assert(valarray.size() > pos);
	return valarray[pos];
}

const JsonValue& JsonArray::operator[](int pos)const
{
	assert(valarray.size() > pos);
	return valarray[pos];
}

JsonMember& JsonObject::operator[](int pos)
{
	assert(memberarray.size() > pos);
	return memberarray[pos];
}

const JsonMember& JsonObject::operator[](int pos)const
{
	assert(memberarray.size() > pos);
	return memberarray[pos];
}