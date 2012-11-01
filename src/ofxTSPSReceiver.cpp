
/***************************************************************************
 *
 *  Receiver.cpp
 *	Receiver
 *  Rockwell LAB + IDEO LAB peopleVision project
 *
 *  Created by Brett Renfer
 *  Adapted/Polished for by James George ( http://www.jamesgeorge.org )
 *  in collaboration with FlightPhase ( http://www.flightphase.com )
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer as
 * the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NUI GROUP ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BEN WOODHOUSE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include "ofxTSPSReceiver.h"

namespace ofxTSPS {

    Receiver::Receiver(){
        bSetup = false;
        personTimeout = 3.5 * 1000; // in millis
    }

    void Receiver::connect( int port ){
        setup(port);
        bSetup = true;
        ofAddListener(ofEvents().update, this, &Receiver::update);
    }

    void Receiver::draw(int width, int height){
        ofPushStyle();
        ofNoFill();
        ofSetLineWidth(.5);
        for(int i = 0; i < totalPeople(); i++){
            Person* p = personAtIndex(i);
            p->draw(width, height);
        }
        ofPopStyle();
    }

    Scene* Receiver::getScene()
    {
        return &scene;
    }
    
    
    vector<Person*> & Receiver::getPeople(){
        return trackedPeople;
    }

    void Receiver::update( ofEventArgs &e ){
        
        if(hasWaitingMessages()){
            // check for waiting messages
            while( hasWaitingMessages() ){
                // get the next message
                ofxOscMessage m;
                getNextMessage( &m );
                
                if(m.getAddress() == "/TSPS/scene/") {
                    scene.percentCovered = m.getArgAsFloat(1);
                }
                
                if (m.getAddress() == "/TSPS/personEntered/" ||
                    m.getAddress() == "/TSPS/personUpdated/" ||
                    m.getAddress() == "/TSPS/personWillLeave/"){
                    
                    int id = m.getArgAsInt32(0);
                    bool personIsNew = false;
                    Person* person = personWithID( id );
                    if(person == NULL){
                        person = new Person(id, trackedPeople.size());
                        trackedPeople.push_back(person);
                        personIsNew = true;
                    }
                    updatePersonFromOSC(person, m);
                
                    EventArgs args;
                    args.person = person;
                    args.scene = &scene;
                    
                    if (m.getAddress() == "/TSPS/personEntered/" || personIsNew){
                        ofNotifyEvent(Events().personEntered, args, this);
                    } else if (m.getAddress() == "/TSPS/personUpdated/"){
                        ofNotifyEvent(Events().personUpdated, args, this);
                        
                    } else if (m.getAddress() == "/TSPS/personWillLeave/"){
                        ofNotifyEvent(Events().personWillLeave, args, this);
                    }
                    
                    if(m.getAddress() == "/TSPS/personWillLeave/"){
                        for (int i = trackedPeople.size() - 1; i >= 0; i--){
                            if (trackedPeople[i]->pid == person->pid){
                                trackedPeople.erase(trackedPeople.begin() + i);
                                break;
                            }
                        }
                        delete person;
                    }
                }
            }
        }
        
        
        //clear orphaned blobs
//        for(int i = trackedPeople.size()-1; i >= 0; i--) {
//            if(ofGetElapsedTimeMillis()-trackedPeople[i]->updatedAt > personTimeout) {
//                EventArgs args;
//                args.person = trackedPeople[i];
//                args.scene = scene;
//                trackedPeople.erase(trackedPeople.begin() + i);
//                ofNotifyEvent(Events().personWillLeave, args, this);
//                delete trackedPeople[i];
//            }
//        }
    }
    
    void Receiver::updatePersonFromOSC( Person * p, ofxOscMessage & m ){
        p->oid = m.getArgAsInt32(1);
        p->age = m.getArgAsInt32(2);
        p->centroid.set( m.getArgAsFloat(3), m.getArgAsFloat(4));
        p->velocity.set( m.getArgAsFloat(5), m.getArgAsFloat(6));
        p->depth = m.getArgAsFloat(7);
        p->boundingRect.set(m.getArgAsFloat(8),m.getArgAsFloat(9),m.getArgAsFloat(10),m.getArgAsFloat(11));
        p->setHaarRect(ofRectangle(m.getArgAsFloat(12),m.getArgAsFloat(13),m.getArgAsFloat(14),m.getArgAsFloat(15)));
        p->opticalFlowVectorAccumulation.set(m.getArgAsFloat(16), m.getArgAsFloat(17));
        p->highest.set(m.getArgAsFloat(18), m.getArgAsFloat(19));
        
        ofPolyline line;
        for (int i=20; i<m.getNumArgs(); i+=2){
            line.addVertex( ofPoint(m.getArgAsFloat(i),m.getArgAsFloat(i+1)) );
        }
        p->updateContour( line );
    }

    //callbacks to get people
    Person* Receiver::personAtIndex(int i)
    {
        return trackedPeople[i];
    }

    Person* Receiver::personWithID(int pid)
    {
        for (int i = 0; i < trackedPeople.size(); i++){
            if (trackedPeople[i]->pid == pid){
                return trackedPeople[i];
            }
        }
        return NULL;
    }

    int Receiver::totalPeople()
    {
        return trackedPeople.size();
    }
}

