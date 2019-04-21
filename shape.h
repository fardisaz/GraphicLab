#pragma once
#ifndef SHAPE_H
#define SHAPE_H


#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
using namespace std;

class shape
{
private:
	// --- Variables ----------------------		
	string Name;
	string Type;
	const char* Texture;
	string ModelName;
	glm::vec3 scale;
	glm::vec3 position;
	double Angle;
	double Length;//for walls, floor and ceiling
	double Width;//for walls, floor and ceiling
	string PairName;

public:
	// --- Variables ----------------------
	std::vector<shape*> children;

	// --- Methods ------------------------	
	void setName(string name) { Name = name; }
	string getName() { return Name; }
	void setType(string type) { Type = type; }
	string getType() { return Type; }
	void setTexture(const char* texture) { Texture = texture; }
	const char* getTexture() { return Texture; }
	void setModelName(std::string model) { ModelName = model; }
	std::string getModelName() { return ModelName; }
	void setScale(double sx, double sy, double sz) { scale.x = sx; scale.y = sy; scale.z = sz; }
	glm::vec3 getScale() { return scale; }
	void setPosition(double sx, double sy, double sz) { position.x = sx; position.y = sy; position.z = sz; }
	glm::vec3 getPosition() { return position; }
	void setAngle(double angle) { Angle = angle; }
	double getAngle() { return Angle; }
	void setLength(double length) { Length = length; }
	double getLength() { return Length; }
	void setWidth(double w) { Width = w; }
	double getWidth() { return Width; }
	void setPair(string pairName) { PairName = pairName; }
	string getPair() { return PairName; }


	shape(std::string name = "name", std::string type = "type")
	{
		Name = name;
		Type = type;
		setAngle(0);
		setPosition(0, 0, 0);
		setScale(1, 1, 1);
		setPair("");
	}
	~shape() {}
};

#endif