#include "pch.h"
#include "audio.h"
#include "utils.h"


using namespace PewPew;

Audio::Audio() :
    mSystem(0)
{
}
//--------------------------------------------------------------------
Audio::~Audio()
{
    for (MSounds::iterator iter = mSounds.begin();
         iter != mSounds.end();
         iter++)
    {
        if ((*iter).second.fmodSound)
        {
            (*iter).second.fmodSound->release();
            (*iter).second.fmodSound = 0;
        }
    }

    if (mSystem) { mSystem->release(); mSystem = 0; }
}
//--------------------------------------------------------------------
bool Audio::init()
{
    FMOD_RESULT result;

    result = FMOD::System_Create(&mSystem);		// Create the main system object.
    if (result != FMOD_OK)
    {
        string message = "FMOD error! (" + StringConverter::toString(result) + ")";
        LogManager::getSingletonPtr()->logMessage(message, LML_CRITICAL);
        return false;
    }

    result = mSystem->init(500, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
    if (result != FMOD_OK)
    {
        string message = "FMOD error! (" + StringConverter::toString(result) + ")";
        LogManager::getSingletonPtr()->logMessage(message, LML_CRITICAL);
        return false;
    }

    LogManager::getSingletonPtr()->logMessage("FMOD Initialized");
    return true;
}
//--------------------------------------------------------------------
bool Audio::loadSoundsXml(const string& filename)
{
    LogManager* log = LogManager::getSingletonPtr();
    //load xml file here
    TiXmlDocument doc (filename);
    if (doc.LoadFile())
    {
        TiXmlElement* root = doc.RootElement();
        TiXmlElement* elem = 0;
        string name, filename;
        int stream=0;
        int isLoop=0;
        double volume = 0;

        if (!root)
        {
            log->logMessage("document root not found");
            return false;
        }
    
        elem = root->FirstChildElement();
        while (elem)
        {
            name = elem->Attribute("name");
            filename = elem->Attribute("filename");
            
            if (elem->QueryIntAttribute("stream", &stream) != TIXML_SUCCESS)
                stream = 0; //default to false;

            if (elem->QueryIntAttribute("loop", &isLoop) != TIXML_SUCCESS)
                isLoop = 0; //default to false;

            if (elem->QueryDoubleAttribute("volume", &volume) != TIXML_SUCCESS)
                volume = 1.0;

            strToLower(name);
            
            loadSound (name, filename, (stream==1) ? true: false,
                volume, (isLoop==1) ? true : false);

            elem = elem->NextSiblingElement();
        }

        return true;
    }

    log->logMessage("failed to open xml file: " + filename);
    return false;
}
//--------------------------------------------------------------------
bool Audio::loadSound(const string& name, const string& filename, bool isStream, double volume, bool isLoop)
{
    if (mSounds.find(name) == mSounds.end())
    {
        FMOD_RESULT result;

        if (isStream)
            result = mSystem->createStream(filename.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &(mSounds[name].fmodSound));
        else
            result = mSystem->createSound(filename.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &(mSounds[name].fmodSound));

        //check if we are successful
        if (result != FMOD_OK)
        {
            LogManager::getSingletonPtr()->logMessage("ERROR: failed to load sound file: " + filename);
            LogManager::getSingletonPtr()->logMessage("FMOD error code: " + string(FMOD_ErrorString(result)));
            return false;
        }

        mSounds[name].volume = volume;
        mSounds[name].isLoop = isLoop;
        return true;
    }
    else
    {
        LogManager::getSingletonPtr()->logMessage("WARNING: Duplicate sound found, ignoring this entry: " + name);
        return false;
    }
}
//--------------------------------------------------------------------
void Audio::play(const string& name, bool isLoop)
{
    string tempName(name);
    strToLower(tempName);

    MSounds::iterator iter = mSounds.find(tempName);
    if (iter != mSounds.end())
    {
        Sound* s = &(*iter).second;

        mSystem->playSound(FMOD_CHANNEL_FREE, s->fmodSound, true, &(s->channel));
        s->channel->setLoopCount((s->isLoop) ? -1 : 0);
        s->channel->setVolume(s->volume);
        s->channel->setPaused(false);
    }
}
//--------------------------------------------------------------------
void Audio::stop(const string& name)
{
    string tempName(name);
    strToLower(tempName);

    MSounds::iterator iter = mSounds.find(tempName);
    if (iter != mSounds.end())
    {
        Sound* s = &(*iter).second;
        s->channel->stop();
    }    
}
//--------------------------------------------------------------------
void Audio::pause(const string& name)
{
}
//--------------------------------------------------------------------
void Audio::update()
{
    mSystem->update();
}