#include "stdafx.h"
#include "ServiceStt.h"



void SttThread::set_STT(STT_NEOP * stt)
{
	this->stt = stt;
}
void SttThread::Start()
{
	this->start = TRUE;
}
void SttThread::Stop()
{
	this->start = FALSE;
}
void SttThread::setDevice(std::string deviceName) 
{
	this->deviceName = deviceName;
	input->setDevice(deviceName);
}

void SttThread::run()
{
	 t1=new std::thread(&SttThread::Process, this);
}
void SttThread::Process()
{
	if (!sf::SoundRecorder::isAvailable()) 
	{
		MessageBox(
			NULL,
			(LPCWSTR)L"No recording device is detect",
			(LPCWSTR)L"STT Details",
			MB_CANCELTRYCONTINUE
		);
	}
	else {
		input = new PlaybackRecorder();
		input->setChannelCount(1);
		lock.lock();
		while (true) 
		{
			while (start) 
			{
				input->start(16000);
				while (!input->isSilent())
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				input->initNbSilence();
				input->stop();
				const sf::SoundBuffer & soundBf = input->getBuffer();
				ch = "audio" + std::to_string(i) + ".wav";
				soundBf.saveToFile(ch);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				setAudio((char *)ch.c_str());
				services.push_back(std::thread(&STT_NEOP::multiProcess, stt, 0));
				i++;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		lock.unlock();
	}
	
}

const std::string & SttThread::getDevice() const 
{
	return input->getDevice();
}

void SttThread::setAudio(char* audioFile)
{
	this->stt->setAudio(audioFile);
}

BOOL SttThread::isRunning()
{
	return this->start;
}

SttThread::~SttThread()
{
	t1->join();
}

