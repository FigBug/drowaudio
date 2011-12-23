/*
  ==============================================================================

    dRowAudio_AudioThumbnailImage.cpp
    Created: 9 Jul 2011 7:35:03pm
    Author:  David Rowland

  ==============================================================================
*/

BEGIN_JUCE_NAMESPACE

AudioThumbnailImage::AudioThumbnailImage (AudioFilePlayer* sourceToBeUsed,
                                          TimeSliceThread& backgroundThread_,
                                          AudioThumbnailCache* cacheToUse,
                                          AudioThumbnail* thumbnailToUse,
                                          int sourceSamplesPerThumbnailSample_)
    : filePlayer            (sourceToBeUsed),
	  currentSampleRate     (44100.0),
      oneOverSampleRate     (1.0),
      backgroundThread      (backgroundThread_),
      audioThumbnailCache   (cacheToUse, (cacheToUse == nullptr) ? true : false),
      audioThumbnail        (thumbnailToUse, (thumbnailToUse == nullptr) ? true : false),
      sourceSamplesPerThumbnailSample (sourceSamplesPerThumbnailSample_),
      lastTimeDrawn         (0.0),
      resolution            (3.0),
      renderComplete        (true)
{
    jassert (filePlayer != nullptr);
    
    waveformImage = Image (Image::RGB, 1, 1, false);

	// instansiate the cache and the thumbnail if needed
	if (audioThumbnailCache == nullptr)
    {
        OptionalScopedPointer<AudioThumbnailCache> newCache (new AudioThumbnailCache (3), true);
		audioThumbnailCache = newCache;
	}
    if (thumbnailToUse == nullptr)
    {
        OptionalScopedPointer<AudioThumbnail> newThumbnail (new AudioThumbnail (sourceSamplesPerThumbnailSample,
                                                                                *filePlayer->getAudioFormatManager(),
                                                                                *audioThumbnailCache),
                                                            true);
        newThumbnail->clear();
        audioThumbnail = newThumbnail;
    }
	//audioThumbnail->addChangeListener (this);
    
	// register with the file player to recieve update messages
	filePlayer->addListener (this);
}

AudioThumbnailImage::~AudioThumbnailImage()
{
	filePlayer->removeListener (this);
    
    for (int i = 0; i < backgroundThread.getNumClients(); i++) 
    {
        if (backgroundThread.getClient (i) == this) 
        {
            backgroundThread.removeTimeSliceClient (this);
        }
    }
	
    stopTimer ();
}

//====================================================================================
Image AudioThumbnailImage::getImageAtTime (double startTime, double duration)
{
    const int startPixel = roundToInt (startTime * oneOverFileLength * waveformImage.getWidth());
    const int numPixels = roundToInt (duration * oneOverFileLength * waveformImage.getWidth());
    
    return waveformImage.getClippedImage (Rectangle<int> (startPixel, 0, numPixels, waveformImage.getHeight()));
}

void AudioThumbnailImage::setResolution (double newResolution)
{
    resolution = newResolution;
    
    waveformImage.clear (waveformImage.getBounds(), Colours::black);
    lastTimeDrawn = 0.0;
    refreshWaveform();
}

//====================================================================================
void AudioThumbnailImage::timerCallback ()
{
    if (! renderComplete)
    {
        listeners.call (&Listener::imageUpdated, this);
    }
    else
    {
        listeners.call (&Listener::imageUpdated, this);
        listeners.call (&Listener::imageFinished, this);
        stopTimer();
    }
}

int AudioThumbnailImage::useTimeSlice()
{
    refreshWaveform();
    
    return 50;
}

void AudioThumbnailImage::fileChanged (AudioFilePlayer *player)
{
	if (player == filePlayer)
	{
        if (filePlayer->getAudioFormatReaderSource() != nullptr)
        {
            currentSampleRate = filePlayer->getAudioFormatReaderSource()->getAudioFormatReader()->sampleRate;

            if (currentSampleRate > 0.0)
            {
                oneOverSampleRate = 1.0 / currentSampleRate;
                fileLength = filePlayer->getLengthInSeconds();
                oneOverFileLength = 1.0 / fileLength;
                
                const int imageWidth = filePlayer->getTotalLength() / sourceSamplesPerThumbnailSample;
                waveformImage = Image (Image::RGB, jmax(1, imageWidth), 100, true);

                waveformImage.clear (waveformImage.getBounds(), Colours::black);
                lastTimeDrawn = 0.0;
                
                File newFile (filePlayer->getPath());
                if (newFile.existsAsFile()) 
                {
                    FileInputSource* fileInputSource = new FileInputSource (newFile);
                    audioThumbnail->setSource (fileInputSource);
                    renderComplete = false;
                }
                else if (filePlayer->sourceIsMemoryBlock())
                {
                    MemoryInputSource* inputSource = new MemoryInputSource (filePlayer->getInputStream());
                    audioThumbnail->setSource (inputSource);
                    renderComplete = false;
                }
                else 
                {
                    audioThumbnail->setSource (nullptr);
                    renderComplete = true;
                }
                
                listeners.call (&Listener::imageChanged, this);

                backgroundThread.addTimeSliceClient (this);
                if (! backgroundThread.isThreadRunning())
                    backgroundThread.startThread (4);
                
                startTimer (100);
            }
        }
        else 
        {
            audioThumbnail->setSource (nullptr);
            renderComplete = true;
            
            listeners.call (&Listener::imageChanged, this);
            
            backgroundThread.addTimeSliceClient (this);
            if (! backgroundThread.isThreadRunning())
                backgroundThread.startThread (4);
            
            startTimer (100);
        }
	}
}

//==============================================================================
void AudioThumbnailImage::addListener (AudioThumbnailImage::Listener* const listener)
{
    listeners.add (listener);
}

void AudioThumbnailImage::removeListener (AudioThumbnailImage::Listener* const listener)
{
    listeners.remove (listener);
}

//==============================================================================	
void AudioThumbnailImage::refreshWaveform()
{
	if (audioThumbnail->getNumSamplesFinished() > 0)
	{
        const double endTime = audioThumbnail->getNumSamplesFinished() * oneOverSampleRate;
        double timeToDraw = endTime - lastTimeDrawn;
        if (lastTimeDrawn > timeToDraw)
        {
            lastTimeDrawn -= timeToDraw * 0.5; // overlap by 0.5
            timeToDraw = endTime - lastTimeDrawn;
        }
        
        const double startPixelX = roundToInt (lastTimeDrawn * oneOverFileLength * waveformImage.getWidth());
        const double numPixels = roundToInt (timeToDraw * oneOverFileLength * waveformImage.getWidth());
        const double numTempPixels = numPixels * resolution;
        
        if (numTempPixels > 0)
        {
            if (tempSectionImage.getWidth() < numTempPixels)
            {
                tempSectionImage = Image (Image::RGB, 
                                          numTempPixels, waveformImage.getHeight(), 
                                          false);
            }
            
            Rectangle<int> rectangleToDraw (0, 0, 
                                            numTempPixels, waveformImage.getHeight());
            
            Graphics gTemp (tempSectionImage);
            tempSectionImage.clear(tempSectionImage.getBounds(), Colours::black);
            gTemp.setColour (Colours::green);
            audioThumbnail->drawChannel (gTemp, rectangleToDraw,
                                         lastTimeDrawn, endTime,
                                         0, 1.0f);
            lastTimeDrawn = endTime;
            
            Graphics g (waveformImage);
            g.drawImage (tempSectionImage,
                         startPixelX, 0, numPixels, waveformImage.getHeight(),
                         0, 0, numTempPixels, tempSectionImage.getHeight());
        }
	}

    if (renderComplete)
        backgroundThread.removeTimeSliceClient (this);

    if (audioThumbnail->isFullyLoaded())
        renderComplete = true;
}

//==============================================================================

END_JUCE_NAMESPACE
