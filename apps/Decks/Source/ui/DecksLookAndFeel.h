/*
 *  DecksLookAndFeel.h
 *  Decks
 *
 *  Created by David Rowland on 04/03/2010.
 *  Copyright 2010 dRowAudio. All rights reserved.
 *
 */

#ifndef _DECKSLOOKANDFEEL__H_
#define _DECKSLOOKANDFEEL__H_

#include <juce/juce.h>

class DecksLookAndFeel :	public LookAndFeel,
							public DeletedAtShutdown
{
public:
	juce_DeclareSingleton (DecksLookAndFeel, false) 

	enum DecksColours {
		backgroundColour,
		panelColour,
		panelLineColour,
		meterLineColour,
		noCustomColours
	};
	
	enum IconType {
		Stop,
		Play,
		Cue,
		Pause,
		Next,
		Previous,
		ShuffleForward,
		ShuffleBack,
		Eject,
		Cross,
		Add,
		Search,
		Power,
		Bypass,
		GoUp,
		Infinity,
		DownTriangle,
		noIcons
	};
	
	DecksLookAndFeel();
	
	~DecksLookAndFeel()
	{
		DBG("DecksLookAndFeel deleted");
	}

	void setDecksColour(DecksColours colour, Colour newColour)	{	decksColours.getReference(colour) = newColour;	}

	Colour getDecksColour(DecksColours colour)					{	return decksColours[colour];	}

	//============================================================
	int getSliderThumbRadius (Slider& slider);

	virtual void drawRotarySlider (Graphics& g,
								   int x, int y,
								   int width, int height,
								   float sliderPosProportional,
								   const float rotaryStartAngle,
								   const float rotaryEndAngle,
								   Slider& slider);
	
	virtual void drawLinearSliderThumb (Graphics& g,
									    int x, int y,
									    int width, int height,
									    float sliderPos,
									    float minSliderPos,
									    float maxSliderPos,
									    const Slider::SliderStyle style,
										Slider& slider);
	
	virtual void drawFaderKnob(Graphics& g,
							   const Slider::SliderStyle style,
							   const float x,
							   const float y,
							   const float width,
							   const float height);
	
	/** Draws the text for a TextButton. */
    virtual void drawButtonBackground (Graphics& g,
									   Button& button,
									   const Colour& backgroundColour,
									   bool isMouseOverButton,
									   bool isButtonDown);                 
	
	static void drawSquareButton (Graphics& g,
                                  const float x, const float y,
                                  const float width, const float height,
                                  const Colour& colour,
                                  const float outlineThickness,
                                  const float cornerSize) throw();
	//============================================================
	virtual Button* createDocumentWindowButton (int buttonType);
	
	virtual void drawDocumentWindowTitleBar (DocumentWindow& window,
                                             Graphics& g, int w, int h,
                                             int titleSpaceX, int titleSpaceW,
                                             const Image* icon,
                                             bool drawTitleTextOnLeft);
		
	//============================================================
	static DrawablePath createIcon (IconType icon, Colour colour);
	
	
private:
	Array<Colour> decksColours;
};

#endif //_DECKSLOOKANDFEEL__H_