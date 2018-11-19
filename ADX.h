#ifndef _ADX_HEADER_
#define _ADX_HEADER_


//	adx2wavmod3 をベースに一部変更


// 　以下オリジナルの著作権表示
/*
	adv2wavmod3

	(c)2001 BERO
    (c)2008 hcs

	http://www.geocities.co.jp/Playtown/2004/
	bero@geocities.co.jp

	adx info from: http://ku-www.ss.titech.ac.jp/~yatsushi/adx.html

*/

#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES		// for M_SQRT2
#include <math.h>

#include <mmsystem.h>

#include "File.h"
#include "WAV.h"


namespace imas
{

typedef struct {
	int s1,s2;
} PREV;

void convert( short *out, unsigned char *in, PREV *prev, int coef1, int coef2, unsigned int crypt_key )
{
	int scale =  (((in[0]<<8)|(in[1]))^crypt_key) + 1;

	int i;
	int s0, s1, s2, d;
//	int over=0;

	in += 2;
	s1 = prev->s1;
	s2 = prev->s2;
	for( i=0; i<16; i++)
	{
		d = in[i]>>4;
		if( d & 8 ) d-=16;
		s0 = d*scale + ((coef1*s1 + coef2*s2)>>12);
//		if (abs(s0)>32767) over=1;
		if( s0 > 32767 ) s0 = 32767;
		else if( s0 < -32768 ) s0 = -32768;
		*out++ = s0;
		s2 = s1;
		s1 = s0;

		d = in[i] & 15;
		if( d & 8 ) d -= 16;
		s0 = d*scale + ((coef1*s1 + coef2*s2)>>12);
//		if (abs(s0)>32767) over=1;
		if( s0 > 32767 ) s0 = 32767;
		else if( s0 < -32768 ) s0 =- 32768;
		*out++ = s0;
		s2 = s1;
		s1 = s0;
	}
	prev->s1 = s1;
	prev->s2 = s2;

//	if (over) putchar('.');
}


class ADX
{
public:
	WAV m_wav;

public:
	ADX() 
	{
		m_wav.wavhdr.totalsize = 0;
		m_wav.wavhdr.hdrsize = 0x10;
		m_wav.wavhdr.format = 1;
		m_wav.wavhdr.channel = 2;
		m_wav.wavhdr.freq = 44100;
		m_wav.wavhdr.byte_per_sec = 44100*2*2;
		m_wav.wavhdr.blocksize = 2*2;
		m_wav.wavhdr.bits = 16;
		m_wav.wavhdr.datasize = 0;

		memcpy(m_wav.wavhdr.hdr1, "RIFF", 4);
		memcpy(m_wav.wavhdr.hdr2, "WAVEfmt ", 8);
		memcpy(m_wav.wavhdr.hdr3, "data", 4);
	}

	bool Load(File* file)
	{
		unsigned char buf[18*2];
		short outbuf[32*2];
		int wsize;
		int coef1,coef2;
		PREV prev[2];

		file->SetEndianness( File::BIG_ENDIAN );

		if( file->ReadByte() != 0x80) return false;
		if( file->ReadByte() != 0x00) return false;

		int copyright_offset = file->ReadUInt16() - 2;
		file->Jump( copyright_offset );
		file->ReadByteArray( buf, 6 );

		if ( memcmp( buf, "(c)CRI", 6 )) return false;


		file->Jump( 0x07 );
		int channel = file->ReadByte();
		int freq = file->ReadInt32();
		int size = file->ReadInt32();

		file->Jump( 0x13 );
		int crypt_flag = file->ReadByte();	// 00: non-crypt, 08: crypt


		m_wav.wavhdr.channel = channel;
		m_wav.wavhdr.freq = freq;
		m_wav.wavhdr.blocksize = channel*sizeof(short);
		m_wav.wavhdr.byte_per_sec = freq * m_wav.wavhdr.blocksize;
		m_wav.wavhdr.datasize = size * m_wav.wavhdr.blocksize;
		m_wav.wavhdr.totalsize = m_wav.wavhdr.datasize + sizeof(m_wav.wavhdr)-8;

		m_wav.m_data.WriteByteArray( (unsigned char*) &m_wav.wavhdr, sizeof(m_wav.wavhdr));

		prev[0].s1 = 0;
		prev[0].s2 = 0;
		prev[1].s1 = 0;
		prev[1].s2 = 0;

		{
			double x,y,z,a,b,c;

			x = 500;
			y = freq;
			z = cos(2.0*M_PI*x/y);

			a = M_SQRT2-z;
			b = M_SQRT2-1.0;
			c = (a-sqrt((a+b)*(a-b)))/b;

			coef1 = floor(8192.0*c);
			coef2 = floor(-4096.0*c*c);
		}


		// a[n+1] = a[n] * xxxx + yyyy (mod 2^15)
		unsigned int crypt_key;
		unsigned int xxxx, yyyy;
		if(crypt_flag == 0)
		{
			crypt_key = 0;		// 初期値
			xxxx = 0;
			yyyy = 0;
		}else{		// 0x08
			crypt_key = 22637;
			xxxx = 23909;
			yyyy = 25579;
		}


		file->Jump( copyright_offset + 6 );

		if ( channel == 1 )
		{
			while(size)
			{
				file->ReadByteArray( buf, 18);

				convert( outbuf, buf, prev, coef1, coef2, crypt_key);		crypt_key = (crypt_key * xxxx + yyyy ) & 0x7fff;	// key更新
				
				if ( size > 32 ) wsize = 32; else wsize = size;
				size -= wsize;
				m_wav.m_data.WriteByteArray((unsigned char*) outbuf, wsize*2);
			}
		}
		else if ( channel == 2)
		{
			while(size)
			{
				short tmpbuf[32*2];
				int i;

				file->ReadByteArray(buf, 18*2);

				convert( tmpbuf, buf, prev, coef1, coef2, crypt_key);			crypt_key = (crypt_key * xxxx + yyyy ) & 0x7fff;	// key更新
				convert( tmpbuf+32, buf+18, prev+1, coef1, coef2, crypt_key);	crypt_key = (crypt_key * xxxx + yyyy ) & 0x7fff;	// key更新
				for(i=0; i<32; i++)
				{
					outbuf[i*2]   = tmpbuf[i];
					outbuf[i*2+1] = tmpbuf[i+32];
				}
				if (size > 32) wsize = 32; else wsize = size;
				size -= wsize;

				m_wav.m_data.WriteByteArray((unsigned char*) outbuf, wsize*2*2);
			}
		}

		return true;
	}

	bool Load(std::string infile)
	{
		File file;
		if( ! file.LoadFromFile(infile.c_str()) )
		{
			printf("Failed to open %s\n", infile.c_str());
			return false;
		}

		if( ! this->Load(&file) )
		{
			printf("Failed to load %s\n", infile.c_str());
			return false;
		}
		return true;
	}

	void Play()
	{	
		m_wav.Play();
	}

	void ExtractWave(char* filename)
	{
		m_wav.ExtractWav( filename );
	}

};

}	// namespace

#endif