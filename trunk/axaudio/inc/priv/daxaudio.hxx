#pragma once

#include <windows.h>
#include <string>
#include <map>

#include "mytypes.h"

class DSPlayer;
class DSRecorder;
class SpeexConverter;
class ACMConverter;
struct SpeexJitter;
class FileLog;

using namespace std;

class DAXAudio
{
public:
////////////////////////////////////////////////////////////////////////
// typedef
////////////////////////////////////////////////////////////////////////
	typedef map<string,SpeexJitter*> tFromJitterMap;
	typedef tFromJitterMap::iterator tFromJitterMapIter;
	typedef tFromJitterMap::const_iterator tFromJitterMapConstIter;
	typedef pair<string,SpeexJitter*> tFromJitterPair;

	DAXAudio(void* hWnd,FileLog* filelog);

	~DAXAudio();

	tBool Start();
	void Stop();

	tBool StartRecord();
	tBool StartPlay();
	void StopRecord();
	void StopPlay();

	/**
	 * @brief:Save the incomming voice data and then write merged datas from jitters into palyer
	 */
	void RegisterFrom(string host,tUInt32 port);
	void UnregisterFrom(string host,tUInt32 port);
	tUInt32 GetBlockSize(void) const;

	tUInt32 WriteSound(string host,tUInt32 port, tByte* data, tUInt32 size);
	tUInt32 ReadSound(tByte* buf, tUInt32 bufsize);

	void SetOutVolume(tUInt32 val);
	tUInt32 GetOutVolume(void);
	void SetInVolume(tUInt32 val);
	tUInt32 GetInVolume(void);

private:
	DAXAudio(const DAXAudio& model);

	tBool Init();
	tBool Uninit();

	void GetMergedFramFromJitters(tByte* buf,tUInt32 bufsize);

	static void MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len);

	HWND _hWnd;
	DSPlayer* _player;
	DSRecorder* _recorder;
	SpeexConverter* _converter;
	//ACMConverter* _converter;

	/**
	 * For buffering the incomming voice data
	 */
	 tFromJitterMap _from_jitter_map;

	 FileLog* _filelog;
};