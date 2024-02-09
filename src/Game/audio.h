#ifndef _AUDIO_H_
#define _AUDIO_H_

namespace PewPew
{
    typedef struct Sound
    {
        FMOD::Sound*   fmodSound;
        FMOD::Channel* channel;
        double         volume;
        bool           isLoop;

        Sound() : fmodSound(0), channel(0), volume(1.0), isLoop(false)
        {}
    } Sound;


    /// Audio subsystem using the FMOD api.
    class Audio
    {
    protected:
        typedef map <string, PewPew::Sound> MSounds;

        FMOD::System*   mSystem;
        MSounds         mSounds;
        
    public:
        Audio();
        ~Audio();
        bool init();
        bool loadSoundsXml(const string& filename);
        bool loadSound    (const string& name, const string& filename, bool isStream, double volume, bool isLoop);
        void play (const string& name, bool isLoop = false);
        void stop (const string& name);
        void pause(const string& name);
        void update();
    };
}

#endif