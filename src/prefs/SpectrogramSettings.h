/**********************************************************************

Audacity: A Digital Audio Editor

SpectrogramSettings.h

Paul Licameli

**********************************************************************/

#ifndef __AUDACITY_SPECTROGRAM_SETTINGS__
#define __AUDACITY_SPECTROGRAM_SETTINGS__

#include "ClientData.h" // to inherit
#include "Prefs.h"
#include "SampleFormat.h"
#include "RealFFTf.h"

#undef SPECTRAL_SELECTION_GLOBAL_SWITCH

class EnumValueSymbols;
struct FFTParam;
class NumberScale;
class SpectrumPrefs;
class wxArrayStringEx;
class WaveTrack;

class AUDACITY_DLL_API SpectrogramSettings
   : public PrefsListener
   , public ClientData::Cloneable< ClientData::UniquePtr >
{
   friend class SpectrumPrefs;
public:

   // Singleton for settings that are not per-track
   class AUDACITY_DLL_API Globals
   {
   public:
      static Globals &Get();
      void SavePrefs();

#ifdef SPECTRAL_SELECTION_GLOBAL_SWITCH
      bool spectralSelection;
#endif

   private:
      Globals();
      void LoadPrefs();
   };

   enum {
      LogMinWindowSize = 3,
      LogMaxWindowSize = 15,

      NumWindowSizes = LogMaxWindowSize - LogMinWindowSize + 1,
   };

   // Do not assume that this enumeration will remain the
   // same as NumberScaleType in future.  That enum may become
   // more general purpose.
   typedef int ScaleType;
   enum ScaleTypeValues : int {
      stLinear,
      stLogarithmic,
      stMel,
      stBark,
      stErb,
      stPeriod,

      stNumScaleTypes,
   };

   static const EnumValueSymbols &GetScaleNames();
   static const EnumValueSymbols &GetColorSchemeNames();
   static const TranslatableStrings &GetAlgorithmNames();

   // Return either the track's independent settings or global defaults
   static SpectrogramSettings &Get(WaveTrack &track);
   static const SpectrogramSettings &Get(const WaveTrack &track);

   // Force creation of track's independent settings
   static SpectrogramSettings &Own(WaveTrack &track);

   //! Make track lose indpendent settings and use defaults
   static void Reset(WaveTrack &track);

   static SpectrogramSettings &defaults();
   SpectrogramSettings();
   SpectrogramSettings(const SpectrogramSettings &other);
   SpectrogramSettings& operator= (const SpectrogramSettings &other);
   ~SpectrogramSettings();

   PointerType Clone() const override;

   bool IsDefault() const
   {
      return this == &defaults();
   }

   bool Validate(bool quiet);
   void LoadPrefs();
   void SavePrefs();

   void UpdatePrefs() override;

   void InvalidateCaches();
   void DestroyWindows();
   void CacheWindows() const;
   void ConvertToEnumeratedWindowSizes();
   void ConvertToActualWindowSizes();

   // Need to be told what the bin unit is, as this structure does not know
   // the rate
   float findBin( float frequency, float binUnit ) const;

   // If "bins" is false, units are Hz
   NumberScale GetScale( float minFreq, float maxFreq ) const;

   int minFreq;
   int maxFreq;

   bool SpectralSelectionEnabled() const;

public:
   int range;
   int gain;
   int frequencyGain;

   int windowType;

private:
   int windowSize;
public:
   size_t WindowSize() const { return windowSize; }

private:
   int zeroPaddingFactor;
public:
   size_t ZeroPaddingFactor() const {
      return algorithm == algPitchEAC ? 1 : zeroPaddingFactor;
   }

   size_t GetFFTLength() const; // window size (times zero padding, if STFT)
   size_t NBins() const;

   enum ColorScheme : int {
      // Keep in correspondence with AColor::colorSchemes, AColor::gradient_pre
      csColorNew = 0,
      csColorTheme,
      csGrayscale,
      csInvGrayscale,

      csNumColorScheme,
   };
   ColorScheme colorScheme;

   class ColorSchemeEnumSetting : public EnumSetting< ColorScheme > {
       using EnumSetting< ColorScheme >::EnumSetting;
       void Migrate(wxString &value) override;
   };
   static ColorSchemeEnumSetting colorSchemeSetting;

   ScaleType scaleType;

#ifndef SPECTRAL_SELECTION_GLOBAL_SWITCH
   bool spectralSelection; // But should this vary per track? -- PRL
#endif

   typedef int Algorithm;
   enum AlgorithmValues : int {
      algSTFT = 0,
      algReassignment,
      algPitchEAC,

      algNumAlgorithms,
   };
   Algorithm algorithm;

#ifdef EXPERIMENTAL_FFT_Y_GRID
   bool fftYGrid;
#endif //EXPERIMENTAL_FFT_Y_GRID

#ifdef EXPERIMENTAL_FIND_NOTES
   bool fftFindNotes;
   double findNotesMinA;
   int numberOfMaxima;
   bool findNotesQuantize;
#endif //EXPERIMENTAL_FIND_NOTES

   // Following fields are derived from preferences.

   // Variables used for computing the spectrum
   mutable HFFT           hFFT;
   mutable Floats         window;

   // Two other windows for computing reassigned spectrogram
   mutable Floats         tWindow; // Window times time parameter
   mutable Floats         dWindow; // Derivative of window
};

extern AUDACITY_DLL_API IntSetting SpectrumMaxFreq;

class AUDACITY_DLL_API SpectrogramBounds
   : public ClientData::Cloneable< ClientData::UniquePtr >
{
public:

   //! Get either the global default settings, or the track's own if previously created
   static SpectrogramBounds &Get( WaveTrack &track );

   //! @copydoc Get
   static const SpectrogramBounds &Get( const WaveTrack &track );

   ~SpectrogramBounds() override;
   PointerType Clone() const override;

   void GetBounds(const WaveTrack &wt, float &min, float &max) const;

   void SetBounds(float min, float max)
   { mSpectrumMin = min, mSpectrumMax = max; }

   float mSpectrumMin = -1, mSpectrumMax = -1;
};

#endif
