#ifndef _GLOBALVARS_H_
#define _GLOBALVARS_H_

#include "var.h"

namespace PewPew
{
    /// This class will hold values that are stored in the xml data files.
    /// Values are stored as a variant type and can be accessed thru
    /// a name defined in the xml file
    class GlobalVars
    {
    private:
        GlobalVars();

    protected:
        typedef map <string, Var> MVars;
        MVars mVariants;

        static GlobalVars* mSingletonPtr;        
      
    public:
        ~GlobalVars();

        bool loadDataFromFile(const string& filename);
        Var* getVar(const string& name);
        
        //use these setters and getters below rather than getVar(),
        //because it handles if the variable does not exist yet.
	    int    getInt(const string& name);
	    double getDouble(const string& name);
	    string getString(const string& name);

	    void set(const string& name, int integer);
	    void set(const string& name, double real);
	    void set(const string& name, const string& str);

        static GlobalVars* getPtr();
        static void release();
    };

    ///shortcut
    #define GLOBALS GlobalVars::getPtr()
}

#endif