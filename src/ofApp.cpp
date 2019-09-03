#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
    ofDisableArbTex();
    ofEnableDepthTest();

	// init Fbos
	texFbo.allocate(texWid, texHei);

	// init shaders
	string vertex = "shaders/shader.vert";
    texShaders[0].load(vertex, "shaders/basicColor.frag");
    texShaders[1].load(vertex, "shaders/raymarch_alien.frag");
    texShaders[2].load(vertex, "shaders/colorWall.frag");
    texShaders[3].load(vertex, "shaders/basicColor.frag");

	// init mesh/model
	skyTex.load("imgs/eso0932a_sphere.jpg");
	model.loadModel("models/nico_00_sit.obj");
	model.setRotation(1, 180, 0, 0, 1);

	// init camera
    cam1.setPosition(1500, 1200, 2500);
    cam1.lookAt(ofVec3f(0, 0, 0));
	cam1.setFarClip(15000.);

	// init light
	//dirLight.setAmbientColor(ofColor(255, 255, 255));
	dirLight.setDirectional();
	dirLight.setOrientation(ofVec3f(1, -1, 1));

}

//--------------------------------------------------------------
void ofApp::update(){
	cam1.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(128);

	renderTexShaders();

	cam1.begin();
		dirLight.enable();
		skyTex.bind();
		ofDrawSphere(ofPoint(0,0,0), 5000);
		skyTex.unbind();
		texFbo.getTexture().bind();
		model.setRotation(2, ofGetElapsedTimef()*10, 0, 1, 0);
		model.drawFaces();
		texFbo.getTexture().unbind();
		dirLight.disable();
    cam1.end();
}

//--------------------------------------------------------------
void ofApp::renderTexShaders() {
	texFbo.begin();
	ofClear(0, 0);
		texShaders[texInd].begin();
		texShaders[texInd].setUniform1f("u_time", ofGetElapsedTimef()/2.);
		texShaders[texInd].setUniform2f("u_resolution", texWid, texHei);
		if(texInd == 2){
			texShaders[texInd].setUniform1f("u_moveSpeed", 2.);
			texShaders[texInd].setUniform1f("u_colorSpeed", 1.);
			texShaders[texInd].setUniform1f("u_colorWave", 0.);
			texShaders[texInd].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[texInd].end();
	texFbo.end();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
			texInd = 0;
			break;
		case '1':
			texInd = 1;
			break;
		case '2':
			texInd = 2;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){ }
