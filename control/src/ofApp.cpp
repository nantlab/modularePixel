#include "ofApp.h"

void ofApp::setup(){
	_model.setup(4,6);
	_oscReceiver.setup(8000);
	_remoteControlOscReceiver.setup(8001);
	_oscSender.setup("192.168.2.3", 8000);

	_parameters.setName("parameters");
	_program.set("program", 0, 0, 4);
	_random.set("random", true);
	_size.set("size", 4,1,12);
	_double.set("double", false);

	_parameters.add(_program);
	_parameters.add(_random);
	_parameters.add(_double);
	_parameters.add(_size);
	_gui.setup(_parameters);
}
void ofApp::update(){
	while(_remoteControlOscReceiver.hasWaitingMessages()){
		ofxOscMessage message;
		_remoteControlOscReceiver.getNextMessage(message);
		auto address = message.getAddress();
		ofLogNotice()<<address;
		if(address == "/1/fader5"){
			_program = ofMap(message.getArgAsFloat(0), 0, 1, 0, _program.getMax());
		}else if(address == "/1/fader1"){
			_size = ofMap(message.getArgAsFloat(0), 0, 1, 0, _size.getMax());
		}else if(address == "/1/toggle1"){
			_random = message.getArgAsFloat(0);
		}else if(address == "/1/toggle2"){
			_double = message.getArgAsFloat(0);
		}
	}
	while(_oscReceiver.hasWaitingMessages()){
		ofxOscMessage message;
		_oscReceiver.getNextMessage(message);
		auto address = message.getAddress();
		if(address == "/beat"){
			tick();
		}else if(address == "/setPixel"){
			auto column = message.getArgAsInt(0);
			auto row = message.getArgAsInt(1);
			auto value = message.getArgAsInt(2);
			_model.set(column, row, value);
		}else if(address == "/setRow"){
			auto row = message.getArgAsInt(0);
			auto value = message.getArgAsInt(1);
			for(int column = 0; column < _model.getWidth(); column++){
				_model.set(column, row, value);
			}
		}else if(address == "/setColumn"){
			auto column = message.getArgAsInt(0);
			auto value = message.getArgAsInt(1);
			for(int row = 0; row < _model.getHeight(); row++){
				_model.set(column, row, value);
			}
		}else if(address == "/setAll"){
			auto value = message.getArgAsInt(0);
			for(int row = 0; row < _model.getHeight(); row++){
				for(int column = 0; column < _model.getWidth(); column++){
					_model.set(column, row, value);
				}
			}
		}
	}
}
void ofApp::draw(){
	_model.draw();
	_gui.draw();
}
void ofApp::keyPressed(int key){
	switch(key){
	case 'o':
	{
		ofxOscMessage message;
		message.setAddress("/setMode");
		message.addIntArg(1);
		_oscSender.sendMessage(message);
		break;
	}
	case 's':
	{
		ofxOscMessage message;
		message.setAddress("/setMode");
		message.addIntArg(0);
		_oscSender.sendMessage(message);
		break;
	}
	}
}
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y ){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
