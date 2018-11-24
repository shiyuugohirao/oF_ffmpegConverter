#include "ofApp.h"

//  [*]
//  you can check CMD "which ffmpeg" in terminal in OSX!!
//  in my case "/usr/local/bin/ffmpeg"
const string FFMPEG_PATH =  "/usr/local/bin/ffmpeg";

//--------------------------------------------------------------
void ofApp::setup(){
    int w = ofGetWidth();
    gui.setup(); // most of the time you don't need a name
    gui.setPosition(0,0);
    gui.setSize(ofGetWidth(), ofGetHeight()-10);
    gui.add(fileNameLabel.setup("fileName", "please drag&drop movie file",w,40));
    gui.add(framerate.setup("framerate(default:40)", 40, 1,60, w,20));
    gui.add(inWidth.setup("inWidth(default:480)", 480,10,1920*2, w,20));
    gui.add(inHeight.setup("inHeight (-1:autoFromWidth)", -1,10,1280*2, w,20));
    gui.add(addName.setup("addName", "_converted","","", w,20));

    gui.add(useThread.setup("useThread",true,w,20));
    gui.add(button.setup("START CONVERT",w,50));
    button.addListener(this, &ofApp::startConvert);

    gui.loadFromFile(ofxPanelDefaultFilename);

    state = WAIT;
}
//--------------------------------------------------------------
void ofApp::startConvert(){
    string fullPath = ofToDataPath(originalFilePath,true);
    if(!ofFile::doesFileExist(fullPath)){
        ofSystemAlertDialog("couldn't find file ("+fullPath+")");
        return;
    }
    string w = to_string(inWidth);
    string h = inHeight==-1 ? "\"trunc(ih/(iw/"+w+")/2)*2\"" : to_string(inHeight);
    string thread = useThread ? " -threads 0 " : "";

    string cmd =
    FFMPEG_PATH + " -i "+ originalFilePath
    + " -y "
    + " -r " + to_string(framerate)
    + " -vf scale=" + w + ":" + h + " -pix_fmt yuv420p"
    + " -vcodec h264" + thread
    + "-acodec aac -strict experimental "
    + filePath + "_converted" + ext[0]
    ;

    ofPtr<std::thread> t;
    t = make_shared<std::thread>([&]{
        state = CONVERTING;
        msg = ofSystem(cmd);
        if(msg==""){
            msg = "exec command \n"+cmd;
            state = SUCCESS;
            originalFilePath = "";
        }else{
            msg = "ERROR\n" + msg;
            state = FAIL;
        }
    });
    t->detach();
    sleep(1);
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch (state) {
        case WAIT: ofBackground(ofColor::white); break;
        case SUCCESS: ofBackground(ofColor::lightBlue); break;
        case FAIL:{
            ofSystemAlertDialog(msg);
            state = WAIT;
            break;
        }
        case CONVERTING:{
            ofBackground(ofFloatColor::fromHsb(ofGetElapsedTimeMillis()%1000/1000., 0.5, 1.0));
            break;
        }
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo info){
    if( info.files.size() > 0 ){
        auto ext = ofToLower(ofFilePath::getFileExt(info.files[0]));
        if(ext=="mov" || ext=="mp4" || ext=="m4v"){
            state = WAIT;
            originalFilePath = info.files[0];
            auto itr = originalFilePath.find_last_of(".");
            filePath.assign(originalFilePath.begin(),originalFilePath.begin()+itr);
            itr = originalFilePath.find_last_of("/");
            itr++;
            fileName.assign(originalFilePath.begin()+itr,originalFilePath.end());
            fileNameLabel = fileName;
        }
        else{
            state = FAIL;
            ofSystemAlertDialog("support only mov,mp4,m4v");
        }
    }
}
