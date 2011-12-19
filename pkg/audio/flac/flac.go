package flac

import (
	"audio"
	"io"
	"io/ioutil"
	"unsafe"
)

// // #cgo CFLAGS: -I../include -DVERSION="1.0"
// #cgo LDFLAGS: -logg -lFLAC
// #include <FLAC/all.h>
// #include "wrapper.h"
import "C"

type container struct {
	decoder *C.FLAC__StreamDecoder
	input struct {
		buffer []byte
		pos uint64
	}
	file *audio.File
}

func Decode(r io.Reader) (f *audio.File, e error) {
	f = &audio.File{}
	c := &container{
		file: f,
	}
	c.decoder = C.FLAC__stream_decoder_new()
	C.FLAC__stream_decoder_set_md5_checking(c.decoder, C.FLAC__bool(0))
	c.input.buffer, e = ioutil.ReadAll(r)
	if e != nil {
		return
	}
	C.Decode(unsafe.Pointer(c))
	return
}

//export createChannelBuffers
func createChannelBuffers(c unsafe.Pointer, num, size uint64) {
	s := (*container)(c)
	s.file.Data = make([]audio.Channel, num)
	for i := range s.file.Data {
		s.file.Data[i] = make(audio.Channel, size)
	}
}
