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

		// === mirror functions
		void initMirrors();
		void updateMirrors();

		// === render functions 
		void renderTexShaders();
		void renderBackgroundTex();

		void render2dUnlitMask();
		void render2dLitMask();
		void render2dBackground();
		void render2dUVTextured();
		
		void renderBaseScene();
		void renderFullScene();
		void renderMirrorViews();

		void textureUnlitBlend();
		void textureShadwBlend();

		// === controlling variables 
		int texWid = 1024; int texHei = 1024;
		int fboWid = 1280; int fboHei = 720;
		int texInd = 0;
		bool bLight, bMapMode, bDebug, bInfo, bTex;
		bool bBackground, bBckgndTex, bMirrors, bAnimated;

		ofFbo		texFbo, texBckgndFbo;
		ofShader	texShaders[4], blendShader, shadwShader, bckgndShader;

		ofFbo			unlitFbo, litFbo, bgFbo, uvFbo, blendUFbo, blendSFbo;
		ofFbo			sceneBaseFbo, sceneFullFbo;
		ofEasyCam		cam1;
		ofLight			dirLight, ambLight;
		ofTexture		skyTex, whiteTex;
		ofSpherePrimitive	skySph;
		ofCylinderPrimitive	skyCyl;
		ofBoxPrimitive		skyBox;
		ofxAssimpModelLoader model;
		ofxFBXScene		fbxScene;
		ofxFBXManager	fbxMngr;

		ofVec2f		mirrorDim[N_MIRRORS];
		ofVec3f		mirrorPos[N_MIRRORS], mirCamPos[N_MIRRORS];
		ofCamera	mirrorCam[N_MIRRORS];
		ofFbo		mirrorFbo[N_MIRRORS], mirrors;
		ofPlanePrimitive	mirrorMsh[N_MIRRORS];

		vector<ofNode>		sceneFull, sceneMask;
		vector<ofTexture>	sceneTex;
};
