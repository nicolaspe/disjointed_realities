#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSpout.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void ofApp::renderTexShaders();
		
		int texWid = 1024; int texHei = 1024;
		int fboWid = 1280; int fboHei = 720;
		int texInd = 0;

		ofFbo		texFbo;
		ofShader	texShaders[4];

		ofLight		dirLight;
		ofImage		skyTex;
		ofxAssimpModelLoader model;

		ofFbo		fbo1, fbo2;
		ofEasyCam   cam1, cam2;

};
