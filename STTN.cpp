#include "stdafx.h"





char*  STT_NEOP::JSONOutput(Metadata* metadata)
	{
		std::vector<meta_word> words = WordsFromMetadata(metadata);

		std::ostringstream out_string;
		out_string << R"({"metadata":{"confidence":)" << metadata->probability << R"(},"words":[)";

		for (int i = 0; i < words.size(); i++) {
			meta_word w = words[i];
			out_string << R"({"word":")" << w.word << R"(","time":)" << w.start_time << R"(,"duration":)" << w.duration << "}";

			if (i < words.size() - 1) {
				out_string << ",";
			}
		}

		out_string << "]}\n";
		return _strdup(out_string.str().c_str());
	}

void STT_NEOP::multiProcess(int outPutID) {
	struct stat wav_info;
	if (0 != stat(audioFile, &wav_info)) {
		printf("Error on stat: %d\n", errno);
	}
	ProcessFile(ctx, audioFile, show_times, outPutID);
}

std::vector<meta_word> STT_NEOP::WordsFromMetadata(Metadata* metadata)
{
	std::vector<meta_word> word_list;

	std::string word = "";
	float word_start_time = 0;

	// Loop through each character
	for (int i = 0; i < metadata->num_items; i++) {
		MetadataItem item = metadata->items[i];

		// Append character to word if it's not a space
		if (strcmp(item.character, " ") != 0
			&& strcmp(item.character, u8"　") != 0) {
			word.append(item.character);
		}

		// Word boundary is either a space or the last character in the array
		if (strcmp(item.character, " ") == 0
			|| strcmp(item.character, u8" ") == 0
			|| i == metadata->num_items - 1) {

			float word_duration = item.start_time - word_start_time;

			if (word_duration < 0) {
				word_duration = 0;
			}

			meta_word w;
			w.word = word;
			w.start_time = word_start_time;
			w.duration = word_duration;

			word_list.push_back(w);

			// Reset
			word = "";
			word_start_time = 0;
		}
		else {
			if (word.length() == 1) {
				word_start_time = item.start_time; // Log the start time of the new word
			}
		}
	}
	return word_list;
}

char* STT_NEOP::metadataToString(Metadata* metadata)
{
	std::string retval = "";
	for (int i = 0; i < metadata->num_items; i++) {
		MetadataItem item = metadata->items[i];
		retval += item.character;
	}
	return _strdup(retval.c_str());
}

void STT_NEOP::ProcessFile(ModelState* context, const char* path, bool show_times, int outPutID)
{
	ds_audio_buffer audio = GetAudioBuffer(path);

	// Pass audio to DeepSpeech
	// We take half of buffer_size because buffer is a char* while
	// LocalDsSTT() expected a short*

	ds_result result = LocalDsSTT(context,
		(const short*)audio.buffer,
		audio.buffer_size / 2,
		audio.sample_rate,
		extended_metadata,
		json_output);
	free(audio.buffer);
	
	int cmp = strcmp(result.string, "");

	if (cmp) {
		std::string editBxTextSTR;
		CString editBxText;
		
		EditMic->GetWindowTextW(editBxText);
		editBxTextSTR = CStringToString(editBxText);
		editBxTextSTR += " \r\n";
		editBxTextSTR += charToString(result.string);
		EditMic->SetWindowTextW(stringToLPCTSTR(editBxTextSTR));
		DS_FreeString((char*)result.string);
	}

	if (show_times) {
		printf("cpu_time_overall=%.05f\n",
			result.cpu_time_overall);
	}
}
LPCTSTR STT_NEOP::CStringToLPCTSTR(CString str) {
	return stringToLPCTSTR(CStringToString(str));
}
CString STT_NEOP::CharToCString(const char * str) {
	return StringToCString(charToString(str));
}
std::string STT_NEOP::CStringToString(CString str) {
	CT2A ct(str);
	std::string strn(ct);
	return strn;
}
 char *  STT_NEOP::StringToChar(std::string str) {
	char * cstr;
	strcpy_s(cstr, str.length(),str.c_str());
	return cstr;
}
CString STT_NEOP::StringToCString(std::string str) {
	CString cstr(str.c_str(), str.length());
	return cstr;
}

ds_audio_buffer STT_NEOP::GetAudioBuffer(const char* path)
{
	ds_audio_buffer res = { 0 };


#ifndef NO_SOX
	// FIXME: Hack and support only 16kHz mono 16-bits PCM
	FILE* wave;
	fopen_s(&wave,path, "r");

	size_t rv;

	unsigned short audio_format;
	fseek(wave, 20, SEEK_SET); rv = fread(&audio_format, 2, 1, wave);

	unsigned short num_channels;
	fseek(wave, 22, SEEK_SET); rv = fread(&num_channels, 2, 1, wave);

	unsigned int sample_rate;
	fseek(wave, 24, SEEK_SET); rv = fread(&sample_rate, 4, 1, wave);

	unsigned short bits_per_sample;
	fseek(wave, 34, SEEK_SET); rv = fread(&bits_per_sample, 2, 1, wave);

	assert(audio_format == 1); // 1 is PCM
	assert(num_channels == 1); // MONO
	assert(sample_rate == 16000); // 16000 Hz
	assert(bits_per_sample == 16); // 16 bits per sample

	fseek(wave, 40, SEEK_SET); rv = fread(&res.buffer_size, 4, 1, wave);
	fseek(wave, 44, SEEK_SET);
	res.buffer = (char*)malloc(sizeof(char) * res.buffer_size);
	rv = fread(res.buffer, sizeof(char), res.buffer_size, wave);

	fclose(wave);
#endif // NO_SOX

#ifdef __APPLE__
	res.buffer_size = (size_t)(output->olength * 2);
	res.buffer = (char*)malloc(sizeof(char) * res.buffer_size);
	FILE* output_file = fopen(output_name, "rb");
	assert(fread(res.buffer, sizeof(char), res.buffer_size, output_file) == res.buffer_size);
	fclose(output_file);
	unlink(output_name);
#endif

	return res;
}

ds_result STT_NEOP::LocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, int aSampleRate, bool extended_output, bool json_output)
{
	ds_result res = { 0 };

	clock_t ds_start_time = clock();

	if (extended_output) {
		Metadata *metadata = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, aSampleRate);
		res.string = metadataToString(metadata);
		DS_FreeMetadata(metadata);
	}
	else if (json_output) {
		Metadata *metadata = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, aSampleRate);
		res.string = JSONOutput(metadata);
		DS_FreeMetadata(metadata);
	}
	else if (stream_size > 0) {
		StreamingState* ctx;
		int status = DS_SetupStream(aCtx, 0, aSampleRate, &ctx);
		if (status != DS_ERR_OK) {
			res.string = _strdup("");
			return res;
		}
		size_t off = 0;
		const char *last = nullptr;
		while (off < aBufferSize) {
			size_t cur = aBufferSize - off > stream_size ? stream_size : aBufferSize - off;
			DS_FeedAudioContent(ctx, aBuffer + off, cur);
			off += cur;
			const char* partial = DS_IntermediateDecode(ctx);
			if (last == nullptr || strcmp(last, partial)) {
				printf("%s\n", partial);
				last = partial;
			}
			else {
				DS_FreeString((char *)partial);
			}
		}
		if (last != nullptr) {
			DS_FreeString((char *)last);
		}
		res.string = DS_FinishStream(ctx);
	}
	else {
		res.string = DS_SpeechToText(aCtx, aBuffer, aBufferSize, aSampleRate);
	}

	clock_t ds_end_infer = clock();

	res.cpu_time_overall =
		((double)(ds_end_infer - ds_start_time)) / CLOCKS_PER_SEC;

	return res;
}

void STT_NEOP::write_callback(struct SoundIoOutStream *outstream,
	int frame_count_min, int frame_count_max) 
{
	PassThrough *pass = static_cast<PassThrough*>(outstream->userdata);
	if (!(pass == NULL)) {
		SNDFILE *infile = pass->infile;
		SF_INFO sfinfo = pass->sfinfo;
		int channels = sfinfo.channels;
		


		float float_sample_rate = sfinfo.samplerate;
		float seconds_per_frame = 1.0f / float_sample_rate;
		struct SoundIoChannelArea *areas;
		int frames_left = frame_count_max;
		int err;

		while (frames_left > 0) {
			int frame_count = frames_left;

			if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
				fprintf(stderr, "%s\n", soundio_strerror(err));
				exit(1);
			}

			if (!frame_count)
				break;

			float * buff = (float*)malloc(sizeof(float) * (frame_count*channels));
			int readcount;
			sf_count_t frames;

			frames = frame_count;

			readcount = sf_readf_float(infile, buff, frames);

			for (int frame = 0; frame < readcount; frame += 1) {
				for (int channel = 0; channel < channels; channel += 1) {
					float sample = buff[frame * 2 + channel];
					float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
					*ptr = sample;
				}
			}
			for (int frame = readcount; frame < frame_count; frame += 1) {
				for (int channel = 0; channel < channels; channel += 1) {
					float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
					*ptr = 0.0f;
				}
			}

			seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);

			if ((err = soundio_outstream_end_write(outstream))) {
				fprintf(stderr, "%s\n", soundio_strerror(err));
				exit(1);
			}
			frames_left -= frame_count;
		}
	}
}

float STT_NEOP::seconds_offset= 0.0f;

void STT_NEOP::setSoundio(struct SoundIo *soundio) 
{
	this->soundio = soundio;
}

void STT_NEOP::write_sample_float64ne(char *ptr, double sample) {
	double *buf = (double *)ptr;
	*buf = sample;
}

void STT_NEOP::setDevice(const char* deviceName) {
	this->deviceName = deviceName;
	playDevice = getLine();
	if (playDevice > -1) {
		device = soundio_get_output_device(soundio, playDevice);
		outstream = soundio_outstream_create(device);
		
		if (soundio_device_supports_format(device, SoundIoFormatFloat32NE)) {
			outstream->format = SoundIoFormatFloat32NE;
		}
		else if (soundio_device_supports_format(device, SoundIoFormatFloat64NE)) {
			outstream->format = SoundIoFormatFloat64NE;
		}
		else if (soundio_device_supports_format(device, SoundIoFormatS32NE)) {
			outstream->format = SoundIoFormatS32NE;
		}
		else if (soundio_device_supports_format(device, SoundIoFormatS16NE)) {
			outstream->format = SoundIoFormatS16NE;
		}

		this->outstream->write_callback = this->write_callback;
		soundio_outstream_open(outstream);
		soundio_outstream_start(outstream);
	}
	
}

int STT_NEOP::getLine()
{
	int output_count = soundio_output_device_count(soundio);
	int default_output = soundio_default_output_device_index(soundio);
	int LineOneDevice = -1;
	int cmp = 0;
	for (int i = 0; i < output_count; i += 1) {
		struct SoundIoDevice *device = soundio_get_output_device(soundio, i);
		cmp = strncmp(device->name, deviceName, strlen(deviceName));
		if (!cmp & (LineOneDevice < 0)) {
			LineOneDevice = i;
		}
		soundio_device_unref(device);
	}
	if (LineOneDevice < 0) {
		return default_output;
	}
	return LineOneDevice;
}

std::string STT_NEOP::LPWSTRToString(LPWSTR lpwstr) {
	std::string str = CW2A(lpwstr);
	return str;
}

void STT_NEOP::getLineNamesSFML(CListBox * listbox) {
	std::vector<std::string> ListDevices;
	ListDevices = sf::SoundRecorder::getAvailableDevices();
	
	for (std::string str : ListDevices) {
		/*wchar_t* Result = new wchar_t[strlen(str.c_str())];
		for (int i = 0; i < strlen(str.c_str()); i++)
			Result[i] = str.c_str()[i];*/
		listbox->AddString(stringToLPCTSTR(str));
	}
}
void STT_NEOP::getLineNamesSoundIo( CListBox * listBox)
{
	int output_count = soundio_output_device_count(soundio);
	for (int i = 0; i < output_count; i += 1) {
		struct SoundIoDevice *device = soundio_get_output_device(soundio, i);
		listBox->AddString(charToLPCTSTR(device->name));
		soundio_device_unref(device);
	}
}

BOOL STT_NEOP::isLoaded() {
	return is_loaded;
}

int STT_NEOP::loadModal()
{
	
	int status = DS_CreateModel(model, N_CEP, N_CONTEXT, alphabet, BEAM_WIDTH, &ctx);
	
	if (status != 0) {
		return 401;
	}
	if (lm && (trie || load_without_trie)) {
		int status = DS_EnableDecoderWithLM(ctx,
			alphabet,
			lm,
			trie,
			LM_ALPHA,
			LM_BETA);
		if (status != 0) {
			return 402;
		}
	}
	is_loaded = TRUE;
	return 0;
}

LPWSTR STT_NEOP::getResult() {
	return Result;
}

void STT_NEOP::setAudio(char* audioFile) {
	this->audioFile = audioFile;
}

void STT_NEOP::initVoiceOutTextBox(CEdit * Mic, CEdit* App) {
	this->EditMic = Mic;
	this->EditApp = App;
}


LPCTSTR STT_NEOP::stringToLPCTSTR(std::string str)
{
	std::wstring str2(str.length(), L' '); 
	std::copy(str.begin(), str.end(), str2.begin());
	const wchar_t * ch = str2.c_str();
	return (LPCTSTR)ch;
}
std::string STT_NEOP::charToString(const char * ch) {
	std::string str(ch);
	return str;
}

LPCTSTR STT_NEOP::charToLPCTSTR(const char * ch) {
	std::string str = charToString(ch);
	return stringToLPCTSTR(str);
}