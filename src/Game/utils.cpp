#include <pch.h>
#include "utils.h"

using namespace PewPew;

void PewPew::strToLower(string& str)
{
    transform (str.begin(), str.end(),    // source
               str.begin(),             // destination
               tolower);              // operation
}
//--------------------------------------------------------------------
double PewPew::strToDouble (const string& s)
{
	double value = 0;
	try
	{	
		value = lexical_cast <double> (s);
	}
	catch(bad_lexical_cast &)
	{
		value = 0;
	}
	return value;
}
//--------------------------------------------------------------------
bool PewPew::parseDouble3 (const string& s, double& val1, double& val2, double& val3)
{

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(", \t");
    tokenizer tok(s, sep);
    tokenizer::iterator iter;
	
	iter = tok.begin();
	val1 = strToDouble(*iter);

	++iter;
	if (iter != tok.end())
		val2 = strToDouble(*iter);
	else
		return false;

	++iter;
	if (iter != tok.end())
		val3 = strToDouble(*iter);
	else
		return false;

	return true;
}
//--------------------------------------------------------------------
double PewPew::lerp(double t, double start, double end)
{
	return (1.0-t) * start + t * end;
}
//--------------------------------------------------------------------
Vector3 PewPew::lerpV3(double t, const Vector3& start, const Vector3& end)
{
	Vector3 temp;
    
    temp.x = (1.0-t) * start.x + t * end.x;
    temp.y = (1.0-t) * start.y + t * end.y;
    temp.z = (1.0-t) * start.z + t * end.z;

    return temp;
}

