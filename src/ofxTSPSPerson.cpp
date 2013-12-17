/*
 *  Person.cpp
 *  openTSPS
 *
 */

#include "ofxTSPSPerson.h"
#define MAX_HAAR_GHOSTFRAMES 15 //how long before we say it's really gone

namespace ofxTSPS {
    //--------------------------------------------------------------
    Person::Person(int pid, int oid )
    : pid(pid),
    oid(oid),
    age(0),
    hasHaar(false),
    haarRect(ofRectangle(0,0,0,0)),
    opticalFlowVectorAccumulation(ofPoint(0,0)),
    centroid(0,0),
    framesWithoutHaar(0),
    customAttributes(NULL),
    depth(0),
    highest(-1,1),
    lowest(-1,-1)
    {
        update();
    }
    
    //--------------------------------------------------------------
    Person::~Person(){
        if(customAttributes != NULL){
            free(customAttributes);
        }
    }
    
    //--------------------------------------------------------------
    void Person::update(){
        age++;
    }
    
    //--------------------------------------------------------------
    void Person::updateBoundingRect(ofRectangle rect ){
        boundingRect.set(rect);
        area = boundingRect.width * boundingRect.height;
    }
    
    //--------------------------------------------------------------
    void Person::updateCentroid( ofPoint _centroid, bool dampen ){
        if(dampen){
            centroid = (centroid * .7) + (_centroid * .3);
        }
        else{
            centroid = _centroid;
        }
        
        velocity	 = _centroid - centroid;
    }
    
    //--------------------------------------------------------------
    void Person::draw( int cameraWidth, int cameraHeight, bool bSendContours, bool bSendHaar, float haarPadding ){
        //draw contours 
        ofPushStyle();
        ofNoFill();
        if (bSendContours){
            ofSetHexColor(0x3abb93);
        } else {
            ofSetHexColor(0xc4b68e);
        }
        ofBeginShape();
        for( int j=0; j<contour.size(); j++ ) {
            ofVertex( contour[j].x * cameraWidth, contour[j].y * cameraHeight );
        }
        ofEndShape();
        ofPopStyle();
        
        // draw haar    
        ofSetHexColor(0xffffff);							
        if(bSendHaar){
            ofSetHexColor(0xee3523);
            //draw haar search area expanded 
            //limit to within data box so it's not confusing				
            ofRectangle haarRect = ofRectangle(boundingRect.x - haarPadding, 
                                               boundingRect.y - haarPadding, 
                                               boundingRect.width  + haarPadding*2, 
                                               boundingRect.height + haarPadding*2);
            if (haarRect.x < 0){
                haarRect.width += haarRect.x;
                haarRect.x = 0;					
            }
            if (haarRect.y < 0){
                haarRect.height += haarRect.y;	
                haarRect.y = 0;
            }
            if (haarRect.x + haarRect.width > cameraWidth) haarRect.width = cameraWidth-haarRect.x;
            if (haarRect.y + haarRect.height > cameraHeight) haarRect.height = cameraHeight-haarRect.y;
            ofRect(haarRect.x* cameraWidth, haarRect.y* cameraHeight, haarRect.width* cameraWidth, haarRect.height* cameraHeight);
            
            if(hasHaarRect()){
                //draw the haar rect
                ofSetHexColor(0xee3523);
                ofRect(getHaarRect().x* cameraWidth, getHaarRect().y* cameraHeight, getHaarRect().width* cameraWidth, getHaarRect().height* cameraHeight);
                //haar-detected people get a red square
                ofSetHexColor(0xfd5f4f);
            } else {
                //no haar gets a yellow square
                ofSetHexColor(0xeeda00);
            }
        } else {
            //no haar gets a yellow square
            ofSetHexColor(0xeeda00);
        }
        
        //draw person
        ofRect(boundingRect.x* cameraWidth, boundingRect.y* cameraHeight, boundingRect.width* cameraWidth, boundingRect.height* cameraHeight);
        
        // draw highest point
        ofSetHexColor(0xff00ff);	
        ofCircle(highest.x* cameraWidth, highest.y* cameraHeight, 4);
        
        //draw centroid
        ofSetHexColor(0xff0000);
        ofCircle(centroid.x* cameraWidth, centroid.y* cameraHeight, 3);
        
        //draw id
        ofSetHexColor(0xffffff);
        char idstr[1024];
        sprintf(idstr, "pid: %d\noid: %d\nage: %d", pid, oid, age );
        ofDrawBitmapString(idstr, centroid.x* cameraWidth+8, centroid.y* cameraHeight);
    }
    
    //--------------------------------------------------------------
    void Person::updateContour(ofPolyline _contour){
        contour		 = _contour; 
        
        simpleContour = contour;    
        simpleContour.simplify(2.0f);
        float simplifyAmount = 2.5f;
        while (simpleContour.size() > 100){
            simpleContour.simplify(simplifyAmount);
            simplifyAmount += .5f;
        }
    }
    
    //--------------------------------------------------------------
    void Person::setHaarRect(ofRectangle _haarRect){
        haarRect = _haarRect;
        if ( haarRect.isEmpty() ){
            noHaarThisFrame();
        } else {
            hasHaar = true;
            framesWithoutHaar = 0;
        }
    }
    
    //--------------------------------------------------------------
    ofRectangle Person::getHaarRect(){
        if(!hasHaar){
            printf("Person: accessing Haar rect without Haar!\n");
            return ofRectangle(0,0,0,0);
        }
        return haarRect;
    }
    
    //--------------------------------------------------------------
    bool Person::hasHaarRect(){
        return hasHaar;
    }
    
    //--------------------------------------------------------------
    void Person::noHaarThisFrame(){
        //temp remove keep haar
        hasHaar = false;
        haarRect.set(0,0,0,0);
        return;
        
        //if we had a haar but it vanished move it by the movement of the blob
        if(framesWithoutHaar++ > MAX_HAAR_GHOSTFRAMES){
            hasHaar = false;
        }
        if(hasHaar){
            haarRect.x += velocity.x;
            haarRect.y += velocity.y;
        }
    }
  
    //--------------------------------------------------------------
    ofRectangle Person::getBoundingRectNormalized(float videoWidth, float videoHeight){
		return ofRectangle(boundingRect.x/videoWidth,
						   boundingRect.y/videoHeight,
						   boundingRect.width/videoWidth,
						   boundingRect.height/videoHeight);
    }
    
    
    //--------------------------------------------------------------
    ofRectangle Person::getHaarRectNormalized(float videoWidth, float videoHeight){
        return ofRectangle(haarRect.x/videoWidth,
                           haarRect.y/videoHeight,
                           haarRect.width/videoWidth,
                           haarRect.height/videoHeight);
    };
    
    //--------------------------------------------------------------
    ofPoint Person::getCentroidNormalized(float videoWidth, float videoHeight){
        return ofPoint(centroid.x / videoWidth, centroid.y / videoHeight);
    }
    
    //--------------------------------------------------------------
    ofPoint Person::getHaarCentroidNormalized(float videoWidth, float videoHeight){
        ofRectangle haarRect = getHaarRect();
        return ofPoint( (haarRect.x + haarRect.width/2) / videoWidth, (haarRect.y + haarRect.height/2) / videoHeight );
    }
   /***************************************************************
     Get OSC Message(s)
     - why messages plural?
     - so we can send new types of OSC packets for new types of data!
     e.g. /TSPS/personEntered/skeleton
     - this is sort of a bum fix for Processing's lack of bundle support...
     ***************************************************************/
    //--------------------------------------------------------------
    vector<ofxOscMessage> Person::getOSCMessages( string type, bool bUseLegacy, int cameraWidth, int cameraHeight, bool sendContours ){
        vector<ofxOscMessage> v;
        
        ofxOscMessage m;
        m.setAddress( type );
        m.addIntArg( pid );
        if(!bUseLegacy){
            m.addIntArg(oid);
        }
        m.addIntArg(age);
        
        //ofPoint centroidNormalized = getCentroidNormalized(cameraWidth, cameraHeight);
        
        m.addFloatArg(centroid.x);
        m.addFloatArg(centroid.y);
        m.addFloatArg(velocity.x);
        m.addFloatArg(velocity.y);
        
        if(!bUseLegacy){
            m.addFloatArg(depth);
        }
        
        //ofRectangle boundingRect = getBoundingRectNormalized(cameraWidth,cameraHeight);
        
        m.addFloatArg(boundingRect.x);
        m.addFloatArg(boundingRect.y);
        m.addFloatArg(boundingRect.width);
        m.addFloatArg(boundingRect.height);
        
        if(!bUseLegacy){
            m.addFloatArg(highest.x / cameraWidth );
            m.addFloatArg(highest.y / cameraHeight );
        }
        
        //ofRectangle haarRect = getHaarRectNormalized(cameraWidth,cameraHeight);
        if (!bUseLegacy){
            m.addFloatArg(haarRect.x);
            m.addFloatArg(haarRect.y);
            m.addFloatArg(haarRect.width);
            m.addFloatArg(haarRect.height);
        }
        
        m.addFloatArg(opticalFlowVectorAccumulation.x);
        m.addFloatArg(opticalFlowVectorAccumulation.y);
        
        if (sendContours){
            //any args after 11 will be contours
            for (int i=0; i<simpleContour.size(); i++){
                m.addFloatArg(simpleContour[i].x);
                m.addFloatArg(simpleContour[i].y);
            };
        }
        
        v.push_back(m);
        
        return v;
    };
  
}
