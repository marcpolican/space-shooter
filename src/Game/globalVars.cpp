#include <pch.h>
#include "globalvars.h"
#include "utils.h"

using namespace PewPew;

GlobalVars* GlobalVars::mSingletonPtr = 0;

//--------------------------------------------------------------------
GlobalVars::GlobalVars()
{}
//--------------------------------------------------------------------
GlobalVars::~GlobalVars()
{
    mVariants.clear();
}
//--------------------------------------------------------------------
GlobalVars* GlobalVars::getPtr()
{
    if (!mSingletonPtr)
        mSingletonPtr = new GlobalVars;

    return mSingletonPtr;
}
//--------------------------------------------------------------------
void GlobalVars::release()
{
    if (mSingletonPtr)
    {
        delete mSingletonPtr;
        mSingletonPtr = 0;
    }
}
//--------------------------------------------------------------------
bool GlobalVars::loadDataFromFile(const string& filename)
{
    //load xml file here
    TiXmlDocument doc (filename);
    if (doc.LoadFile())
    {
        TiXmlElement* root = doc.RootElement();
        TiXmlElement* elem = 0;
        string name, val;
        Var value;

        if (!root)
        {
            LogManager::getSingletonPtr()->logMessage("document root not found");
            return false;
        }
    
        elem = root->FirstChildElement();
        while (elem)
        {
            name = elem->Attribute("name");
            val = elem->Attribute("value");
            value.set(val);

            strToLower(name);
            mVariants.insert (make_pair (name, value));

            elem = elem->NextSiblingElement();
        }
    }
    return true;
}
//--------------------------------------------------------------------
Var* GlobalVars::getVar(const string& name)
{
    if (mVariants.size() == 0) return 0;

    string temp = name;
    strToLower(temp);
    MVars::iterator iter = mVariants.find(temp);

    //check if the variable is found
    if (iter == mVariants.end()) return 0;

    return &((*iter).second);
}
//--------------------------------------------------------------------
int GlobalVars::getInt(const string& name)
{
    Var* v = getVar(name);
    if (v) return v->getInt();

    return 0;
}
//--------------------------------------------------------------------
double GlobalVars::getDouble(const string& name)
{
    Var* v = getVar(name);
    if (v) return v->getDouble();

    return 0.0;
}
//--------------------------------------------------------------------
string GlobalVars::getString(const string& name)
{
    Var* v = getVar(name);
    if (v) return v->getString();

    return "";
}
//--------------------------------------------------------------------
void GlobalVars::set(const string& name, int integer)
{
    string temp = name;
    strToLower(temp);
    mVariants[temp].set(integer);
}
//--------------------------------------------------------------------
void GlobalVars::set(const string& name, double real)
{
    string temp = name;
    strToLower(temp);
    mVariants[temp].set(real);
}
//--------------------------------------------------------------------
void GlobalVars::set(const string& name, const string& str)
{
    string temp = name;
    strToLower(temp);
    mVariants[temp].set(str);
}

