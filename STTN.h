#include "stdafx.h"

#ifndef STT_N
#define STT_N

struct PassThrough {
	SF_INFO sfinfo;
	SNDFILE *infile;
};


struct Samples {
	Samples(sf::Int16 const* ss, std::size_t count) {
		samples.reserve(count);
		std::copy_n(ss, count, std::back_inserter(samples));
	}
	Samples() {}
	std::vector<sf::Int16> samples;
};
typedef struct {
	const char* string;
	double cpu_time_overall;
} ds_result;

struct meta_word {
	std::string word;
	float start_time;
	float duration;
};

typedef struct {
	char*  buffer;
	size_t buffer_size;
	int    sample_rate;
} ds_audio_buffer;

class PlaybackRecorder;


class STT_NEOP {

private:
	int nbSilence = 20;
	const char * soundFile = { "audio.wav" };
	UINT fuSound;
	bool NotSilence = true;
	int  nb_silence = nbSilence;
	BOOL is_loaded = FALSE;
	//----------------------------------------
	char* model = "models/output_graph.pbmm";
	char* alphabet = "models/alphabet.txt";
	char* lm = "models/lm.binary";
	char* trie = "models/trie";
	char* audioFile = "audio.wav";
	ModelState* ctx;
	wchar_t  *Result;
	CEdit * EditMic;
	CEdit * EditApp;
	//----------------------------------------
	bool load_without_trie = false;

	bool show_times = false;

	bool has_versions = false;

	bool extended_metadata = false;

	bool json_output = false;

	int stream_size = 0;
	//----------------------------------------

	
	//----DS Hyper Parameters---------------
	float N_CEP = 26;
	float N_CONTEXT = 9;
	float BEAM_WIDTH = 500;
	float LM_ALPHA = 0.75f;
	float LM_BETA = 1.85f;
	//--------------------------------------
	
	//--------Libsoundio--------------------
	
	int playDevice = 0;
	const char* deviceName ;
	struct SoundIo *soundio;
	struct SoundIoDevice *device;
	//----------------------------------------
public:
	struct SoundIoOutStream *outstream;
	static float seconds_offset;
	static void write_sample_float64ne(char *ptr, double sample);
	int loadModal();
	BOOL isLoaded();
	void setDevice(const char* deviceName);
	void setSoundio(struct SoundIo *soundio);
	void initVoiceOutTextBox(CEdit * Mic, CEdit* App);
	//----------------------------------------
	static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max);
	void multiProcess(int outPutID);
	LPWSTR getResult();
	char* JSONOutput(Metadata* metadata);
	std::vector<meta_word> WordsFromMetadata(Metadata* metadata);
	char* metadataToString(Metadata* metadata);
	void ProcessFile(ModelState* context, const char* path, bool show_times, int outPutID);
	ds_audio_buffer GetAudioBuffer(const char* path);
	ds_result LocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, int aSampleRate, bool extended_output, bool json_output);
	void setAudio(char* audioFile);
	int getLine();
	void getLineNamesSoundIo( CListBox * listBox);
	void getLineNamesSFML( CListBox * listbox );
	//------------convert--------------------
	static char * StringToChar(std::string str);
	static LPCTSTR stringToLPCTSTR(std::string str);
	static LPCTSTR CStringToLPCTSTR(CString str);
	static std::string CStringToString(CString str);
	static CString StringToCString(std::string str);
	static std::string charToString(const char * str);
	static CString CharToCString(const char * str);
	static LPCTSTR charToLPCTSTR(const char * str);
	static std::string LPWSTRToString(LPWSTR lpwstr);
};

#endif 