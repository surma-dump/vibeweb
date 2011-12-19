#include "wrapper.h"

#include <stdio.h>
#include "_cgo_export.h"


typedef struct {
	uint64_t SampleRate;
	uint8_t Depth;
	struct {
		struct {
			int32_t *pointer;
			int len;
			int cap;
		} *pointer;
		int len;
		int cap;
	} Data;
} AudioFile;

typedef struct {
	FLAC__StreamDecoder *dec;
	struct {
		struct {
			char *pointer;
			int len;
			int cap;
		} buffer;
		uint64_t pos;
	} input;
	AudioFile *file;
} container;

/*
 ad88888ba
d8"     "8b ,d
Y8,         88
`Y8aaaaa, MM88MMM 8b,dPPYba,  ,adPPYba, ,adPPYYba, 88,dPYba,,adPYba,
  `"""""8b, 88    88P'   "Y8 a8P_____88 ""     `Y8 88P'   "88"    "8a
        `8b 88    88         8PP""""""" ,adPPPPP88 88      88      88
Y8a     a8P 88,   88         "8b,   ,aa 88,    ,88 88      88      88
 "Y88888P"  "Y888 88          `"Ybbd8"' `"8bbdP"Y8 88      88      88

88                                            ad88
88              ,d                           d8"
88              88                           88
88 8b,dPPYba, MM88MMM ,adPPYba, 8b,dPPYba, MM88MMM
88 88P'   `"8a  88   a8P_____88 88P'   "Y8   88
88 88       88  88   8PP""""""" 88           88
88 88       88  88,  "8b,   ,aa 88           88 888
88 88       88  "Y888 `"Ybbd8"' 88           88 888
*/
FLAC__StreamDecoderReadStatus ByteArray_ReadCallback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data) {
	int i;
	container *s = (container *)client_data;

	*bytes = MIN(s->input.buffer.len - s->input.pos, *bytes);
	for(i = 0; i < *bytes; i++) {
		buffer[i] = s->input.buffer.pointer[s->input.pos+i];
	}
	s->input.pos += *bytes;
	if(s->input.pos >= s->input.buffer.len) {
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	}
	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus ByteArray_SeekCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data) {
	container *s = (container *)client_data;

	s->input.pos = absolute_byte_offset;
	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus ByteArray_TellCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data) {
	container *s = (container *)client_data;

	*absolute_byte_offset = s->input.pos;
	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus ByteArray_LengthCallback(const FLAC__StreamDecoder*decoder, FLAC__uint64 *stream_length, void *client_data) {
	container *s = (container *)client_data;

	*stream_length = s->input.buffer.len;
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool ByteArray_EofCallback(const FLAC__StreamDecoder *decoder, void *client_data) {
	container *s = (container *)client_data;

	return (s->input.pos >= s->input.buffer.len);
}

/*
88888888ba,
88      `"8b               ,d
88        `8b              88
88         88 ,adPPYYba, MM88MMM ,adPPYYba,
88         88 ""     `Y8   88    ""     `Y8
88         8P ,adPPPPP88   88    ,adPPPPP88
88      .a8P  88,    ,88   88,   88,    ,88
88888888Y"'   `"8bbdP"Y8   "Y888 `"8bbdP"Y8

                         ,d                                       ,d
                         88                                       88
 ,adPPYba, 8b,     ,d8 MM88MMM 8b,dPPYba, ,adPPYYba,  ,adPPYba, MM88MMM
a8P_____88  `Y8, ,8P'    88    88P'   "Y8 ""     `Y8 a8"     ""   88
8PP"""""""    )888(      88    88         ,adPPPPP88 8b           88
"8b,   ,aa  ,d8" "8b,    88,   88         88,    ,88 "8a,   ,aa   88,   888
 `"Ybbd8"' 8P'     `Y8   "Y888 88         `"8bbdP"Y8  `"Ybbd8"'   "Y888 888
*/

static uint64_t getSampleNumber(const FLAC__Frame *frame) {
	switch(frame->header.number_type) {
		case FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER:
			return (uint64_t)frame->header.number.frame_number * (uint64_t)frame->header.blocksize;
		case FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER:
			return frame->header.number.sample_number;
	}
	return (uint64_t)-1;
}

FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data) {
	container *s = (container *)client_data;
	uint64_t i, j;
	uint64_t start_sample = getSampleNumber(frame);

	for(i = 0; i < frame->header.channels; i++) {
		for(j = 0; j < frame->header.blocksize; j++) {
			uint64_t newj = start_sample + j;
			s->file->Data.pointer[i].pointer[newj] = buffer[i][j];
		}
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
	container *s = (container *)client_data;

	switch(metadata->type) {
		case FLAC__METADATA_TYPE_STREAMINFO: {
				const FLAC__StreamMetadata_StreamInfo *d = &(metadata->data.stream_info);
				s->file->SampleRate = d->sample_rate;
				s->file->Depth = d->bits_per_sample;
				createChannelBuffers(s, d->channels, d->total_samples);
		}
		break;
		case FLAC__METADATA_TYPE_PADDING:
		case FLAC__METADATA_TYPE_APPLICATION:
		case FLAC__METADATA_TYPE_SEEKTABLE:
		case FLAC__METADATA_TYPE_VORBIS_COMMENT:
		case FLAC__METADATA_TYPE_CUESHEET:
		case FLAC__METADATA_TYPE_PICTURE:
		case FLAC__METADATA_TYPE_UNDEFINED:
		default:
		break;
	}
}

void ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
}

void Decode(void *c) {
	container *s = (container *)c;

	FLAC__stream_decoder_init_stream(s->dec, ByteArray_ReadCallback, ByteArray_SeekCallback, ByteArray_TellCallback, ByteArray_LengthCallback, ByteArray_EofCallback, WriteCallback, MetadataCallback, ErrorCallback, s);
	FLAC__stream_decoder_process_until_end_of_stream(s->dec);
}
