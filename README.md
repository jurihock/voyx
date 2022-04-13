# voix

The project idea is essentially to harmonize the source vocal audio signal depending on the MIDI input just in real time.

Each MIDI note determines the particular pitch shifting amount relative to the detected fundamental frequency of the source signal segment.

The required multi pitch shift feature is already implemented in the [stftPitchShift](https://github.com/jurihock/stftPitchShift) project.
