#include"json.hpp"
#include<iostream>

int testtimes = 0;
int passtimes = 0;
int errtimes = 0;


template<typename Ty,typename Ty2>
inline void testbase(Ty except, Ty2 actual)
{
	++testtimes;
	if (except == actual)
		++passtimes;
	else
	{
		++errtimes;
	}
}

void testnull()
{
	JsonValue v;
	v.Parse("null");
	testbase(JSON_NULL, v.type);
	v.Parse(" \r\r\r\r\t\n \n   null");
	testbase(JSON_NULL, v.type);
	v.Parse("nul");
	testbase(JSON_UK, v.type);
}

void testtrue()
{
	JsonValue v;
	v.Parse("true");
	testbase(JSON_TRUE, v.type);
	v.Parse("  \t true      ");
	testbase(JSON_TRUE, v.type);
	v.Parse("truue");
	testbase(JSON_UK, v.type);
}

void testfalse()
{
	JsonValue v;
	v.Parse("false");
	testbase(JSON_FALSE, v.type);
	v.Parse("  \n false   ");
	testbase(JSON_FALSE, v.type);
}


void testnum()
{
	JsonValue v;
	v.Parse("-1.22222");
	testbase(JSON_NUMBER, v.type);
	testbase(-1.22222, v.GetNum());
	v.Parse("1.22222");
	testbase(1.22222, v.GetNum());
	v.Parse("0.5e1");
	testbase(5, v.GetNum());
	testbase(JSON_PARSE_ERR,v.Parse("01.5557e"));
	testbase(JSON_PARSE_ERR, v.Parse("1.2e"));
	testbase(JSON_PARSE_ERR, v.Parse("1."));
	testbase(JSON_PARSE_OK, v.Parse("1.0"));
}

void teststring()
{
	JsonValue v;
	testbase(JSON_PARSE_OK,v.Parse("\"\""));
	testbase("", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"HEllow   World\""));
	testbase("HEllow   World", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"Hello\\nWorld\""));
	testbase("Hello\nWorld", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\""));
	testbase("\" \\ / \b \f \n \r \t", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\u0024\""));
	testbase("\x24", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\u00A2\""));
	testbase("\xC2\xA2", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\u20AC\""));
	testbase("\xE2\x82\xAC", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\uD834\\uDD1E\""));
	testbase("\xF0\x9D\x84\x9E", v.GetString());
	testbase(JSON_PARSE_OK, v.Parse("\"\\ud834\\udd1e\""));
	testbase("\xF0\x9D\x84\x9E", v.GetString());
}

void testarray()
{
	JsonValue v;
	testbase(JSON_PARSE_OK, v.Parse("[]"));
	testbase(JSON_ARRAY, v.GetType());


	testbase(JSON_PARSE_OK, v.Parse("[  ]"));
	testbase(JSON_PARSE_OK, v.Parse("[null ,true, false,1234567,\"test\"]"));
	testbase(JSON_PARSE_OK, v.Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
	testbase(JSON_PARSE_OK, v.Parse("[null ,true, false,1234567,\"test\"]"));


}

void testobject()
{
	JsonValue v;
	testbase(JSON_PARSE_OK, v.Parse(" { ""\"n\" : null , ""\"f\" : false , ""\"t\" : true , ""\"i\" : 123 , ""\"s\" : \"abc\", ""\"a\" : [ 1, 2, 3 ],""\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"" } "));
	testbase(JSON_OBJECT, v.GetType());
}

void testall()
{
	testnull();
	testtrue();
	testfalse();
	testnum();
	teststring();
	testarray();
	testobject();
}

int main()
{
	testall();
	_STD cout << "test:  " << testtimes << _STD endl <<
				 "pass:  " << passtimes << _STD endl <<
				 "passrate:  " << (static_cast<double>(passtimes) / static_cast<double>(testtimes))*100 << " %";

}