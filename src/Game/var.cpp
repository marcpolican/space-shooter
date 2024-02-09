#include <pch.h>
#include "var.h"
#include "boost/lexical_cast.hpp"

using namespace boost;
using namespace std;
using namespace PewPew;

/** Constructor: assigns mValue to 0 (int) */
Var::Var() : mValue(0)
{}
//--------------------------------------------------------------------
/** Constructor: assigns mValue to integer */
Var::Var(int integer) : mValue (integer)
{}
//--------------------------------------------------------------------
/** Constructor: assigns mValue to real */
Var::Var(double real) : mValue (real)
{}
//--------------------------------------------------------------------
/** Constructor: assigns mValue to str */
Var::Var(const string& str) : mValue (str)
{}
//--------------------------------------------------------------------
/** Destructor */
Var::~Var()
{
}
//--------------------------------------------------------------------
/** Handles the internal conversion to int */
int Var::toInt() const
{
	int val = 0;
	double real=0;

	try
	{
    	string s = boost::get<string> (mValue);
		try
		{
			val = lexical_cast <int> (s);
			return val;
		}
        catch (...) {}

		try
		{
			val = (int) (lexical_cast <double> (s));
			return val;
		}
        catch (...) {}

		return 0;
	}
	catch (...) {}

	try
	{
		real = boost::get<double> (mValue);
		return (int)real;
	}
	catch (...) {}

	return 0;
}
//--------------------------------------------------------------------
double Var::toDouble() const
{
	int val = 0;
	double real=0;

	try
	{
		string s = boost::get<string> (mValue);
		try
		{
			real = lexical_cast <double> (s);
			return real;
		}
		catch (...)
		{
			return 0;
		}
		return 0;
	}
	catch (...) {}

    //if still not successful get an int
	return (double)toInt();
}
//--------------------------------------------------------------------
/** Handles the internal conversion to string */
string Var::toString() const
{
	string s;
	try
	{
		s = lexical_cast <string> (boost::get<int> (mValue));
		return s;
	}
	catch (...) {}

	try
	{
		s = lexical_cast <string> (boost::get<double> (mValue));
		return s;
	}
	catch (...) {}

	return "";
}
//--------------------------------------------------------------------
/** Retrieves the variable as an integer.<br>
	If the value is a double, it truncates decimal places.<br>
	If the value is a string, the int value is returned
	if it is convertible to an integer
*/
int Var::getInt() const
{
	int val = 0;
	
	try
	{
		val = boost::get<int> (mValue);
	}
	catch (boost::bad_get e)
	{
		val = toInt();
	}

	return val;
}
//--------------------------------------------------------------------
double Var::getDouble() const
{
	double val = 0.0;
	
	try
	{
		val = boost::get<double> (mValue);
	}
	catch (boost::bad_get e)
	{
		val = toDouble();
	}

	return val;
}
//--------------------------------------------------------------------
string Var::getString() const
{
	string s;
	
	try
	{
		s = boost::get<string> (mValue);
	}
	catch (boost::bad_get e)
	{
		s = toString();
	}

	return s;
}
//--------------------------------------------------------------------
void Var::set(int integer)
{
	mValue = integer;
}
//--------------------------------------------------------------------
void Var::set(double real)
{
	mValue = real;
}
//--------------------------------------------------------------------
void Var::set(const string& str)
{
	mValue = str;
}
