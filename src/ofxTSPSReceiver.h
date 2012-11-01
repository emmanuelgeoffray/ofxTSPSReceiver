//
//  ofxTSPSReceiver.h
//  TSPSReceiver
//
//  Created by Brett Renfer on 11/1/12.
//
//

#pragma once

#include "ofxOsc.h"

#include "ofxTSPSEvents.h"
#include "ofxTSPSPerson.h"
#include "ofxTSPSScene.h"

namespace ofxTSPS {
    
    class Receiver : public ofxOscReceiver
    {
    public:
        Receiver();
        
        void update( ofEventArgs &e );
        void draw( int width, int height );
        void connect( int port );
        
        Person* personAtIndex(int i);
        Person* personWithID(int pid);
        
        int totalPeople();
        Scene* getScene();
        vector<Person*> & getPeople();
        
    protected:	
        vector<Person*> trackedPeople;
        Scene scene;
        bool bSetup;
        float personTimeout;
        
        void updatePersonFromOSC( Person * p, ofxOscMessage & m );
        
    };
}
