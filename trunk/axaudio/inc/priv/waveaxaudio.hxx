#ifndef __WAVE_AX_AUDIO_HXX__
#define __WAVE_AX_AUDIO_HXX__

#include "mytypes.h"

class SpeexConverter;
class WFAudioPlayer;
class WFAudioRecorder;
class Event;
class FileLog;

class WaveAXAudio
{
public:
	// \brief Constructor
	WaveAXAudio(FileLog* filelog);

	// \brief Destructor
	~WaveAXAudio();

////////////////////////////////////////////////////////
// Public Methods
////////////////////////////////////////////////////////
	void WriteSound(string fromendpoint,tByte* audio,tUInt32 size);
	tUInt32 ReadSound(tByte* buf,tUInt32 size);

	void SetOutVolume(tUInt32 val);
	tUInt32 GetOutVolume(void);

	void SetInVolume(tUInt32 val);
	tUInt32 GetInVolume(void);

private:
	tBool Init();
	tBool Uninit();

	SpeexConverter* _converter;
	WFAudioPlayer* _player;
	WFAudioRecorder* _recorder;

	FileLog* _filelog;
};


#endif	//__WAVE_AX_AUDIO_HXX__