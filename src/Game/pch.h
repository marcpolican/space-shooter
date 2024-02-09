#ifndef _PCH_H_
#define _PCH_H_

//precompiled header

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include <OIS/OIS.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "tinyxml/tinyxml.h"

using namespace boost;
using namespace Ogre;
using namespace std;
using namespace FMOD;


/// macro for getting the absolute value or a variable
#define FABS(x) (x<0) ? -x : x;

#endif