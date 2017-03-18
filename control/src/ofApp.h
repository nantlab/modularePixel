#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "model.h"

class ofApp : public ofBaseApp {

	public:
		~ofApp(){
		}

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void tick(){
			_model.black();
			switch(_program){
			 case 0: {//random
				for(int i = 0; i < _size; i++){
					int column = ofRandom(_model.getWidth());
					int row = ofRandom(_model.getHeight());
					_model.toggle(column, row);
					if(_double){
						_model.toggle(column+1, row);
					}
				}
				 break;
			 }

			 case 1: {//rows
				_index++;
				int row = _index % _model.getHeight();
				if(_random){
					row = ofRandom(0, _model.getHeight());
				}
				for(int column = 0; column < _model.getWidth(); column++){
					_model.set(column, row, 1);
					if(_double){
						_model.set(column, row+1, 1);
					}
				}
				 break;
			 }
			case 2: {//columns
				_index++;
				int column = _index % _model.getWidth();
				if(_random){
					column = ofRandom(0, _model.getWidth());
				}
				for(int row = 0; row < _model.getHeight(); row++){
					_model.set(column, row, 1);
					if(_double){
						_model.set(column+1, row, 1);
					}
				}
				break;
			}
			case 3: {//squares
				int column = ofRandom(0, _model.getWidth());
				int row = ofRandom(0, _model.getHeight());

				_model.set(column, row, 1);
				_model.set(column+1, row, 1);
				_model.set(column, row+1, 1);
				_model.set(column+1, row+1, 1);
				break;
			}
			case 4: {//threes
				int column = ofRandom(0, _model.getWidth());
				int row = ofRandom(0, _model.getHeight());

				bool horizontal = true;
				if(ofRandom(1) > .5){
					horizontal = false;
				}
				_model.set(column, row, 1);
				if(horizontal){
					_model.set(column+1, row, 1);
					_model.set(column+2, row, 1);
					if(_double){
						_model.set(column, row+1, 1);
						_model.set(column+1, row+1, 1);
						_model.set(column+2, row+1, 1);
					}
				}else{
					_model.set(column, row+1, 1);
					_model.set(column, row+2, 1);
					if(_double){
						_model.set(column+1, row, 1);
						_model.set(column+1, row+1, 1);
						_model.set(column+1, row+2, 1);
					}
				}
				break;
			}
			}


			ofxOscMessage message;
			message.setAddress("/set");
			ofBuffer buffer;
			buffer.allocate(_model.getWidth() * _model.getHeight());
			for(int column = 0; column < _model.getWidth(); column++){
				for(int row = 0; row < _model.getHeight(); row++){
					buffer.getData()[_model.getIndex(column, row)] = _model.get(column, row);
//					_oscSender.sendMessage(message);
				}
			}
			message.addBlobArg(buffer);
			_oscSender.sendMessage(message);
		}

	private:
		model _model;
		ofxOscReceiver _oscReceiver;
		ofxOscReceiver _remoteControlOscReceiver;
		ofxOscSender _oscSender;
		ofxPanel _gui;
		ofParameterGroup _parameters;
		ofParameter <int> _program;
		ofParameter <bool> _random;
		ofParameter <bool> _double;
		ofParameter <int> _index;
		ofParameter <int> _size;
};
