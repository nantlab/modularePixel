#ifndef MODEL_H
#define MODEL_H

#include "ofMain.h"

class model
{
	public:
		model();
		void setup(int width, int height){
			_width = width;
			_height = height;
			_values.clear();
			_values.resize(_width * _height);
		}

		void draw(){
			int length = std::min(ofGetHeight()/_height, ofGetWidth()/_width);
			int x = 0;
			int y = 0;
			for(int row = 0; row < _height; row++){
				for(int column = 0; column < _width; column++){
					if(_values[getIndex(column, row)]){
						ofSetColor(255, 0, 0);
					}else{
						ofSetColor(0, 0, 0);
					}
					ofDrawRectangle(x, y, length, length);
					x += length + 1;
				}
				x = 0;
				y += length + 1;
			}
		}

		int getWidth(){
			return _width;
		}
		int getHeight(){
			return _height;
		}
		int getIndex(int x, int y){
			auto index = y * _width + x;
			if(index >= _values.size()){
				index = _values.size() - 1;
			}
			return index;
		}

		int get(int column, int row){
			return _values[getIndex(column, row)];
		}

		void set(int column, int row, int value = 1){
			_values[getIndex(column, row)] = value;
		}
		void toggle(int column, int row){
			set(column, row, (get(column, row)+1)%2);
		}

		void black(){
			for(int i = 0; i < _values.size(); i++){
				_values[i] = 0;
			}
		}


	private:

		int _width;
		int _height;
		vector<int> _values;
};

#endif // MODEL_H
