#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("disjointed realities v0.0.1");
	ofSetFrameRate(30);
    ofSetVerticalSync(true);
	//ofSetSmoothLighting(true);
    ofDisableArbTex();
    ofEnableDepthTest();
	ofEnableAlphaBlending();

	// init state variables
	bLight = false;
	bMapMode = true; 
	bDebug = false;
	bBackground = false;
	bInfo = true;
	bTex = false;
	bMirrors = false;


	// init Fbos
	texFbo.allocate   (texWid, texHei);
	unlitFbo.allocate (fboWid, fboHei);
	litFbo.allocate   (fboWid, fboHei);
	uvFbo.allocate    (fboWid, fboHei);
	bgFbo.allocate    (fboWid, fboHei);
	blendUFbo.allocate(fboWid, fboHei);
	blendSFbo.allocate(fboWid, fboHei);
	mirrors.allocate  (fboWid, fboHei);

	// init shaders
	string vertex = "shaders/shader.vert";
	texShaders[0].load(vertex, "shaders/white.frag");
	texShaders[1].load(vertex, "shaders/basicColor.frag");
    texShaders[2].load(vertex, "shaders/raymarch_alien.frag");
    texShaders[3].load(vertex, "shaders/colorWall.frag");
	blendShader.load(vertex, "shaders/maskBlend.frag");
	shadwShader.load(vertex, "shaders/shadowBlend.frag");

	// init mesh/model
	ofLoadImage(skyTex, "imgs/eso0932a_sphere.jpg");
	ofLoadImage(whiteTex, "imgs/white.png");
	model.loadModel("models/nico_00_sit.obj");
	model.setRotation(1, 180, 0, 0, 1);

	// init fbx
	//ofxFBXSceneSettings fbxSettings;
	//if (fbxScene.load("models/nico_00_walk.fbx", fbxSettings)) {
	//	cout << "ofApp :: fbx scene load OK" << endl;
	//} else {
	//	cout << "ofApp :: ERROR loading fbx scene" << endl;
	//}
	//fbxMngr.setup( &fbxScene );
	//fbxMngr.setAnimation(0);

	// init camera
    cam1.setPosition(1500, 1200, 2500);
    cam1.lookAt(ofVec3f(0, 1000, 000));
	cam1.setFarClip(15000.);

	// init light
	ambLight.setAmbientColor(ofColor(255));
	dirLight.setDirectional();
	dirLight.rotateDeg(90, 0, 1, 0);
	

	// init mirrors
	initMirrors();
}

//--------------------------------------------------------------
void ofApp::update(){
	updateMirrors();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(120 + 120*sin(ofGetElapsedTimef()));

	// render fbos by themselves
	ofEnableDepthTest();

	renderBackground();

	renderTexShaders();
	renderUVTextured();
	renderUnlitMask();
	renderLitMask();

	textureUnlitBlend();
	textureShadwBlend();

	renderMirrorViews();


	// display fbos on screen
	ofDisableDepthTest();
	if (bBackground) { bgFbo.draw(0, 0); }
	else {}

	if (bTex) { texFbo.draw(0, 0); }
	else {}

	if (bMapMode) {
		uvFbo.draw(0, 0);
	}
	else{
		if (bDebug) {
			if (!bLight) { unlitFbo.draw(0, 0); }
			else { litFbo.draw(0, 0); }
		}
		else {
			if (!bLight) { blendUFbo.draw(0, 0); }
			else { blendSFbo.draw(0, 0); }
		}
	}


	// mirrors!
	if (bMirrors) {
		// for each mirror
		for (size_t i = 0; i < N_MIRRORS; i++) {
			ofPushMatrix();
			// bind the corresponding texture
			mirrorFbo[i].getTexture().bind();
			// go to the mirror position
			ofTranslate(mirrorPos[i]);
			// and draw the mirror
			mirrorMsh[i].draw();
			// unbind texture and reset position
			mirrorFbo[i].getTexture().unbind();
			ofPopMatrix();
		}
	}
}


//--- MIRRORS --------------------------------------------------
void ofApp::initMirrors() {
	// initialize mirror dimensions & center positions
	// mirror 0
	mirrorDim[0] = ofVec2f(512, 1024);
	mirrorPos[0] = ofVec3f(500, 512, 500);
	// mirror 1
	mirrorDim[1] = ofVec2f(512, 1024);
	mirrorPos[1] = ofVec3f(-500, 512, 500);
	// mirror 2
	mirrorDim[2] = ofVec2f(512, 1024);
	mirrorPos[2] = ofVec3f(500, 512, -500);
	// mirror 3
	mirrorDim[3] = ofVec2f(512, 1024);
	mirrorPos[3] = ofVec3f(-500, 512, -500);

	// create each mirror surface and camera
	for (size_t i = 0; i < N_MIRRORS; i++) {
		// calculate distance from mirror to main camera
		ofVec3f d = cam1.getGlobalPosition() - mirrorPos[i];
		mirrorCam[i].setGlobalPosition( mirrorPos[i] - d );
		mirrorCam[i].lookAt( cam1.getGlobalPosition() );
		// create viewport and mesh according to dimensions
		mirrorFbo[i].allocate(mirrorDim[i].x, mirrorDim[i].y);
		mirrorMsh[i] = ofMesh::plane(mirrorDim[i].x, mirrorDim[i].y);	
	}
}
void ofApp::updateMirrors() {
	// for each mirror
	for (size_t i = 0; i < N_MIRRORS; i++) {
		// update each mirror camera position
		ofVec3f d = cam1.getGlobalPosition() - mirrorPos[i];
		mirrorCam[i].setGlobalPosition( mirrorPos[i] - d );
		// update where each mirror camera looks at
		mirrorCam[i].lookAt(cam1.getGlobalPosition());
	}
}


//--------------------------------------------------------------
void ofApp::renderTexShaders() {
	texFbo.begin();
	ofClear(0, 0);
		texShaders[texInd].begin();
		texShaders[texInd].setUniform1f("u_time", ofGetElapsedTimef()/2.);
		texShaders[texInd].setUniform2f("u_resolution", texWid, texHei);
		if(texInd == 3){
			texShaders[texInd].setUniform1f("u_moveSpeed", 2.);
			texShaders[texInd].setUniform1f("u_colorSpeed", 1.);
			texShaders[texInd].setUniform1f("u_colorWave", 0.);
			texShaders[texInd].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[texInd].end();
	texFbo.end();
}

void ofApp::renderBackground() {
	bgFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		skyTex.bind();
		ofDrawSphere(ofPoint(0, 0, 0), 5000);
		skyTex.unbind();
	cam1.end();
	bgFbo.end();
}

void ofApp::renderUVTextured() {
	uvFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		dirLight.enable();
		texFbo.getTexture().bind();
		model.drawFaces();
		texFbo.getTexture().unbind();
		dirLight.disable();
	cam1.end();
	uvFbo.end();
}

void ofApp::renderUnlitMask() {
	unlitFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		// disable directional lights
		dirLight.disable();
		// and only use flat ambient light
		ambLight.enable();
		whiteTex.bind();
		model.drawFaces();
		whiteTex.unbind();
		ambLight.disable();
	cam1.end();
	unlitFbo.end();
}
void ofApp::renderLitMask() {
	litFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		dirLight.enable();
		whiteTex.bind();
		model.drawFaces();
		whiteTex.unbind();
		dirLight.disable();
	cam1.end();
	litFbo.end();
}

void ofApp::textureUnlitBlend() {
	blendUFbo.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex",   texFbo.getTextureReference(0),   0);
		blendShader.setUniformTexture("maskTex", unlitFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef() / 2.);
		blendShader.setUniform2f("u_resolution", fboWid, fboHei);
		ofDrawRectangle(0, 0, fboWid, fboHei);
		blendShader.end();
	blendUFbo.end();
}
void ofApp::textureShadwBlend() {
	blendSFbo.begin();
	ofClear(0, 0);
		shadwShader.begin();
		shadwShader.setUniformTexture("inTex",    texFbo.getTexture(), 0);
		shadwShader.setUniformTexture("unlitTex", unlitFbo.getTexture(), 1);
		shadwShader.setUniformTexture("shadwTex", unlitFbo.getTexture(), 2);
		ofDrawRectangle(0, 0, fboWid, fboHei);
		shadwShader.end();
	blendSFbo.end();
}

void ofApp::renderMirrorViews() {
	// first, draw each mirror's perspectives
	for (size_t i = 0; i < N_MIRRORS; i++) {
		mirrorFbo[i].begin();
		ofClear(255, 255, 255, 1);
			mirrorCam[i].begin();
				if (bBackground) {
					skyTex.bind();
					ofDrawSphere(ofPoint(0, 0, 0), 5000);
					skyTex.unbind();
				}
				dirLight.enable();
				texFbo.getTexture().bind();
				model.drawFaces();
				texFbo.getTexture().unbind();
				dirLight.disable();
			mirrorCam[i].end();
		mirrorFbo[i].end();
	}
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
		case '3':
			texInd = 3;
			break;
		case 'l':
			bLight = !bLight;
			break;
		case 'm':
			bMapMode = !bMapMode;
			break;
		case 'i':
			bInfo = !bInfo;
			break;
		case 'd':
			bDebug = !bDebug;
			break;
		case 'b':
			bBackground = !bBackground;
			break;
		case 't':
			bTex = !bTex;
			break;
		case 'x':
			bMirrors = !bMirrors;
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
