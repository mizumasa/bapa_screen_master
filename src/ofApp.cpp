#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofBackground(0);
    //ofSetVerticalSync(true);
    grabber.initGrabber(640, 360);
    ofSetCircleResolution(32);
    
    img.loadImage("screen.png");
    
    bHideImage = false;
    bHideGui = false;
    bMusicStop = false;
    bMusicPlay = false;
    bMusicReset = false;
    
    judgeLine = 440;
    score = 0;
    
    //objVelocity = 3.9095f;
    objVelocity = 3.8925f;
    
    gui.setup("panel");
    gui.add(length_1.set("length",400,200,800));
    gui.add(missThr.set("missThr", 30,1,200));//失敗と判定する閾値
    
    missThr.addListener(this, &ofApp::valChanged);

    startTime = ofGetElapsedTimeMillis();
    
    ofShowCursor();
    
    receiver.setup(12345);
    sender.setup(HOST, 12346);
    
    
    //ここから3D CG
    ofDisableArbTex();
    ofLoadImage(texture, "dot.png");
    
    // set the camera distance
    camDist  = 1605;
    camera.setDistance(camDist);
    
    // randomly add a point on a sphere
    int   num = 500;
    float radius = 1000;
    for(int i = 0; i<num; i++ ) {
        
        float theta1 = ofRandom(0, TWO_PI);
        float theta2 = ofRandom(0, TWO_PI);
        
        ofVec3f p;
        p.x = cos( theta1 ) * cos( theta2 );
        p.y = sin( theta1 );
        p.z = cos( theta1 ) * sin( theta2 );
        p *= radius;
        
        addPoint(p.x, p.y, p.z);
        
    }
    
    // upload the data to the vbo
    int total = (int)points.size();
    vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
    vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
    
    // load the shader
#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/shader");
#else
    shader.load("shaders/shader");
#endif
    
}

void ofApp::valChanged(int &val){
}



//--------------------------------------------------------------
void ofApp::update(){
    
    grabber.update();
    
    while (receiver.hasWaitingMessages()) {
        //メッセージを取得
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        if(m.getAddress() == "/bang"){ //名前をチェック
            Obj o;
            o.setup(ofVec2f(ofGetWidth(),40), objVelocity);
            Objects.push_back(o);
            //cout << "bang fired" << endl;
        }
        else if(m.getAddress() == "/bang/long"){ //名前をチェック
            Obj o;
            o.setupLong(ofVec2f(ofGetWidth(),120), objVelocity, length_1);
            longObjects.push_back(o);
        }
        else if(m.getAddress() == "/bang/big"){ //名前をチェック
            Obj o;
            o.setup(ofVec2f(ofGetWidth(),200), objVelocity);
            bigObjects.push_back(o);
        }
        else if(m.getAddress() == "/mouse/position2"){ //名前をチェック
            getMessage2(m);
        }
        else if(m.getAddress() == "/mouse/position22"){ //名前をチェック
            getMessage22(m);
        }
    }
    
    for (int i = 0; i < Objects.size(); i++){
        if(Objects[i].position.x < -Objects[i].radius){
            Objects.erase(Objects.begin()+i);
            score -= 100;
        }
        Objects[i].update();
    }
    
    for (int i = 0; i < longObjects.size(); i++){
        if(longObjects[i].position.x < -(longObjects[i].radius+longObjects[i].length)){
            longObjects.erase(longObjects.begin()+i);
        }
        longObjects[i].update();
    }
    
    for (int i = 0; i < bigObjects.size(); i++){
        if(bigObjects[i].position.x < -bigObjects[i].radius){
            bigObjects.erase(bigObjects.begin()+i);
        }
        bigObjects[i].update();
    }
    
    for (int i = 0; i < ObjHumans.size(); i++){
        if(ObjHumans[i].count > 100){
            ObjHumans.erase(ObjHumans.begin()+i);
        }
        ObjHumans[i].update();
    }
}


//--------------------------------------------------------------
void ofApp::addPoint(float x, float y, float z) {
    ofVec3f p(x, y, z);
    points.push_back(p);
    
    // we are passing the size in as a normal x position
    float size = ofRandom(5, 50);
    sizes.push_back(ofVec3f(size));
}

//--------------------------------------------------------------
void ofApp::draw3d(){
    glDepthMask(GL_FALSE);
    
    ofSetColor(255, 100, 90);
    
    // this makes everything look glowy :)
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();
    
    // bind the shader and camera
    // everything inside this function
    // will be effected by the shader/camera
    shader.begin();
    camera.begin();
    
    // bind the texture so that when all the points
    // are drawn they are replace with our dot image
    texture.bind();
    vbo.draw(GL_POINTS, 0, (int)points.size());
    texture.unbind();
    
    camera.end();
    shader.end();
    
    ofDisablePointSprites();
    ofDisableBlendMode();
    
    // check to see if the points are
    // sizing to the right size
    ofEnableAlphaBlending();
    
    camera.begin();
    ofSetLineWidth(1);
    for (unsigned int i=0; i<points.size(); i++) {
        ofSetColor(255, 80);
        ofVec3f mid = points[i];
        mid.normalize();
        mid *= 300;
        ofLine(points[i], mid);
    }
    camera.end();
    
    glDepthMask(GL_TRUE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofSetColor(255);
    
    //if(bHideImage) img.draw(0,0);
    //else grabber.draw(0, 0, ofGetWidth(), ofGetHeight());
    
//    ofSetColor(100, 200);
//    ofRect(0, 0, ofGetWidth(), 250);

    //ここから3D CG
    draw3d();
    //ここまで3D CG

    ofSetColor(255);
    ofSetLineWidth(3.5);
    ofLine(0, 40, ofGetWidth(), 40);
    ofLine(0, 120, ofGetWidth(), 120);
    ofLine(0, 200, ofGetWidth(), 200);
    //ofLine(0, 250, ofGetWidth(), 250);
    //ofSetLineWidth(4);
    //ofLine(judgeLine, 0, judgeLine, 230);
    ofFill();
    for(int i=0;i<16;i++){
        ofCircle(i*80+40, 40, 6.5);
        ofCircle(i*80+40, 120, 6.5);
        ofCircle(i*80+40, 200, 6.5);
    }
    for(int i=0;i<3;i++){
        ofSetColor(150);
        ofCircle(judgeLine, i*80+40, 35);
    }
    
    //ofSetColor(0, 0, 255);
    for (int i = 0; i < Objects.size(); i++) {
        Objects[i].draw();
    }
    
    //ofSetColor(0, 255, 0);
    for (int i = 0; i < longObjects.size(); i++) {
        longObjects[i].drawLong();
        
//        if(bPressed[i]){
//            longObjects[i].fillRed();
//        }

    }

    for (int i = 0; i < bigObjects.size(); i++) {
        bigObjects[i].drawBig();
    }

    if(bHideGui) gui.draw();
    
    ofSetColor(255);
    ofRect(0, 0, judgeLine-80, 250);
    
//    float timer = ofGetElapsedTimeMillis() - startTime;
//    int x = ofGetWidth() - timer/3.561f;
//    int y = ofGetHeight()/2;
//    
//    ofSetColor(0, 0, 255);
//    ofCircle(x,y,30);

    //こっから動体描画
    for (int i = 0; i < ObjHumans.size(); i++) {
        ObjHumans[i].draw();
    }
    
    
    if(!bHideGui) gui.draw();
    
    string info = "FPS: "+ofToString(ofGetFrameRate(), 3);
    //info += "\nObjects num: "+ofToString(Objects.size());
    //info += "\nlongObjects num: "+ofToString(longObjects.size());
    info += "\npress z: normal x: long c: big";
    info += "\npress p: music play s: stop r: reset";
    if(bMusicPlay) info += "\nmusic: play";
    else if(bMusicStop) info += "\nmusic: stop";
    else if(bMusicReset) info += "\nmusic: reset";
    info += "\nScore: "+ofToString(score);
    info += "\nelapsed time: "+ofToString(ofGetElapsedTimeMillis());
//    info += "\ntimer: "+ofToString(timer)+" ms";
    info += "\nobjVel: "+ofToString(objVelocity);
    ofSetColor(255);
    ofDrawBitmapString(info, 20, ofGetHeight()-100);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'h') bHideImage = !bHideImage;
    else if(key == 'g') bHideGui = !bHideGui;
    else if(key == 'z'){
        Obj o;
        o.setup(ofVec2f(ofGetWidth(),40), objVelocity);
        Objects.push_back(o);
    }
    else if(key == 'x'){
        Obj o;
        o.setupLong(ofVec2f(ofGetWidth(),120), objVelocity, length_1);
        longObjects.push_back(o);
    }
    else if(key == 'c'){
        Obj o;
        o.setup(ofVec2f(ofGetWidth(),200), objVelocity);
        bigObjects.push_back(o);
    }

    else if(key == 'p'){
        ofxOscMessage m;
        m.setAddress("/duration/play");
        sender.sendMessage(m);
        bMusicStop = false;
        bMusicReset = false;
        bMusicPlay = true;
    }
    else if(key == 's'){
        ofxOscMessage m;
        m.setAddress("/duration/stop");
        sender.sendMessage(m);
        bMusicPlay = false;
        bMusicReset = false;
        bMusicStop = true;
        score = 0;
    }
    else if(key == 'r'){
        ofxOscMessage m;
        m.setAddress("/duration/stop");
        sender.sendMessage(m);
        bMusicPlay = false;
        bMusicStop = false;
        bMusicReset = true;
        objVelocity = 3.9095f;
        score = 0;
    }
    else if(key == OF_KEY_UP){
        objVelocity += 0.001f;
    }
    else if(key == OF_KEY_DOWN){
        objVelocity -= 0.001f;
    }
    else if(key == OF_KEY_RIGHT){
        objVelocity += 0.01f;
    }
    else if(key == OF_KEY_LEFT){
        objVelocity -= 0.01f;
    }
    else if(key == OF_KEY_RETURN){
        for (int i = 0; i < bigObjects.size(); i++){
            if(bigObjects[i].position.x > judgeLine-25 && bigObjects[i].position.x < judgeLine+25){
                bigObjects.erase(bigObjects.begin()+i);
                score += 300;
            }
        }
    }else if(key == 's') {
        gui.saveToFile("settings.xml");
    }
    else if(key == 'l') {
        gui.loadFromFile("settings.xml");
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    for (int i = 0; i < Objects.size(); i++){
        if(Objects[i].position.x > judgeLine-25 && Objects[i].position.x < judgeLine+25){
            Objects.erase(Objects.begin()+i);
            score += 100;
        }
    }
    
    for (int i = 0; i < longObjects.size(); i++){
//        if( longObjects[i].position.x < judgeLine && longObjects[i].position.x + longObjects[i].length > judgeLine){
//            
//        }
    }
    
    startTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::getMessage2(ofxOscMessage m){
    ObjHumans.clear();
    int mousenum;
    mousenum = m.getNumArgs();
    for (int j=0 ; j < mousenum/4 ; j++){
        //叩かれた座標
        mouseX = m.getArgAsInt32(0+j*3);
        mouseY = m.getArgAsInt32(1+j*3);
        //物体検出id
        int mouseID;
        mouseID = m.getArgAsInt32(2+j*3);
        //平均動きからのズレ
        int mouseStd;
        mouseStd = m.getArgAsInt32(j+mousenum/4*3);
        ObjHuman o;
        o.setup(mouseX,mouseY,mouseID,mouseStd,missThr);
        ObjHumans.push_back(o);
    }
}

//--------------------------------------------------------------
void ofApp::getMessage22(ofxOscMessage m){
    int msgnum;
    msgnum = m.getNumArgs();
    if(msgnum==2){
        velx_ave = m.getArgAsInt32(0);
        vely_ave = m.getArgAsInt32(1);
    }
}
