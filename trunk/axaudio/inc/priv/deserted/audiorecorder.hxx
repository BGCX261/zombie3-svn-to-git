#ifndef __AUDIO_RECORDER_HXX__
#define __AUDIO_RECORDER_HXX__

#include "mydll.h"
#include "mytypes.h"

#include "mymutex.hxx"
#include "mythread.hxx"
#include "audioplayer.hxx"

class FileLog;

class EXPORT_AXAUDIO AudioRecorder : public Thread
{
public:
	// \brief Constructor
	AudioRecorder();

	AudioRecorder(tUInt32 bits_sample,tUInt32 sample_sec,tUInt32 block_nb,tUInt32 block_size,tUInt32 channel_nb,FileLog* filelog = NULL);

	// \brief Destructor
	virtual ~AudioRecorder();

	virtual tInt32 Init() = 0;
	//virtual tInt32 Start() = 0;
	//virtual tInt32 Pause() = 0;
	//virtual tInt32 Resume() = 0;
	//virtual tInt32 Stop() = 0;
	virtual tInt32 UnInit() = 0;

	virtual void Execute(Thread::Arg arg) = 0;

protected:
	//
	friend class RecordHandler;
	tInt32 IncrFreeBlockCount(void);
	tInt32 DecrFreeBlockCount(void);

	virtual tByte* AllocateBlocks(tUInt32 count, tUInt32 size) = 0;

	virtual void FreeBlocks(void) = 0;
	/* Configuration for recording */
	
	/* How many bits in a sample */
	tUInt32 _bits_per_sample;

	/* Number of samples picked per second */
	tUInt32 _samples_per_sec;

	/* Number of buffer block for recording */
	tUInt32 _block_nb;

	/* size of each buffer block */
	tUInt32 _block_size;

	tUInt32 _current_block;

	tUInt32 _free_block_count;

	/* always set to 1 or 2 channels */
	tUInt32 _channel_nb;

	Mutex _mutex_on_count;
	Mutex _mutex_on_resource;

	FileLog* _filelog;
};

#endif