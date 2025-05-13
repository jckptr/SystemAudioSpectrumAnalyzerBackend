// #include <iostream>
// #include "portaudio.h"
// #include "AudioSpectrumAnalyzer.h"

// // #ifdef _WINDOWS
// // #include <Windows.h>
// // #endif

// int main(int argc, char *argv[])
// {
// // #ifdef _WINDOWS
// //     SetConsoleCP(CP_UTF8);
// //     system("chcp 65001>nul"); 
// // #endif

//     std::cout << "Hello" << std::endl;

//     // Initialize PortAudio just to list devices
//     Pa_Initialize();
//     int numDevices = Pa_GetDeviceCount();
//     std::cout << "Available audio devices:" << std::endl;

//     for (int i = 0; i < numDevices; i++)
//     {
//         const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
//         std::cout << i << ": " << deviceInfo->name
//                   << " (Input channels: " << deviceInfo->maxInputChannels
//                   << ", Output channels: " << deviceInfo->maxOutputChannels << ")" << std::endl;

//         // Mark default devices
//         if (i == Pa_GetDefaultInputDevice())
//         {
//             std::cout << "   * Default Input Device" << std::endl;
//         }
//         if (i == Pa_GetDefaultOutputDevice())
//         {
//             std::cout << "   * Default Output Device" << std::endl;
//         }
//     }
//     Pa_Terminate();
//     return 0;
// }


// #include "RtAudio.h"
// #include <iostream>
// #include <map>

// void usage( void ) {
//   // Error function in case of incorrect command-line
//   // argument specifications
//   std::cout << "\nuseage: audioprobe <apiname> <nRepeats>\n";
//   std::cout << "    where apiname = an optional api (ex., 'core', default = all compiled),\n";
//   std::cout << "    and nRepeats = an optional number of times to repeat the device query (default = 0),\n";
//   std::cout << "                   which can be used to test device (dis)connections.\n\n";
//   exit( 0 );
// }

// std::vector< RtAudio::Api > listApis()
// {
//   std::vector< RtAudio::Api > apis;
//   RtAudio :: getCompiledApi( apis );

//   std::cout << "\nCompiled APIs:\n";
//   for ( size_t i=0; i<apis.size(); i++ )
//     std::cout << i << ". " << RtAudio::getApiDisplayName(apis[i])
//               << " (" << RtAudio::getApiName(apis[i]) << ")" << std::endl;

//   return apis;
// }

// void listDevices( RtAudio& audio )
// {
//   RtAudio::DeviceInfo info;

//   std::cout << "\nAPI: " << RtAudio::getApiDisplayName(audio.getCurrentApi()) << std::endl;

//   std::vector<unsigned int> devices = audio.getDeviceIds();
//   std::cout << "\nFound " << devices.size() << " device(s) ...\n";

//   for (unsigned int i=0; i<devices.size(); i++) {
//     info = audio.getDeviceInfo( devices[i] );

//     std::cout << "\nDevice Name = " << info.name << '\n';
//     std::cout << "Device Index = " << i << '\n';
//     std::cout << "Output Channels = " << info.outputChannels << '\n';
//     std::cout << "Input Channels = " << info.inputChannels << '\n';
//     std::cout << "Duplex Channels = " << info.duplexChannels << '\n';
//     if ( info.isDefaultOutput ) std::cout << "This is the default output device.\n";
//     else std::cout << "This is NOT the default output device.\n";
//     if ( info.isDefaultInput ) std::cout << "This is the default input device.\n";
//     else std::cout << "This is NOT the default input device.\n";
//     if ( info.nativeFormats == 0 )
//       std::cout << "No natively supported data formats(?)!";
//     else {
//       std::cout << "Natively supported data formats:\n";
//       if ( info.nativeFormats & RTAUDIO_SINT8 )
//         std::cout << "  8-bit int\n";
//       if ( info.nativeFormats & RTAUDIO_SINT16 )
//         std::cout << "  16-bit int\n";
//       if ( info.nativeFormats & RTAUDIO_SINT24 )
//         std::cout << "  24-bit int\n";
//       if ( info.nativeFormats & RTAUDIO_SINT32 )
//         std::cout << "  32-bit int\n";
//       if ( info.nativeFormats & RTAUDIO_FLOAT32 )
//         std::cout << "  32-bit float\n";
//       if ( info.nativeFormats & RTAUDIO_FLOAT64 )
//         std::cout << "  64-bit float\n";
//     }
//     if ( info.sampleRates.size() < 1 )
//       std::cout << "No supported sample rates found!";
//     else {
//       std::cout << "Supported sample rates = ";
//       for (unsigned int j=0; j<info.sampleRates.size(); j++)
//         std::cout << info.sampleRates[j] << " ";
//     }
//     std::cout << std::endl;
//     if ( info.preferredSampleRate == 0 )
//       std::cout << "No preferred sample rate found!" << std::endl;
//     else
//       std::cout << "Preferred sample rate = " << info.preferredSampleRate << std::endl;
//   }
// }

// int main(int argc, char *argv[])
// {
//   std::cout << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;

//   std::vector< RtAudio::Api > apis = listApis();

//   // minimal command-line checking
//   if (argc > 3 ) usage();
//   unsigned int nRepeats = 0;
//   if ( argc > 2 ) nRepeats = (unsigned int) atoi( argv[2] );

//   char input;
//   for ( size_t api=0; api < apis.size(); api++ ) {
//     if (argc < 2 || apis[api] == RtAudio::getCompiledApiByName(argv[1]) ) {
//       RtAudio audio(apis[api]);
//       for ( size_t n=0; n <= nRepeats; n++ ) {
//         listDevices(audio);
//         if ( n < nRepeats ) {
//           std::cout << std::endl;
//           std::cout << "\nWaiting ... press <enter> to repeat.\n";
//           std::cin.get(input);
//         }
//       }
//     }
//   }

//   return 0;
// }

/******************************************/
/*
  record.cpp
  by Gary P. Scavone, 2007

  This program records audio from a device and writes it to a
  header-less binary file.  Use the 'playraw', with the same
  parameters and format settings, to playback the audio.
*/
/******************************************/

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cstdint>
/*
typedef int8_t MY_TYPE;
#define FORMAT RTAUDIO_SINT8
*/

typedef int16_t MY_TYPE;
#define FORMAT RTAUDIO_SINT16

/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24

typedef int32_t MY_TYPE;
#define FORMAT RTAUDIO_SINT32

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
*/

// Platform-dependent sleep routines.
#if defined( WIN32 )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications
  std::cout << "\nuseage: record N fs <duration> <device> <channelOffset>\n";
  std::cout << "    where N = number of channels,\n";
  std::cout << "    fs = the sample rate,\n";
  std::cout << "    duration = optional time in seconds to record (default = 2.0),\n";
  std::cout << "    device = optional device index to use (default = 0),\n";
  std::cout << "    and channelOffset = an optional channel offset on the device (default = 0).\n\n";
  exit( 0 );
}

unsigned int getDeviceIndex( std::vector<std::string> deviceNames )
{
  unsigned int i;
  std::string keyHit;
  std::cout << '\n';
  for ( i=0; i<deviceNames.size(); i++ )
    std::cout << "  Device #" << i << ": " << deviceNames[i] << '\n';
  do {
    std::cout << "\nChoose a device #: ";
    std::cin >> i;
  } while ( i >= deviceNames.size() );
  std::getline( std::cin, keyHit );  // used to clear out stdin
  return i;
}

struct InputData {
  MY_TYPE* buffer;
  unsigned long bufferBytes;
  unsigned long totalFrames;
  unsigned long frameCounter;
  unsigned int channels;
};

// Interleaved buffers
int input( void * /*outputBuffer*/, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  InputData *iData = (InputData *) data;

  // Simply copy the data to our allocated buffer.
  unsigned int frames = nBufferFrames;
  if ( iData->frameCounter + nBufferFrames > iData->totalFrames ) {
    frames = iData->totalFrames - iData->frameCounter;
    iData->bufferBytes = frames * iData->channels * sizeof( MY_TYPE );
  }

  unsigned long offset = iData->frameCounter * iData->channels;
  memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
  iData->frameCounter += frames;

  if ( iData->frameCounter >= iData->totalFrames ) return 2;
  return 0;
}

int main( int argc, char *argv[] )
{
  unsigned int channels, fs, bufferFrames, device = 0, offset = 0;
  double time = 2.0;
  FILE *fd;

  // minimal command-line checking
  if ( argc < 3 || argc > 6 ) usage();

  std::vector< RtAudio::Api > apis;
  RtAudio :: getCompiledApi( apis );

  std::cout << "\nCompiled APIs:\n";
  for ( size_t i=0; i<apis.size(); i++ )
    std::cout << i << ". " << RtAudio::getApiDisplayName(apis[i])
              << " (" << RtAudio::getApiName(apis[i]) << ")" << std::endl;

  RtAudio adc;
  std::vector<unsigned int> deviceIds = adc.getDeviceIds();
  if ( deviceIds.size() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  channels = (unsigned int) atoi( argv[1] );
  fs = (unsigned int) atoi( argv[2] );
  if ( argc > 3 )
    time = (double) atof( argv[3] );
  if ( argc > 4 )
    device = (unsigned int) atoi( argv[4] );
  if ( argc > 5 )
    offset = (unsigned int) atoi( argv[5] );

  // Let RtAudio print messages to stderr.
  adc.showWarnings( true );

  // Set our stream parameters for input only.
  bufferFrames = 512;
  RtAudio::StreamParameters iParams;
  iParams.nChannels = channels;
  iParams.firstChannel = offset;

  if ( device == 0 )
    iParams.deviceId = adc.getDefaultInputDevice();
  else {
    if ( device >= deviceIds.size() )
      device = getDeviceIndex( adc.getDeviceNames() );
    iParams.deviceId = deviceIds[device];
  }

  InputData data;
  data.buffer = 0;
  if ( adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, &input, (void *)&data ) )
    goto cleanup;

  if ( adc.isStreamOpen() == false ) goto cleanup;

  data.bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );
  data.totalFrames = (unsigned long) (fs * time);
  data.frameCounter = 0;
  data.channels = channels;
  unsigned long totalBytes;
  totalBytes = data.totalFrames * channels * sizeof( MY_TYPE );

  // Allocate the entire data buffer before starting stream.
  data.buffer = (MY_TYPE*) malloc( totalBytes );
  if ( data.buffer == 0 ) {
    std::cout << "Memory allocation error ... quitting!\n";
    goto cleanup;
  }

  if ( adc.startStream() ) goto cleanup;

  std::cout << "\nRecording for " << time << " seconds ... writing file 'record.raw' (buffer frames = " << bufferFrames << ")." << std::endl;
  while ( adc.isStreamRunning() ) {
    SLEEP( 100 ); // wake every 100 ms to check if we're done
  }

  // Now write the entire data to the file.
  fd = fopen( "record.raw", "wb" );
  fwrite( data.buffer, sizeof( MY_TYPE ), data.totalFrames * channels, fd );
  fclose( fd );

 cleanup:
  if ( adc.isStreamOpen() ) adc.closeStream();
  if ( data.buffer ) free( data.buffer );

  return 0;
}
