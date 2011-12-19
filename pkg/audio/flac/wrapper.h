#include <FLAC/all.h>

#ifndef MIN
	#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

FLAC__StreamDecoderReadStatus ByteArray_ReadCallback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data);
FLAC__StreamDecoderSeekStatus ByteArray_SeekCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data);
FLAC__StreamDecoderTellStatus ByteArray_TellCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data);
FLAC__StreamDecoderLengthStatus ByteArray_LengthCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data);
FLAC__bool ByteArray_EofCallback(const FLAC__StreamDecoder *decoder, void *client_data);
FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
void MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
void ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
void Decode(void *c);
