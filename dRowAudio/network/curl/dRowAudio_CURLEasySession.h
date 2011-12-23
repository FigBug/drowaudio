/*
  ==============================================================================

    CURLEasySession.h
    Created: 18 May 2011 12:50:44am
    Author:  David Rowland

  ==============================================================================
*/

#ifndef __DROWAUDIO_CURLEASYSESSION_H__
#define __DROWAUDIO_CURLEASYSESSION_H__

#include "dRowAudio_CURLManager.h"

//==============================================================================
/**	Creates a CURLEasySession.
	One of these is used to handle a specific transfer. Create one on the stack
	and when it goes out of scope it will clean up after itself.
 
	@todo directory list is returned if this is found before a file transfer
	@todo rename remote file if it already exists
 */
class CURLEasySession : public TimeSliceClient
{
public:
	//==============================================================================
	CURLEasySession();

	CURLEasySession (String localPath,
                     String remotePath,
                     bool upload,
                     String username = String::empty,
                     String password = String::empty);
    
	~CURLEasySession();

    //==============================================================================
    /** Sets the the source of an upload to an input stream.
     */
    void setInputStream (InputStream* newInputStream);
    
    /** Sets the local file to either upload or download into.
     */
	void setLocalFile (File newLocalFile);
	
    /** Returns the local file being used.
        If an input stream has been specified this will return File::nonexistent.
     */
	File getLocalFile()     {	return localFile;	}
	
    /** Sets the remote path to use.
        This can be a complete path with a file name. If so the path will be used 
        as the destination file for an upload or as the source for a download.
        If this ends with a '/' character a random file name will be generated.
     */
	void setRemotePath (String newRemotePath);
		
    /** Returns the remote path being used.
        If this ends with a '/' character it specifies a directory.
     */
	String getRemotePath()  {	return remotePath;	}

    /**	Sets the user name and password of the connection.
     This is only used if required by the connection to the server.
	 */
	void setUserNameAndPassword (String username, String password);
    
    //==============================================================================
    /** Returns the current working directory of the remote server.
     */
	String getCurrentWorkingDirectory();
	
    /**	Returns the directory listing of the remote file.
	 */
	StringArray getDirectoryListing();
	
    //==============================================================================
	/**	Turns on full debugging information.
		This is probably best turned off in release builds to avoid littering the console.
	 */
	void enableFullDebugging (bool shouldEnableFullDebugging);
	
	/**	Begins the transfer.
		Returns an error code or an empty String if everything is set up ok.
		The transfer will actually take place on a background thread so use getLastError()
		to determine the last error that occured.
	 */
	void beginTransfer (bool transferIsUpload, bool performOnBackgroundThread = true);

    /** Stops the current transfer.
     */
    void stopTransfer();
    
	/** Resets the state of the session to the parameters that have been specified.
     */
	void reset();
	    
    /** Returns the progress of the current transfer.
     */
    float getProgress()            {   return progress.get();  }
    
    //==============================================================================
    /** A class for receiving callbacks from a CURLEasySession.
	 */
    class  Listener
    {
    public:
        //==============================================================================
        /** Destructor. */
        virtual ~Listener() {}
		
        //==============================================================================
        /** Called when a transfer is about to start.
		 */
        virtual void transferAboutToStart (CURLEasySession* session) {};
		
        /** Called when a transfer is about to start.
		 */
        virtual void transferProgressUpdate (CURLEasySession* session) {};

        /** Called when a transfer is about to start.
		 */
        virtual void transferEnded (CURLEasySession* session) {};
    };
	
    /** Adds a listener to be called when this slider's value changes. */
    void addListener (Listener* listener);
	
    /** Removes a previously-registered listener. */
    void removeListener (Listener* listener);
	
    //==============================================================================
	/** @internal */
	int useTimeSlice();
	
private:
    //==============================================================================
	static size_t writeCallback (void* sourcePointer, size_t blockSize, size_t numBlocks, CURLEasySession* session);
	static size_t readCallback (void* destinationPointer, size_t blockSize, size_t numBlocks, CURLEasySession* session);
	static size_t directoryListingCallback (void* sourcePointer, size_t blockSize, size_t numBlocks, CURLEasySession* session);
	static int internalProgressCallback (CURLEasySession* session, double dltotal, double dlnow, double ultotal, double ulnow);
	
    //==============================================================================
	CURL* handle;
	String remotePath, userNameAndPassword;
	bool isUpload, shouldStopTransfer;
	Atomic<float> progress;
	
	File localFile;
	ScopedPointer<FileOutputStream> outputStream;
	ScopedPointer<InputStream> inputStream;
	MemoryBlock directoryContentsList;
    
    CriticalSection transferLock;
    ListenerList <Listener> listeners;

    //==============================================================================
    CURLcode performTransfer (bool transferIsUpload);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CURLEasySession);
};

#endif  // __DROWAUDIO_CURLEASYSESSION_H__
