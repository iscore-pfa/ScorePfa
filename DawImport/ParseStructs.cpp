
#include "ParseStructs.hpp"

int ClipEventLoader::loadNotes(
    QDomElement& keyTrack,
    std::vector<MidiNote>& notes,
    uint8_t midiKeyValue)
{
  QDomNodeList notesListXml = getList(keyTrack, PATH_NOTES);

  for (int i = 0; i < notesListXml.length(); i++)
  {
    QDomElement a = notesListXml.item(i).toElement();
    uint8_t velocity = stoi(getItemValue(a, NAME_VELOCITY).toStdString());
    double duration = stod(getItemValue(a, NAME_VELOCITY).toStdString());
    double time = stod(getItemValue(a, NAME_TIME).toStdString());

    notes.emplace_back(time, duration, midiKeyValue, velocity);
  }
  return 0;
}

void ClipEventLoader::loadClipAtributes(ClipEvent& clipEvent)
{
  double start
      = stod(getItemValue(clipXml, PATH_START_VALUE).toStdString());
  double end = stod(getItemValue(clipXml, PATH_END_VALUE).toStdString());
  double startRelative = stod(
      getItemValue(clipXml, PATH_STARTRELATIVE_VALUE).toStdString());
  double clipLength
      = stod(getItemValue(clipXml, PATH_LOOPEND_VALUE).toStdString());

  this->end = end;
  clipEvent.start = start;
  clipEvent.end = end;
  clipEvent.startRelative = startRelative;
  clipEvent.clipLength = clipLength;
}

void ClipEventLoader::operator()(MidiClipEvent& midiClipEvent)
{
  loadClipAtributes(midiClipEvent);

  QDomNodeList keyTrackListXml = getList(clipXml, PATH_KEY_TRACKS);

  for (int i = 0; i < keyTrackListXml.length(); i++)
  {
    QDomElement a = keyTrackListXml.item(i).toElement();
    uint8_t midiKeyValue = stoi(getItemValue(a, PATH_MIDIKEY_VALUE).toStdString());
    loadNotes(a, midiClipEvent.midiNotes, midiKeyValue);
  }
}

void ClipEventLoader::operator()(AudioClipEvent& audioClipEvent)
{
  loadClipAtributes(audioClipEvent);
  audioClipEvent.path = getAudioClipPath(clipXml);
}

void TrackLoader::loadTrack(Track& track)
{
  track.name = getItemValue(trackXml, PATH_TRACK_NAME).toStdString();
  QDomNodeList clipEventsXml = getClipEvents(trackXml);
  double trackLength = 0;

  for (int i = 0; i < clipEventsXml.length(); i++)
  {
    QDomElement a = clipEventsXml.item(i).toElement();
    if (trackType == MIDITRACK)
    {
      track.clipEvents.push_back(MidiClipEvent());
    }
    else if (trackType == AUDIOTRACK)
    {
      track.clipEvents.push_back(AudioClipEvent());
    }
    ClipEventLoader loader{a};
    std::visit(loader, track.clipEvents.back());
    trackLength = loader.end;
  }
  track.length = trackLength;
}

void TrackLoader::operator()(MidiTrack& track)
{
  trackType = MIDITRACK;
  loadTrack(track);
  track.VST = getItemValue(trackXml, PATH_VST_NAME).toStdString();
}

void TrackLoader::operator()(AudioTrack& track)
{
  trackType = AUDIOTRACK;
  loadTrack(track);
}

void AbletonDocumentLoader::loadTracks(
    QDomDocument& docXml,
    std::vector<std::variant<AudioTrack, MidiTrack>>& tracks,
    QString path,
    int trackType)
{

  QDomNodeList tracksXml = getList(docXml, path);
  std::cout << tracksXml.length() << std::endl;
  for (int i = 0; i < tracksXml.length(); i++)
  {
    if (trackType == AUDIOTRACK)
    {
      tracks.push_back(AudioTrack());
    }
    else if (trackType == MIDITRACK)
    {
      tracks.push_back(MidiTrack());
    }

    QDomElement a = tracksXml.item(i).toElement();
    std::visit(TrackLoader{a}, tracks.back());
  }
}

void AbletonDocumentLoader::loadAbletonDocument(
    QDomDocument& docXml,
    AbletonDocument& abletonDoc)
{
  abletonDoc.BPM = stod(getItemValue(docXml, PATH_BPM_VALUE).toStdString());
  loadTracks(docXml, abletonDoc.tracks, PATH_MIDI_TRACKS, MIDITRACK);
  loadTracks(docXml, abletonDoc.tracks, PATH_AUDIO_TRACKS, AUDIOTRACK);
}
