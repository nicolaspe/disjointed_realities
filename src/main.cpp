#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;
	settings.setSize(1280, 720);

	settings.setGLVersion(4, 1);
	ofCreateWindow(settings);

	ofRunApp( new ofApp() );
}
