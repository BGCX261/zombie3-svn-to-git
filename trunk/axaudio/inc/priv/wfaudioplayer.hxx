#ifndef __WF_AUDIO_PLAYER_HXX__
#define __WF_AUDIO_PLAYER_HXX__

/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>
#include <map>

#include "mycriticalsection.hxx"

#if defined(WIN32)
#pragma comment(lib,"winmm")
#else
#error "Unsupported OS"
#endif

class SpeexConverter;
struct SpeexJitter;
class Event;
class FileLog;

class WFAudioPlayer
{
public:
/////////////////////////////////////////////////////////
// typedefs
/////////////////////////////////////////////////////////
	typedef struct tagInVoiceBuffer
	{
		tBool accessed;
		tByte* invoice;
	}tInVoiceBuffer;

	//typedef std::map<string,tInVoiceBuffer*> tHostBufferMap;
	typedef std::map<string,SpeexJitter*> tHostBufferMap;
	typedef tHostBufferMap::iterator tHostBufferMapIter;
	typedef tHostBufferMap::const_iterator tHostBufferMapConstIter;
	typedef std::pair<string,SpeexJitter*> tHostBufferMapPair;

	//\brief Constructor
	WFAudioPlayer(SpeexConverter* converter,FileLog* filelog = NULL);

	//\brief Descontructor
	~WFAudioPlayer();

	/**
	 * Write a frame of voice data into player
	 */
	void WriteSound(string fromendpoint,tByte* data,tUInt32 size);

	void SetOutVolume(tUInt32 val);
	tUInt32 GetOutVolume(void);

private:
	static void __stdcall WaveOutProc(HWAVEOUT hWaveOut,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);
		
	tBool Init();
	tBool Uninit();
	tByte* AllocateBlocks(tUInt32 count, tUInt32 size);
	void FreeBlocks(void);
	tInt32 IncrFreeBlockCount(void);
	tInt32 DecrFreeBlockCount(void);
	tByte* GetNextBlockFrame(void);

	/**
	 * move buffered data in Jitter into player if player is ready for playback.
	 */
	void MoveJitteredVoiceToPlayer(void);

	/**
	 * Get the first frame from all jitters,mix them into one frame.
	 */
	const tByte* GetMixedInVoiceFrame();
	static void MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len);

	SpeexConverter* GetConverter(void) const;
	//Event* GetContinueRecordEvent(void) const;
	//Event* GetContinuePlayEvent(void) const;

	tInt32 GetCurrentPos(void);

	SpeexConverter* _converter;

	tUInt32 _current_block;
	tUInt32 _free_block_count;
	CriticalSection _critical_section_on_free_block_count;

	//////////////////////////////////
	HWAVEOUT _hwaveout;
	WAVEHDR* _blocks;

	short _mixed_invoice_data[FRAME_SIZE];

	tHostBufferMap _host_buffer_map;
	tByte* _mixed_buffer;
	//Mutex* _playback_mutex;
	FileLog* _filelog;
};

#endif // __WF_AUDIO_PLAYER_HXX__