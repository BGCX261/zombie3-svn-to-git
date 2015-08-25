#ifndef __AUDIO_PLAYER_HXX__
#define __AUDIO_PLAYER_HXX__

#if defined(WIN32)
#pragma comment(lib,"winmm")
#else
#error "Unsupported OS"
#endif

#include "mytypes.h"
#include "mymutex.hxx"

class FileLog;

class EXPORT_AXAUDIO AudioPlayer
{
public:
	// \brief Constructor
	AudioPlayer(tUInt32 bits_sample,tUInt32 sample_sec,tUInt32 block_nb,tUInt32 block_size,tUInt32 channel_nb);

	// \brief Destructor
	virtual ~AudioPlayer();

	/**
	 * Write voice data into player's play queue for playback.
	 */
	virtual tInt32 WriteAudio(string fromendpoint,tByte* data,tUInt32 size) = 0;

protected:

	//
	friend class RecordHandler;
	

	virtual tInt32 Init() = 0;
	virtual tInt32 UnInit() = 0;

	virtual tByte* AllocateBlocks(tUInt32 count, tUInt32 size) = 0;

	virtual void FreeBlocks(void) = 0;

};

#endif	// __AUDIO_PLAYER_HXX__