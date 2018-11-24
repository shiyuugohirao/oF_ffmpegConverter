#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void draw();
    void dragEvent(ofDragInfo dragInfo);
    void startConvert();
    void finConvert(string &msg);

    int sizeX,sizeY;
    ofxPanel gui;
    ofxLabel fileNameLabel;
    ofxIntField framerate;
    ofxIntField inWidth;
    ofxIntField inHeight;
    ofxTextField addName;
    ofxButton button;
    ofxToggle useThread;

    string msg="";
    string originalFilePath = "none";
    string filePath = "none";
    string fileName = "none";
    enum STATE{ WAIT, CONVERTING, SUCCESS, FAIL } state;
    const string ext[3]={
        ".mp4",".mov","m4v"
    };
};
