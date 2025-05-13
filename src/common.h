struct AudioSpectrumConfig
{
    // Audio capture parameters
    int sampleRate = 44100;
    int framesPerBuffer = 1024;
    int numChannels = 2;
    int deviceIndex = -1; // Default device

    // FFT parameters
    int fftSize = 2048;
    int overlapFactor = 2; // 50% overlap

    // Display parameters
    int numBins = 64;         // Number of frequency bins to display
    double minFreq = 20.0;    // Minimum frequency in Hz
    double maxFreq = 20000.0; // Maximum frequency in Hz
    int spectrumHeight = 20;  // Height of the spectrum visualization
};