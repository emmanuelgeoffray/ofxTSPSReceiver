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
    
    // update these if TSPS core changes!
    // valid as of TSPS version 1.3.4
    static const string PERSON_ENTERED  = "/TSPS/personEntered/";
    static const string PERSON_UPDATED  = "/TSPS/personUpdated/";
    static const string PERSON_LEAVING  = "/TSPS/personWillLeave/";
    static const string SCENE_UPDATED   = "/TSPS/scene/";
    static const string CUSTOM_EVENT    = "/TSPS/customEvent/";
    
    enum ConnectionStatus
    {
        UNKNOWN,
        CONNECTING,
        CONNECTED,
        DISCONNECTED
    };
    
    class Receiver : public ofxOscReceiver
    {
    public:
        Receiver();
        
        void connect( int port );
        void update( ofEventArgs &e );
        void draw( int width, int height );
        
        Person* personAtIndex(int i);
        Person* personWithID(int pid);
        
        int totalPeople();
        Scene* getScene();
        vector<Person*> & getPeople();
        
    protected:
        ConnectionStatus status;
        
        vector<Person*> trackedPeople;
        Scene scene;
        float personTimeout;
        
        void updatePersonFromOSC( Person * p, ofxOscMessage & m );
    };
}
