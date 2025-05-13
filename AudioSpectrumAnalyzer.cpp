#include "portaudio.h"
#include "fftw3.h"
#include "common.h"
#include <vector>
#include <corecrt_math_defines.h>
#include <sstream>
#include <iomanip>

#include "AudioSpectrumAnalyzer.h"

class AudioSpectrumAnalyzer
{


AudioSpectrumAnalyzer(const AudioSpectrumConfig &cfg) : config(cfg)
    {
        // Initialize buffers
        audioBuffer.resize(config.framesPerBuffer * config.numChannels);
        processingBuffer.resize(config.fftSize);
        spectrum.resize(config.numBins);

        // Initialize FFTW
        fftIn = fftwf_alloc_real(config.fftSize);
        fftOut = fftwf_alloc_complex(config.fftSize / 2 + 1);
        fftPlan = fftwf_plan_dft_r2c_1d(config.fftSize, fftIn, fftOut, FFTW_MEASURE);
    }

    ~AudioSpectrumAnalyzer()
    {
        if (stream)
        {
            Pa_CloseStream(stream);
        }
        fftwf_destroy_plan(fftPlan);
        fftwf_free(fftIn);
        fftwf_free(fftOut);
        Pa_Terminate();
    }

    bool initialize()
    {
        // Initialize PortAudio
        PaError err = Pa_Initialize();
        if (err != paNoError)
        {
            std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }

        // List available devices if requested
        listAudioDevices();

        // Set up input parameters for loopback (system audio capture)
        PaStreamParameters inputParams;

        if (config.deviceIndex == -1)
        {
            // Use default device
            inputParams.device = Pa_GetDefaultOutputDevice(); // Default output for loopback
        }
        else
        {
            inputParams.device = config.deviceIndex;
        }

        if (inputParams.device == paNoDevice)
        {
            std::cerr << "No default output device." << std::endl;
            return false;
        }

        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(inputParams.device);
        std::cout << "Using device: " << deviceInfo->name << std::endl;

        inputParams.channelCount = config.numChannels;
        inputParams.sampleFormat = paFloat32;
        inputParams.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        inputParams.hostApiSpecificStreamInfo = nullptr;

        // Open stream
        err = Pa_OpenStream(
            &stream,
            &inputParams,
            nullptr, // No output parameters
            config.sampleRate,
            config.framesPerBuffer,
            paClipOff,
            audioCallback,
            this);

        if (err != paNoError)
        {
            std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }

        return true;
    }

    void listAudioDevices()
    {
        int numDevices = Pa_GetDeviceCount();
        std::cout << "Available audio devices:" << std::endl;

        for (int i = 0; i < numDevices; i++)
        {
            const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << i << ": " << deviceInfo->name
                      << " (Input channels: " << deviceInfo->maxInputChannels
                      << ", Output channels: " << deviceInfo->maxOutputChannels << ")" << std::endl;

            // Mark default devices
            if (i == Pa_GetDefaultInputDevice())
            {
                std::cout << "   * Default Input Device" << std::endl;
            }
            if (i == Pa_GetDefaultOutputDevice())
            {
                std::cout << "   * Default Output Device" << std::endl;
            }
        }

        std::cout << "\nNote: For system audio capture, select an output device with loopback capability.\n"
                  << std::endl;
    }

    bool start()
    {
        PaError err = Pa_StartStream(stream);
        if (err != paNoError)
        {
            std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        return true;
    }

    void stop()
    {
        if (stream)
        {
            Pa_StopStream(stream);
        }
    }

    void updateConfig(const AudioSpectrumConfig &newConfig)
    {
        // Stop the stream if it's running
        bool wasRunning = false;
        if (stream && Pa_IsStreamActive(stream))
        {
            wasRunning = true;
            stop();
            Pa_CloseStream(stream);
            stream = nullptr;
        }

        // Update the configuration
        config = newConfig;

        // Resize buffers
        audioBuffer.resize(config.framesPerBuffer * config.numChannels);
        processingBuffer.resize(config.fftSize);
        spectrum.resize(config.numBins);

        // Recreate FFT plan
        fftwf_destroy_plan(fftPlan);
        fftwf_free(fftIn);
        fftwf_free(fftOut);

        fftIn = fftwf_alloc_real(config.fftSize);
        fftOut = fftwf_alloc_complex(config.fftSize / 2 + 1);
        fftPlan = fftwf_plan_dft_r2c_1d(config.fftSize, fftIn, fftOut, FFTW_MEASURE);

        // Reinitialize and restart if needed
        if (initialize() && wasRunning)
        {
            start();
        }
    }

    static int audioCallback(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo *timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData)
    {
        AudioSpectrumAnalyzer *analyzer = static_cast<AudioSpectrumAnalyzer *>(userData);
        const float *in = static_cast<const float *>(inputBuffer);

        if (in == nullptr)
        {
            // No input data
            return paContinue;
        }

        // Copy input data to the analyzer's audio buffer
        for (unsigned long i = 0; i < framesPerBuffer * analyzer->config.numChannels; i++)
        {
            analyzer->audioBuffer[i] = in[i];
        }

        // Process the audio
        analyzer->processAudio();

        return paContinue;
    }

    void processAudio()
    {
        // Mix down to mono if we have multiple channels
        for (int i = 0; i < config.framesPerBuffer; i++)
        {
            float sampleSum = 0.0f;
            for (int ch = 0; ch < config.numChannels; ch++)
            {
                sampleSum += audioBuffer[i * config.numChannels + ch];
            }
            float monoSample = sampleSum / config.numChannels;

            // Add to processing buffer with overlap
            processingBuffer[bufferPos] = monoSample;
            bufferPos++;

            // When processing buffer is filled, perform FFT
            if (bufferPos >= config.fftSize)
            {
                computeFFT();

                // Move data for overlap
                int hopSize = config.fftSize / config.overlapFactor;
                for (int j = 0; j < config.fftSize - hopSize; j++)
                {
                    processingBuffer[j] = processingBuffer[j + hopSize];
                }
                bufferPos = config.fftSize - hopSize;

                // Increment frame count
                frameCount++;
            }
        }
    }

    void computeFFT()
    {
        // Apply Hann window function
        for (int i = 0; i < config.fftSize; i++)
        {
            double windowVal = 0.5 * (1 - std::cos(2 * M_PI * i / (config.fftSize - 1)));
            fftIn[i] = processingBuffer[i] * windowVal;
        }

        // Execute FFT
        fftwf_execute(fftPlan);

        // Calculate magnitude spectrum and map to display bins
        std::vector<double> rawSpectrum(config.fftSize / 2 + 1);

        // Calculate raw spectrum
        for (int i = 0; i < config.fftSize / 2 + 1; i++)
        {
            double real = fftOut[i][0];
            double imag = fftOut[i][1];
            rawSpectrum[i] = std::sqrt(real * real + imag * imag);
        }

        // Map to logarithmic frequency bins
        for (int i = 0; i < config.numBins; i++)
        {
            // Calculate logarithmic frequency for this bin
            double ratio = std::pow(config.maxFreq / config.minFreq, 1.0 / config.numBins);
            double freqLow = config.minFreq * std::pow(ratio, i);
            double freqHigh = config.minFreq * std::pow(ratio, i + 1);

            // Convert frequencies to FFT indices
            int indexLow = static_cast<int>(freqLow * config.fftSize / config.sampleRate);
            int indexHigh = static_cast<int>(freqHigh * config.fftSize / config.sampleRate);

            // Ensure indices are in valid range
            indexLow = std::max(1, std::min(indexLow, config.fftSize / 2));
            indexHigh = std::max(1, std::min(indexHigh, config.fftSize / 2));

            // Average the values in the range
            double sum = 0.0;
            for (int j = indexLow; j <= indexHigh; j++)
            {
                sum += rawSpectrum[j];
            }
            spectrum[i] = sum / (indexHigh - indexLow + 1);
        }

        // Apply normalization and log scale for better visualization
        double maxVal = 1e-10; // Avoid division by zero
        for (int i = 0; i < config.numBins; i++)
        {
            if (spectrum[i] > maxVal)
                maxVal = spectrum[i];
        }

        for (int i = 0; i < config.numBins; i++)
        {
            spectrum[i] = std::max(0.0, 20 * std::log10(spectrum[i] / maxVal));
            spectrum[i] = (spectrum[i] + 96) / 96;                   // Normalize to 0-1 range (assuming -96dB floor)
            spectrum[i] = std::max(0.0, std::min(1.0, spectrum[i])); // Clamp to 0-1
        }

        // Display the spectrum
        displaySpectrum();
    }

    void displaySpectrum()
    {
        // Only update display every few frames to avoid flickering
        if (frameCount % 3 != 0)
            return;

        // Clear screen (ANSI escape code)
        std::cout << "\033[2J\033[H";

        // Print frequency range headers
        std::cout << "Spectrum Analyzer - " << config.sampleRate << " Hz Sample Rate, "
                  << "FFT Size: " << config.fftSize << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl
                  << std::endl;

        // Print frequency markers
        for (int i = 0; i < config.numBins; i += config.numBins / 8)
        {
            double freq = config.minFreq * std::pow(config.maxFreq / config.minFreq,
                                                    static_cast<double>(i) / config.numBins);
            std::string freqLabel;
            if (freq < 1000)
            {
                {
                    std::ostringstream oss;
                    oss << static_cast<int>(freq) << "Hz";
                    freqLabel = oss.str();
                }
            }
            else
            {
                freqLabel = std::to_string(static_cast<int>(freq / 1000)) + "kHz";
            }

            int position = i * 80 / config.numBins;
            std::cout << std::setw(position) << freqLabel << " ";
        }
        std::cout << std::endl;

        // Draw spectrum
        for (int y = 0; y < config.spectrumHeight; y++)
        {
            double level = 1.0 - static_cast<double>(y) / config.spectrumHeight;

            for (int x = 0; x < config.numBins; x++)
            {
                if (spectrum[x] >= level)
                {
                    std::cout << "█";
                }
                else
                {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }

        // Print dB scale
        std::cout << "-96dB ";
        for (int i = 1; i < 8; i++)
        {
            int db = -96 + i * 12;
            std::cout << std::setw(8) << db << "dB ";
        }
        std::cout << " 0dB" << std::endl;
    }
}