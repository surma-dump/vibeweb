package wav

import (
	"audio"
	"io"
	"errors"
)

var (
	ErrUnsupportedDepth = errors.New("Unsupported depth")
)

func Encode(w io.Writer, f *audio.File) error {
	writeHeader(w, f)
	writeString(w, "data")
	writeUint32(w, uint32(f.Length()*int(f.ByteDepth())*f.NumChannels()))

	for sample := range f.Data[0] {
		for channel := range f.Data {
			value := f.Data[channel][sample]
			switch f.ByteDepth() {
				case 1:
					writeUint8(w, uint8(value))
				case 2:
					writeUint16(w, uint16(value))
				default:
					return ErrUnsupportedDepth
			}
		}
	}
	return nil
}

func EstimateFileSize(f *audio.File) uint64 {
	return 44+uint64(f.Length()*int(f.ByteDepth())*f.NumChannels())
}

func writeHeader(w io.Writer, f *audio.File) {
	writeString(w, "RIFF")
	writeUint32(w, uint32(36 + f.Length()*int(f.ByteDepth())*f.NumChannels()))
	writeString(w, "WAVE")
	writeString(w, "fmt ")
	writeUint32(w, 16)
	writeUint16(w, 1)
	writeUint16(w, uint16(f.NumChannels()))
	writeUint32(w, uint32(f.SampleRate))
	writeUint32(w, uint32(int(f.SampleRate) * f.NumChannels() * int(f.ByteDepth())))
	writeUint16(w, uint16(int(f.ByteDepth()) * f.NumChannels()))
	writeUint16(w, uint16(f.Depth))
}


func writeString(w io.Writer, s string) {
	w.Write([]byte(s))
}

func writeUint32(w io.Writer, v uint32) {
	for i := uint(0); i < 4; i++ {
		w.Write([]byte{byte((v>>(i*8)) & 0xFF)})
	}
}

func writeUint16(w io.Writer, v uint16) {
	for i := uint(0); i < 2; i++ {
		w.Write([]byte{byte((v>>(i*8)) & 0xFF)})
	}
}

func writeUint8(w io.Writer, v uint8) {
	w.Write([]byte{byte(v)})
}
