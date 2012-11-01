/*
 *  Events.cpp
 *  openTSPS
 */

#include "ofxTSPSEvents.h"

namespace ofxTSPS {
    CoreEvents & Events(){
        static CoreEvents * events = new CoreEvents;
        return *events;
    }
}