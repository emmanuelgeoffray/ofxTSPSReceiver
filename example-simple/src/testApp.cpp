#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    tspsReceiver.connect(12000);
    
    // this will add listeners to your app; this is optional!
    // you will need to add all three of these functions to do this:
    // void onPersonEntered( ofxTSPS::EventArgs & tspsEvent );
    // void onPersonUpdated( ofxTSPS::EventArgs & tspsEvent );
    // void onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent );
    
    ofxAddTSPSListeners(this);
}

//--------------------------------------------------------------
void testApp::update(){
    // here, you can loop through each person and do stuff
    vector<ofxTSPS::Person*> people = tspsReceiver.getPeople();
    for ( int i=0; i<people.size(); i++){
        // do stuff for each person!
        //people[i]->contour...
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    // debug draw!
    tspsReceiver.draw(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void testApp::onPersonEntered( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "New person!");
    // you can access the person like this:
    // tspsEvent.person
}

//--------------------------------------------------------------
void testApp::onPersonUpdated( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person updated!");
    // you can access the person like this:
    // tspsEvent.person
    
}

//--------------------------------------------------------------
void testApp::onPersonWillLeave( ofxTSPS::EventArgs & tspsEvent ){
    ofLog(OF_LOG_NOTICE, "Person left!");
    // you can access the person like this:
    // tspsEvent.person
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}