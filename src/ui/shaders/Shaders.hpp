/*
 * Shaders.hpp
 *
 *  Created on: Jan 5, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_SHADERS_SHADERS_HPP_
#define SRC_UI_SHADERS_SHADERS_HPP_

//#include "CircleShader.hpp"
//#include "DiskShader.hpp"
//#include "PointShader.hpp"

class AuroraWindow;

class Shaders {
public:
	Shaders(AuroraWindow* window);
	~Shaders();
	
	AuroraWindow& window;
	
//	CircleShader circle;
//	DiskShader disk;
//	PointShader point;
};

#endif /* SRC_UI_SHADERS_SHADERS_HPP_ */

