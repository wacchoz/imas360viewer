#ifndef _WAV_HEADER_
#define _WAV_HEADER_

#include <mmsystem.h>

#include <vector>
#include "File.h"

// defined in mmsystem.h
/*
typedef struct wavehdr_tag {
    LPSTR       lpData;                 // pointer to locked data buffer
    DWORD       dwBufferLength;         // length of data buffer
    DWORD       dwBytesRecorded;        // used for input only
    DWORD_PTR   dwUser;                 // for client's use
    DWORD       dwFlags;                // assorted flags (see defines)
    DWORD       dwLoops;                // loop control counter
    struct wavehdr_tag FAR *lpNext;     // reserved for driver
    DWORD_PTR   reserved;               // reserved for driver
} WAVEHDR;

typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         // format type 
    WORD        nChannels;          // number of channels (i.e. mono, stereo...) 
    DWORD       nSamplesPerSec;     // sample rate 
    DWORD       nAvgBytesPerSec;    // for buffer estimation 
    WORD        nBlockAlign;        // block size of data 
    WORD        wBitsPerSample;     // number of bits per sample of mono data 
    WORD        cbSize;             // the count in bytes of the size of 
                                    // extra information (after cbSize) 
} WAVEFORMATEX;
*/


//
// このクラスは汎用性無いで流用せぬよう注意！！
//

struct WAV
{
public:
	struct {
		char hdr1[4];			// 4
		long totalsize;			// 8

		char hdr2[8];			// 16
		long hdrsize;			// 20
		short format;			// 22
		short channel;			// 24
		long freq;				// 28
		long byte_per_sec;		// 32
		short blocksize;		// 34
		short bits;				// 36

		char hdr3[4];			// 40
		long datasize;			// 44
	} wavhdr;					// WAVEファイルのヘッダ

	File m_data;	// ヘッダ含む

private:
	WAVEHDR m_whdr;			// for waveOurPrepareHeader/waveOurWrite
	WAVEFORMATEX m_wfe;		// for waveOurOpen

	HWAVEOUT hWaveOut;

public:
	WAV()
	{
		hWaveOut = NULL;
	}

	~WAV()
	{
		if(hWaveOut != NULL)
		{
			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &m_whdr, sizeof(WAVEHDR));
			waveOutClose(hWaveOut);
		}
	}

	void Append(WAV& wav)
	{
		if( m_data.FileSize() == 0 )
		{
			m_data = wav.m_data;

			wavhdr = wav.wavhdr;
		}
		else
		{
			m_data.JumpToLast();
			m_data.WriteByteArray( wav.m_data.pDataTop() + sizeof(wavhdr), wav.m_data.FileSize() - sizeof(wavhdr));	// ヘッダを除き連結

			wavhdr.datasize += wav.wavhdr.datasize;
			wavhdr.totalsize += wav.wavhdr.datasize;
		}

		m_data.SetEndianness( File::LITTLE_ENDIAN );
		m_data.Jump(4);	m_data.WriteInt32( wavhdr.totalsize );
		m_data.Jump(40);	m_data.WriteInt32( wavhdr.datasize );
	}

	void Play()
	{	
		m_wfe.wFormatTag		= WAVE_FORMAT_PCM;
		m_wfe.nChannels			= wavhdr.channel;
		m_wfe.wBitsPerSample	= 16;
		m_wfe.nBlockAlign		= m_wfe.nChannels * m_wfe.wBitsPerSample/8;
		m_wfe.nSamplesPerSec	= wavhdr.freq;
		m_wfe.nAvgBytesPerSec	= m_wfe.nSamplesPerSec * m_wfe.nBlockAlign;

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &m_wfe, 0, 0, CALLBACK_NULL);

		m_whdr.lpData = (LPSTR) m_data.pDataTop() + sizeof(wavhdr);
		m_whdr.dwBufferLength = wavhdr.datasize;
		m_whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
		m_whdr.dwLoops = 1;

		waveOutPrepareHeader(hWaveOut, &m_whdr, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, &m_whdr, sizeof(WAVEHDR));
	}


	void ExtractWav(char* filename)
	{
		m_data.WriteToFile( filename );
	}

};


#endif