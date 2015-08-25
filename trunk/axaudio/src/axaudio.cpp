#include <iostream>		//cerr
#include "priv/waveaxaudio.hxx"
#include "priv/daxaudio.hxx"
#include "priv/dduplexaudio.hxx"
#include "axaudio.hxx"

AXAudio::AXAudio(void* arg1,FileLog* filelog)
{
	_impl = new DDuplexAudio(arg1,filelog);
}
AXAudio::~AXAudio()
{
	delete _impl;
}
tBool AXAudio::Start()
{
	_impl->Start();
	return TRUE;
}
void AXAudio::Stop()
{
	_impl->Shutdown();

	// Just stop it but not remove it.
	int counter=1000;
	while (--counter>0)
	{
		if (_impl->State()==Thread::FINISHED_STATE)
		{
			_impl->Join();
			break;
		}

		Thread::Yield();
	}
}
//tBool AXAudio::StartRecord()
//{
//	return _impl->StartRecord();
//}
//tBool AXAudio::StartPlay()
//{
//	return _impl->StartPlay();
//}
//void AXAudio::StopRecord()
//{
//	_impl->StopRecord();
//}
//void AXAudio::StopPlay()
//{
//	_impl->StopPlay();
//}
void AXAudio::RegisterFrom(string host,tUInt32 port)
{
	_impl->RegisterFrom(host,port);
}
void AXAudio::UnregisterFrom(string host,tUInt32 port)
{
	_impl->UnregisterFrom(host,port);
}
tUInt32 AXAudio::GetBlockSize(void) const
{
	return _impl->GetBlockSize();
}
tUInt32 AXAudio::WriteSound(string host,tUInt32 port, tByte* data, tUInt32 size)
{
	return _impl->WriteSound(host,port,data,size);
}
tUInt32 AXAudio::ReadSound(tByte* buf, tUInt32 bufsize)
{
	return _impl->ReadSound(buf,bufsize);
}

void AXAudio::SetOutVolume(tUInt32 val)
{
	_impl->SetOutVolume(val);
}
tUInt32 AXAudio::GetOutVolume(void)
{
	return _impl->GetOutVolume();
}

void AXAudio::SetInVolume(tUInt32 val)
{
	_impl->SetInVolume(val);
}
tUInt32 AXAudio::GetInVolume(void)
{
	return _impl->GetInVolume();
}
string AXAudio::GetWarning(void)
{
	return _impl->GetWarning();
}
AXAudio::AXAudio(const AXAudio& model)
{
	std::cerr<<"Not allowed to call this copy constructor,exit..."<<endl;
	exit(0);
}