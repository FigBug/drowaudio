/*
 *  DeckTransport.cpp
 *  Decks
 *
 *  Created by David Rowland on 05/03/2010.
 *  Copyright 2010 dRowAudio. All rights reserved.
 *
 */

#include "DeckTransport.h"

void DeckTransport::setLoopAndCueValueToReferTo(Value &valueToReferTo)
{
	loopAndCuePoints->setToggleValueToReferTo(valueToReferTo);
}