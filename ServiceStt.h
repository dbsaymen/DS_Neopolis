#pragma once
class PlaybackRecorder : public sf::SoundBufferRecorder {
private:
	int init_nbSilence = 12;
	int nb_silence = 12;
protected:
	bool onProcessSamples(sf::Int16 const* samples, std::size_t sampleCount) {
		SoundBufferRecorder::onProcessSamples(samples, sampleCount);
		printf("%d\t%d\n",*samples,nb_silence);
		if (*samples < 500 & *samples> -500) {
			nb_silence--;
		}
		else {
			nb_silence = init_nbSilence;
		}
		return true; // continue capture
	}
public:
	void set_NbSilence(int nbSilence) {
		init_nbSilence = nbSilence;
		nb_silence = nbSilence;
	}
	bool isSilent() {
		if (nb_silence < 0)
			return TRUE;
		else
			return FALSE;
	}
	void initNbSilence() {
		nb_silence = init_nbSilence;
	}
};

class SttThread
{
public:
	void set_STT(STT_NEOP * stt);
	void run();
	void Process();
	void Start();
	void Stop();
	BOOL isRunning();
	void setAudio(char* audioFile);
	void setDevice(std::string deviceName);
	const std::string & getDevice() const;
	~SttThread();

private:
	PlaybackRecorder * input;
	std::string deviceName;
	int i = 0;
	std::string ch;
	std::thread *t1;
	std::vector<std::thread> services;
	std::mutex lock;
	STT_NEOP * stt;
	BOOL start = FALSE;
};

