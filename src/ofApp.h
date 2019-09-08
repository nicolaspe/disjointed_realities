#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSpout.h"
#include "ofxOsc.h"
#include "ofxFBX.h"

#define N_MIRRORS 4

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

		void initMirrors();
		void updateMirrors();
		void renderMirrorViews();

		void renderTexShaders();
		void renderUnlitMask();
		void renderLitMask();
		void renderBackground();
		void renderUVTextured();
		void textureUnlitBlend();
		void textureShadwBlend();
		
		int texWid = 1024; int texHei = 1024;
		int fboWid = 1280; int fboHei = 720;
		int texInd = 0;
		bool bLight, bMapMode, bDebug, bBackground, bInfo, bTex, bMirrors;

		ofFbo		texFbo;
		ofShader	texShaders[4], blendShader, shadwShader;

		ofFbo		unlitFbo, litFbo, bgFbo, uvFbo, blendUFbo, blendSFbo;
		ofEasyCam   cam1;
		ofLight		dirLight, ambLight;
		ofTexture	skyTex, whiteTex;
		ofxAssimpModelLoader model;

		ofVec2f		mirrorDim[N_MIRRORS];
		ofVec3f		mirrorPos[N_MIRRORS], mirCamPos[N_MIRRORS];
		ofCamera	mirrorCam[N_MIRRORS];
		ofFbo		mirrorFbo[N_MIRRORS], mirrors;
		ofMesh		mirrorMsh[N_MIRRORS];

		ofxFBXScene   fbxScene;
		ofxFBXManager fbxMngr;

};
