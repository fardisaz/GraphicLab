#pragma once
#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <glm/glm.hpp>
#include <map>
#include "shape.h"
#include "render.h"

using namespace std;

//structure of 2d vector
struct Vector2 {
	double x;
	double y;
};

namespace optimization
{
	//main class for creating boundaries
	class Wall
	{
		Vector2 Point1, Point2;
		string WallName;
	public:
		Wall(const Vector2& p1, const Vector2& p2, string name);

		const Vector2& get_point1() const { return Point1; }
		const Vector2& get_point2() const { return Point2; }
		string get_name() { return WallName; }

		Vector2 closest_point(const Vector2& point);
		double distance_to(const Vector2& point);

	};

	class optimize
	{
	private:
		shape* room;
		double l, w;//length and width of the floor
		vector<Wall> room_walls;
		map<string, shape> priorFurnitures;
		Vector2 door_visible_space;
		double door_visibility_prior;
		double temperature;

	public:
		optimize(shape* room);
		~optimize();
		void debug();
		void GetWalls();
		void GetFurnitures();
		void SetFurnitures(vector<shape>);
		void GetDoorVisibilityPriors();
		double GetDistanceFromClosestWall(Vector2);
		string GetClosestWallName(Vector2);
		double GetFurnitureDiagonal(Vector2);
		double PriorDistanceCost(vector<shape>);
		double PriorOrientationCost(vector<shape>);
		double PriorPairwiseDistanceCost(vector<shape>);
		double AccessibilityCost(vector<shape>);
		double VisibilityCost(vector<shape>);//TODO use visibility cost at least for the door
		double TotalCost(double, double, double, double, double);
		double CalculateCostFunctions(vector<shape>);
		//double AcceptanceProbability(double, double, double);
		vector<shape> GenerateRandomMovement(vector<shape>);
		vector<shape> SwapRandomObjects(vector<shape>);
		vector<shape> GenerateRandomTheta(vector<shape>);//TODO trick theta so it nevers rotates more than 180degrees
		vector<shape> RandomInitialPosition(vector<shape>);
		void DebugSimAn(int, double, double);
		void HillClimbing();
		double getTemp() { return temperature; }
		vector<shape> FixAngle(vector<shape>);

	};

}

#endif