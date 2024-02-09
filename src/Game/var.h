#ifndef _VAR_H_
#define _VAR_H_

#include "boost/variant.hpp"
#include <string>

using namespace boost;
using namespace std;

namespace PewPew
{

    /** An object that can hold either int, double,
	    std::string types. Conversion will be done if possible.
	    @remarks
		    This class uses boost::variant
    */
    class Var
    {
    protected:
	    /** The value */
	    variant <int, double, string> mValue;

	    int    toInt() const;
        double toDouble() const;
	    string toString() const;

    public:
	    Var();
	    Var(int integer);
	    Var(double real);
	    Var(const string& str);
	    Var(const char* str);
	    ~Var();

	    int    getInt() const;
	    double getDouble() const;
	    string getString() const;

	    void set(int integer);
	    void set(double real);
	    void set(const string& str);
    };

}

#endif