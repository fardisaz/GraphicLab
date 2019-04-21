#include <iostream>
#include <typeinfo>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shape.h"
#include "render.h"
#include "optimization.h"
//#include "optimization.h"
using namespace visualisation;
//using namespace optimization;
using namespace optimization;
shape* testexample() {

	double width = 25.0;//for floor, ceiling, front and back wall
	double length = 20.0;//floor, ceiling, left and rigth wall
	double height = 10.0;//only for walls
	auto door = new shape("door", "door");
	door->setTexture("resources/door/door1.jpg");
	auto window = new shape("window", "window");
	window->setTexture("resources/window/window1.jpg");
	auto poster = new shape("poster", "poster");
	poster->setTexture("resources/poster/poster1.jpg");
	auto fwall = new shape("fwall", "wall");
	fwall->setLength(height);
	fwall->setWidth(width);
	fwall->setTexture("resources/wall/wall.jpg");
	fwall->children.push_back(door);
	auto bwall = new shape("bwall", "wall");
	bwall->setLength(height);
	bwall->setWidth(length);
	bwall->setTexture("resources/wall/wall.jpg");
	bwall->children.push_back(window);
	bwall->children.push_back(poster);
	auto lwall = new shape("lwall", "wall");
	lwall->setLength(height);
	lwall->setWidth(length);
	lwall->setTexture("resources/wall/wall.jpg");
	auto rwall = new shape("rwall", "wall");
	rwall->setLength(height);
	rwall->setWidth(width);
	rwall->setTexture("resources/wall/wall.jpg");
	/*

	rwall->children.push_back(poster);*/
	auto floor = new shape("floor", "floor");
	floor->setLength(length);
	floor->setWidth(width);
	floor->setTexture("resources/floor/floor.jpg");
	//table
	auto table = new shape("table", "furniture");
	table->setPosition(11.0f, -3.5f, 0.0f);
	table->setScale(5.0f, 3.0f, -2.0f);
	table->setAngle(270.0);
	table->setModelName("resources/table/table.obj");
	//chair
	auto chair = new shape("chair", "furniture");
	chair->setPosition(9.0f, -3.0f, 0.0f);
	chair->setScale(2.0f, 3.0f, -2.0f);
	chair->setAngle(270.0);
	chair->setPair(table->getName());
	chair->setModelName("resources/chair/chair.obj");
	//floor->children.push_back(table);
	//floor->children.push_back(chair);
	//cube

	

	//rack
	auto rack = new shape("rack", "furniture");
	rack->setPosition(-11.5f, -2.0f, -6.0f);
	rack->setScale(4.0f, 6.0f, -2.0f);
	rack->setAngle(270.0);
	rack->setModelName("resources/rack/rack.obj");
	

	
	floor->children.push_back(table);
	floor->children.push_back(chair);
	floor->children.push_back(rack);
	auto room = new shape("room", "room");
	room->children.push_back(fwall);
	room->children.push_back(bwall);
	room->children.push_back(lwall);
	room->children.push_back(rwall);
	room->children.push_back(floor);

	return room;
}

int main()
{

	auto room = new shape("room", "room");
	room = testexample();
	optimization::optimize optimizer(room);

}