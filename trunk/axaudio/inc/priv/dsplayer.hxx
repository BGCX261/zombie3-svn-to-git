#pragma once
#include <string>
//#include <map>
//#include <vector>
#include <DSound.h>		//DirectSound APIs
#include "mytypes.h"
#include "mythread.hxx"
#include "priv/dsutil.h"

using namespace std;

class SpeexConverter;
class FileLog;

class DSPlayer : public Thread
{
public:
/////////////////////////////////////////////////////////////////
// typedefs
/////////////////////////////////////////////////////////////////
	//typedef std::map<string,CStreamingSound*> tFromStreamSoundMap;
	//typedef tFromStreamSoundMap::iterator tFromStreamSoundMapIter;
	//typedef tFromStreamSoundMap::const_iterator tFromStreamSoundMapConstIter;
	//typedef pair<string,CStreamingSound*> tFromStreamSoundPair;

	//typedef std::map<HANDLE, CStreamingSound*> tHandleStreamSoundMap;
	//typedef tHandleStreamSoundMap::iterator tHandleStreamSoundMapIter;
	//typedef tHandleStreamSoundMap::const_iterator tHandleStreamSoundMapConstIter;
	//typedef std::pair<HANDLE,CStreamingSound*> tHandleStreamSoundPair;

	//typedef vector<HANDLE> tEventHandleVec;
	//typedef tEventHandleVec::iterator tEventHandleVecIter;

	DSPlayer(HWND hwnd,SpeexConverter* converter,FileLog* filelog);
	~DSPlayer();

	void Shutdown();

	tBool ReadyForPlayMore(void) const;

	tUInt32 WriteSound(tByte* data,tUInt32 size);

protected:
	virtual void Execute(Thread::Arg);
private:

	tBool Init();
	tBool Uninit();

	const tByte* GetLastPlayedFrame(void) const;
	const tByte* GetNextPlayFrame(void) const;

	HWND _hWnd;	// for SetCooperativeLevel(HWND,LEVEL) or you can only paly 8000 sample rate sound.

	//tFromStreamSoundMap _fromstreamsoundmap;
	//tHandleStreamSoundMap _handlestreamsoundmap;
	//tEventHandleVec _eventhandlevec;
	CSoundManager* _soundmanager;
	CStreamingSound* _streamsound;
	HANDLE _eventhandle;

	SpeexConverter* _converter;
	tBool _is_shutdown_requested;
	FileLog* _filelog;
};