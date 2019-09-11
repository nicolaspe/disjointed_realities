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
	bInfo = true;
	bTex = false;
	bBackground	= false;
	bBckgndTex	= false;
	bMirrors	= false;
	bAnimated	= false;

	// init Fbos
	texFbo.allocate			(texWid, texHei, GL_RGBA);
	texBckgndFbo.allocate	(texWid, texHei, GL_RGBA);
	unlitFbo.allocate		(fboWid, fboHei, GL_RGBA);
	litFbo.allocate			(fboWid, fboHei, GL_RGBA);
	uvFbo.allocate			(fboWid, fboHei, GL_RGBA);
	bgFbo.allocate			(fboWid, fboHei, GL_RGBA);
	blendUFbo.allocate		(fboWid, fboHei, GL_RGBA);
	blendSFbo.allocate		(fboWid, fboHei, GL_RGBA);
	mirrors.allocate		(fboWid, fboHei, GL_RGBA);
	sceneBaseFbo.allocate	(fboWid, fboHei, GL_RGBA);
	sceneFullFbo.allocate	(fboWid, fboHei, GL_RGBA);

	// init shaders
	string vertex = "shaders/shader.vert";
	texShaders[0].load	(vertex, "shaders/white.frag");
	texShaders[1].load	(vertex, "shaders/basicColor.frag");
    texShaders[2].load	(vertex, "shaders/raymarch_alien.frag");
    texShaders[3].load	(vertex, "shaders/colorWall.frag");
	blendShader.load	(vertex, "shaders/maskBlend.frag");
	shadwShader.load	(vertex, "shaders/shadowBlend.frag");
	bckgndShader.load	(vertex, "shaders/clouds.frag");

	// init mesh/model
	//ofLoadImage( skyTex, "imgs/eso0932a_sphere.jpg" );
	ofLoadImage( skyTex, "imgs/sky_horizon.jpg" );
	//ofLoadImage( skyTex, "imgs/sky_sea.jpg" );
	ofLoadImage( whiteTex, "imgs/white.png" );
	skySph.setRadius( 5000 );
	skyCyl.set( 2500, 25000 );
	skyCyl.setPosition(0, 200, 0);
	skyCyl.rotate(90, 1, 0, 0);
	model.loadModel( "models/nico_00_sit.obj" );
	model.setRotation( 1, 180, 0, 0, 1 );
	model.setPosition( 0, -100, 0 );

	// init fbx
	ofxFBXSceneSettings fbxSettings;
	if (fbxScene.load("models/nico_00.fbx", fbxSettings)) {
	//if (fbxScene.load("models/model.fbx", fbxSettings)) {
		cout << "ofApp :: fbx scene load OK" << endl;
	} else {
		cout << "ofApp :: ERROR loading fbx scene" << endl;
	}
	fbxMngr.setup( &fbxScene );
	fbxMngr.setScale( 5 );
	fbxMngr.setPosition(0, -100, 0);
	fbxMngr.setAnimation( 0 );

	// init camera
    cam1.setPosition(1500, 1200, 2500);
    cam1.lookAt(ofVec3f(0, 1000, 000));
	cam1.setFarClip(10000.);

	// init light
	ambLight.setAmbientColor(ofColor(255));
	dirLight.setDirectional();
	dirLight.rotateDeg(90, 0, 1, 0);
	

	// init mirrors
	initMirrors();
}

//--------------------------------------------------------------
void ofApp::update(){
	fbxMngr.update();
	fbxMngr.lateUpdate();

	updateMirrors();
}

//--------------------------------------------------------------
void ofApp::draw(){
	//ofBackground(120 + 120*sin(ofGetElapsedTimef()));

	// render fbos by themselves
	ofEnableDepthTest();

	render2dBackground();

	renderTexShaders();
	renderBackgroundTex();
	render2dUVTextured();
	render2dUnlitMask();
	render2dLitMask();

	textureUnlitBlend();
	textureShadwBlend();

	renderMirrorViews();
	renderFullScene();

	//// display fbos on screen
	//ofDisableDepthTest();
	//if (bBackground) { bgFbo.draw(0, 0); }
	//else {}

	//if (bTex) { texFbo.draw(0, 0); }
	//else {}

	//if (bMapMode) {
	//	uvFbo.draw(0, 0);
	//}
	//else{
	//	if (bDebug) {
	//		if (!bLight) { unlitFbo.draw(0, 0); }
	//		else { litFbo.draw(0, 0); }
	//	}
	//	else {
	//		if (!bLight) { blendUFbo.draw(0, 0); }
	//		else { blendSFbo.draw(0, 0); }
	//	}
	//}


	//// mirrors!
	//if (bMirrors) {
	//	cam1.begin();
	//	// for each mirror
	//	for (size_t i = 0; i < N_MIRRORS; i++) {
	//		// bind the corresponding texture
	//		mirrorFbo[i].getTexture().bind();
	//		// and draw the mirror
	//		mirrorMsh[i].draw();
	//		//mirrorMsh[i].drawNormals( 20 );
	//		// unbind texture and reset position
	//		mirrorFbo[i].getTexture().unbind();
	//	}
	//	cam1.end();
	//}

	sceneFullFbo.draw(0, 0);
}


//--- MIRRORS --------------------------------------------------
void ofApp::initMirrors() {
	// initialize mirror dimensions & center positions
	// mirror 0
	mirrorDim[0] = ofVec2f(512, 1024);
	mirrorPos[0] = ofVec3f( 800,   0,  800);
	// mirror 1
	mirrorDim[1] = ofVec2f(512, 1024);
	mirrorPos[1] = ofVec3f(-800,   0,  800);
	// mirror 2
	mirrorDim[2] = ofVec2f(512, 1024);
	mirrorPos[2] = ofVec3f( 800, 512, -500);
	//// mirror 3
	mirrorDim[3] = ofVec2f(512, 1024);
	mirrorPos[3] = ofVec3f(-500, 512, -800);

	// create each mirror surface and camera
	for (size_t i = 0; i < N_MIRRORS; i++) {
		// calculate distance from mirror to main camera
		ofVec3f d = cam1.getGlobalPosition() - mirrorPos[i];
		mirrorCam[i].setGlobalPosition( mirrorPos[i] - d );
		mirrorCam[i].lookAt( cam1.getGlobalPosition() );
		mirrorCam[i].setFarClip( 10000. );
		// create viewport and mesh according to dimensions
		mirrorFbo[i].allocate( mirrorDim[i].x, mirrorDim[i].y );
		mirrorMsh[i].set( mirrorDim[i].x, mirrorDim[i].y );
		mirrorMsh[i].setPosition( mirrorPos[i] );
		mirrorMsh[i].lookAt( ofVec3f(0, 0, 0) );
		// set the camera look at as the normal of the plane
		ofVec3f n = mirrorMsh[i].getOrientationEuler();
		mirrorCam[i].setOrientation( -n );
		// add to the specific 3d scene
		sceneFull.push_back( mirrorMsh[i] );
	}
}
void ofApp::updateMirrors() {
	// for each mirror
	for (size_t i = 0; i < N_MIRRORS; i++) {
		// calculate reflection vector
		ofVec3f v   = mirrorPos[i] - cam1.getGlobalPosition();
		ofVec3f n   = mirrorMsh[i].getOrientationEuler();
		ofVec3f v_r = v - 2*n *( n.dot(v)/n.dot(n) );
		// update each mirror camera position
		mirrorCam[i].setGlobalPosition( mirrorPos[i] - v_r );
		// update camera direction
		mirrorCam[i].lookAt( mirrorMsh[i].getLookAtDir() );

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
void ofApp::renderBackgroundTex() {
	texBckgndFbo.begin();
	ofClear(0, 0);
		bckgndShader.begin();
		bckgndShader.setUniform1f("u_time", ofGetElapsedTimef() / 2.);
		bckgndShader.setUniform2f("u_resolution", texWid, texHei);
		ofDrawRectangle(0, 0, texWid, texHei);
		bckgndShader.end();
	texBckgndFbo.end();
}

void ofApp::render2dBackground() {
	bgFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		skyTex.bind();
		skySph.draw();
		skyTex.unbind();
	cam1.end();
	bgFbo.end();
}

void ofApp::render2dUVTextured() {
	uvFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		dirLight.enable();
		texFbo.getTexture().bind();
		if (!bAnimated) { model.drawFaces(); }
		else { fbxMngr.draw(); }
		texFbo.getTexture().unbind();
		dirLight.disable();
	cam1.end();
	uvFbo.end();
}

void ofApp::render2dUnlitMask() {
	unlitFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		// disable directional lights
		dirLight.disable();
		// and only use flat ambient light
		ambLight.enable();
		whiteTex.bind();
		if (!bAnimated) { model.drawFaces(); }
		else { fbxMngr.draw(); }
		whiteTex.unbind();
		ambLight.disable();
	cam1.end();
	unlitFbo.end();
}
void ofApp::render2dLitMask() {
	litFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		dirLight.enable();
		whiteTex.bind();
		if (!bAnimated) { model.drawFaces(); }
		else { fbxMngr.draw(); }
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
				// render background
				// -- select texture
				if(bBckgndTex) { skyTex.bind(); }
				else { texBckgndFbo.getTexture().bind(); }
				// -- render mesh
				if (bBackground) { skySph.draw(); }
				else { skyCyl.draw(); }
				// -- unbind texture
				if (bBckgndTex) { skyTex.unbind(); }
				else { texBckgndFbo.getTexture().unbind(); }
				// render model
				dirLight.enable();
				texFbo.getTexture().bind();
				if (!bAnimated) { model.drawFaces(); }
				else { fbxMngr.draw(); }
				texFbo.getTexture().unbind();
				dirLight.disable();
			mirrorCam[i].end();
		mirrorFbo[i].end();
	}
}

void ofApp::renderBaseScene() {

}
void ofApp::renderFullScene() {
	sceneFullFbo.begin();
	ofClear(0, 0);
	cam1.begin();
		// render background
		// -- select texture
		if (bBckgndTex) { skyTex.bind(); }
		else { texBckgndFbo.getTexture().bind(); }
		// -- render mesh
		if (bBackground) { skySph.draw(); }
		else { skyCyl.draw(); }
		// -- unbind texture
		if (bBckgndTex) { skyTex.unbind(); }
		else { texBckgndFbo.getTexture().unbind(); }
		// lights
		dirLight.enable();
		// render mirrors
		if (bMirrors) {
			for (size_t i = 0; i < N_MIRRORS; i++) {
				// bind the corresponding texture
				mirrorFbo[i].getTexture().bind();
				// and draw the mirror
				mirrorMsh[i].draw();
				//mirrorMsh[i].drawNormals( 20 );
				// unbind texture and reset position
				mirrorFbo[i].getTexture().unbind();
			}
		}
		// render objects
		/*for (size_t i = 0; i < sceneMask.size(); i++); {

		}*/
		texFbo.getTexture().bind();
		if (!bAnimated) { model.drawFaces(); }
		else { fbxMngr.draw(); }
		texFbo.getTexture().unbind();
		// lights off
		dirLight.disable();
	cam1.end();
	sceneFullFbo.end();
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
		case 'v':
			bBckgndTex = !bBckgndTex;
			break;
		case 't':
			bTex = !bTex;
			break;
		case 'x':
			bMirrors = !bMirrors;
			break;
		case 'a':
			bAnimated = !bAnimated;
			break;
		case ' ':
			fbxMngr.getCurrentAnimation().togglePlayPause();
			break;
		case 's':		// save image
			ofPixels  pix;
			sceneFullFbo.readToPixels( pix );
			stringstream ss;
			ss << "img_" << ofGetUnixTime() << ".png";
			ofSaveImage(pix, ss.str());
			break;
	}

	if (fbxScene.getNumAnimations() > 1) {
		if (key == OF_KEY_DOWN) {
			int newAnimIndex = fbxMngr.getCurrentAnimationIndex() + 1;
			if (newAnimIndex > fbxScene.getNumAnimations() - 1) {
				newAnimIndex = 0;
			}
			fbxMngr.setAnimation(newAnimIndex);
			cout << "new anim: " << newAnimIndex << endl;
		}
		else if (key == OF_KEY_UP) {
			int newAnimIndex = fbxMngr.getCurrentAnimationIndex() - 1;
			if (newAnimIndex < 0) {
				newAnimIndex = fbxScene.getNumAnimations() - 1;
			}
			fbxMngr.setAnimation(newAnimIndex);
			cout << "new anim: " << newAnimIndex << endl;
		}
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
