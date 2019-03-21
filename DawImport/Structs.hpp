/**
  \file Structs.hpp
  \brief Structs annalogue to to those of Ableton

  *These structures act as an etermediary between Ableton objects and Score Objects
  */


#ifndef STRUCTS
#define STRUCTS

#include<stdlib.h>
#include<vector>

struct MidiNote{
  double start;
  double duration;
  uint8_t pitch;
  uint8_t velocity;
  MidiNote(double s, double d, uint8_t p, uint8_t v):start(s), duration(d), pitch(p), velocity(v){};

};


struct ClipEvent{
  double start;
  double end;
  double startRelative;
  double clipLength;
};

struct AudioClipEvent:ClipEvent{
  std::string path;
};


struct MidiClipEvent:ClipEvent{
  std::vector<MidiNote> midiNotes;
};

strcut Track{
  std::string trackName;
  std::vector<std::variant<AudioClipEvent, MidiClipEvent>> clipEvents;
}

struct MidiTrack:Track{
  //std::vector<MidiClipEvent> midiClipEvents;

};

struct AudioTrack:Track{
  //std::vector<AudioClipEvent> audioClipEvents;

};



struct AbletonDocument{
  std::vector<std::variant<AudioTrack, MidiTrack>> tracks;
};

#endif