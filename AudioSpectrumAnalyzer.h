#include "portaudio.h"
#include "fftw3.h"
#include "common.h"
#include <vector>

class AudioSpectrumAnalyzer
{
private:
    AudioSpectrumConfig config;
    PaStream *stream = nullptr;
    std::vector<float> audioBuffer;
    std::vector<float> processingBuffer;

    fftwf_plan fftPlan;
    float *fftIn;
    fftwf_complex *fftOut;
    std::vector<double> spectrum;

    int bufferPos = 0;
    int frameCount = 0;

public:
    AudioSpectrumAnalyzer(const AudioSpectrumConfig &cfg);

    ~AudioSpectrumAnalyzer();

    bool initialize();

    void listAudioDevices();

    bool start();

    void stop();

    void updateConfig(const AudioSpectrumConfig &newConfig);

    static int audioCallback(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo *timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData);

    void processAudio();

    void computeFFT();

    void displaySpectrum();
};