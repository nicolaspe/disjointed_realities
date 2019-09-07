#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetSmoothLighting(true);
    ofDisableArbTex();
    ofEnableDepthTest();
	ofEnableAlphaBlending();

	// init state variables
	bLight = false;
	bMapMode = true; 
	bDebug = false;
	bBackground = false;
	bInfo = true;


	// init Fbos
	texFbo.allocate(texWid, texHei, GL_RGBA);
	unlitFbo.allocate(fboWid, fboHei, GL_RGBA);
	litFbo.allocate(fboWid, fboHei, GL_RGBA);
	uvFbo.allocate(fboWid, fboHei, GL_RGBA);
	bgFbo.allocate(fboWid, fboHei, GL_RGBA);
	blendFbo.allocate(fboWid, fboHei, GL_RGBA);


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

	// init camera
    cam1.setPosition(1500, 1200, 2500);
    cam1.lookAt(ofVec3f(0, 1000, 000));
	cam1.setFarClip(15000.);

	// init light
	ambLight.setAmbientColor(ofColor(255));
	dirLight.setDirectional();
	dirLight.rotateDeg(90, 0, 1, 0);

}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(120 + 120*sin(ofGetElapsedTimef()));

	renderTexShaders();
	renderBackground();


	if (bBackground) { bgFbo.draw(0, 0); }
	else {}

	if (bMapMode) {
		renderUVTextured();
		uvFbo.draw(0, 0);
	}
	else{
		renderUnlitMask();
		renderLitMask();
		if (bDebug) {
			if (!bLight) { unlitFbo.draw(0, 0); }
			else { litFbo.draw(0, 0); }
		}
		else {
			if (!bLight) { textureUnlitBlend(); }
			else { textureShadwBlend(); }
			blendFbo.draw(0, 0);
		}
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
		ofDrawSphere(ofPoint(0, 0, 0), 50);
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
	blendFbo.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex",   texFbo.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", unlitFbo.getTexture(), 1);
		ofDrawRectangle(0, 0, fboWid, fboHei);
		blendShader.end();
	blendFbo.end();
}
void ofApp::textureShadwBlend() {
	blendFbo.begin();
	ofClear(0, 0);
		shadwShader.begin();
		shadwShader.setUniformTexture("inTex",    texFbo.getTexture(), 0);
		shadwShader.setUniformTexture("unlitTex", unlitFbo.getTexture(), 1);
		shadwShader.setUniformTexture("shadwTex", unlitFbo.getTexture(), 2);
		ofDrawRectangle(0, 0, fboWid, fboHei);
		shadwShader.end();
	blendFbo.end();
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
