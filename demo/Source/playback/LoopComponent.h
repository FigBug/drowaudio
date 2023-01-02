/*
    ==============================================================================

    This file is part of the dRowAudio JUCE module
    Copyright 2004-13 by dRowAudio.

    ------------------------------------------------------------------------------

    dRowAudio is provided under the terms of The MIT License (MIT):

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    ==============================================================================
*/

#ifndef LOOP_COMPONENT_H
#define LOOP_COMPONENT_H

#include "../DemoHeader.h"

class LoopMarker : public Component
{
public:
    LoopMarker() :
        isMouseOver (false)
    {
    }

    //==============================================================================
    void resized() override
    {
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }

    void paint (Graphics& g) override
    {
        g.fillAll (isMouseOver ? Colours::red.withAlpha (0.9f) : Colours::red.withAlpha (0.7f));
    }

    void mouseEnter (const MouseEvent&) override
    {
        setMouseCursor (MouseCursor::PointingHandCursor);
        isMouseOver = true;
        repaint();
    }

    void mouseExit (const MouseEvent&) override
    {
        setMouseCursor (MouseCursor::NormalCursor);
        isMouseOver = false;
        repaint();
    }

    void mouseDown (const MouseEvent& e) override
    {
        setMouseCursor (MouseCursor::DraggingHandCursor);
        dragger.startDraggingComponent (this, e);
    }

    void mouseUp (const MouseEvent&) override
    {
        setMouseCursor (MouseCursor::PointingHandCursor);
    }

    void mouseDrag (const MouseEvent& e) override
    {
        dragger.dragComponent (this, e, &constrainer);
    }

private:
    //==============================================================================
    ComponentDragger dragger;
    ComponentBoundsConstrainer constrainer;
    bool isMouseOver;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopMarker)
};

//==============================================================================
class LoopComponent : public Component,
                      public ComponentListener,
                      public AudioFilePlayer::Listener
{
public:
    LoopComponent (AudioFilePlayerExt& audioFilePlayer);

    ~LoopComponent();

    //==============================================================================
    void resized() override;
    void paint (Graphics& g) override;
    void componentMovedOrResized (Component& component, bool wasMoved, bool wasResized) override;
    void fileChanged (AudioFilePlayer* player) override;
    void audioFilePlayerSettingChanged (AudioFilePlayer* player, int settingCode) override;

private:
    //==============================================================================
    AudioFilePlayerExt& audioFilePlayer;
    LoopMarker marker1, marker2;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopComponent)
};

#endif //LOOP_COMPONENT_H
