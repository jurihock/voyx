# voyx

The project idea is essentially to harmonize the source vocal audio signal just in real time depending on the MIDI input.

Each MIDI key determines the particular pitch shifting amount relative to the detected fundamental frequency of the source signal segment.

The required multi pitch shifting feature is already implemented in the [stftPitchShift](https://github.com/jurihock/stftPitchShift) project.

Additional ideas:

* [Sliding DFT](https://quod.lib.umich.edu/cgi/p/pod/dod-idx/sliding-is-smoother-than-jumping.pdf?c=icmc;idno=bbp2372.2005.086;format=pdf) instead of STFT
* [Metal](https://developer.apple.com/metal/cpp) acceleration, especially for M1 macs
* [OpenCL](https://github.com/KhronosGroup/OpenCL-CLHPP) acceleration otherwise

Let's get started...
