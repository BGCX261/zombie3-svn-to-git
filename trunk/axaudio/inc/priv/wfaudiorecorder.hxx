#ifndef __AF_AUDIO_RECORDER_HXX__
#define __AF_AUDIO_RECORDER_HXX__

/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>

#include "priv/speexconverter.hxx"
#include "mycriticalsection.hxx"

class FileLog;

class WFAudioRecorder
{
public:
////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////
	typedef struct tagCodedData
	{
		tUInt32 codedsize;
		tByte* codeddata;
	}tCodedData;

	// \brief Constructor with basic config
	WFAudioRecorder(SpeexConverter* converter,FileLog* filelog = NULL);

	~WFAudioRecorder();
	
	void Shutdown();

	tUInt32 ReadRecord(tByte* buf, tUInt32 size);

	void SetInVolume(tUInt32 val);
	tUInt32 GetInVolume(void);
private:
////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////
	static void __stdcall WFAudioRecorder::WaveInProc(HWAVEIN hWaveIn,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);

	tBool Init();
	tBool Uninit();
	tInt32 IncrFreeBlockCount(void);
	tInt32 DecrFreeBlockCount(void);

	tByte* AllocateBlocks(tUInt32 count, tUInt32 size);
	void FreeBlocks(void);

	void ProcessRecordedFrame(tByte* record);
	void PrepareInHeaders(void);
	
	SpeexConverter* _converter;

	tUInt32 _current_block;
	tUInt32 _free_block_count;

	tCodedData* _recorded_data_blocks;
	tUInt32 _cur_write_recorded_data_block;
	tUInt32 _cur_read_recorded_data_block;
	CriticalSection _recorded_data_criticalsection;

	WAVEHDR* _blocks;
	HWAVEIN _device;

	BOOL _shutdown_requested;
	FileLog* _filelog;
};

#endif // __AF_AUDIO_RECORDER_HXX__