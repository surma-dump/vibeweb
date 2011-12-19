package audio

import (
)

type Channel []int32

type File struct {
	SampleRate uint64
	Depth uint8
	Data []Channel
}

func (f *File) NumChannels() int {
	return len(f.Data)
}

func (f *File) Length() int {
	return len(f.Data[0])
}

func (f *File) ByteDepth() uint8 {
	return (f.Depth + 7)/8
}
