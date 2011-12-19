package main

import (
	"net/http"
	"log"
	"flag"
	"fmt"
	"audio/flac"
	"audio/wav"
	"os"
)

var (
	root string
	helpFlag = flag.Bool("h", false, "Show this help")
)

func handler(w http.ResponseWriter, r *http.Request) {
	w.Header().Add("Access-Control-Allow-Origin", "*")
	f, e := os.Open(root+"/Foo Fighters/Foo Fighters/06 Floaty.flac")
	if e != nil {
		http.Error(w, "Could not read file: "+e.Error(), 500)
		return
	}
	defer f.Close()
	d, e := flac.Decode(f)
	fmt.Printf("Sample: %d Depth: %d\n", d.SampleRate, d.Depth)
	if e != nil {
		http.Error(w, "Could not decode file: "+e.Error(), 500)
		return
	}
	w.Header().Add("Content-Type", "audio/x-wav")
	w.Header().Add("Content-Length", fmt.Sprintf("%d", wav.EstimateFileSize(d)))
	wav.Encode(w, d)
}

func main() {
	flag.Parse()

	if *helpFlag  || flag.NArg() != 1 {
		fmt.Println("Usage: vibeweb <folder>")
		flag.PrintDefaults()
		return
	}
	root = flag.Arg(0)
	http.HandleFunc("/", handler)
	log.Fatal(http.ListenAndServe(":8080", nil))
}
