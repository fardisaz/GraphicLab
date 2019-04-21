#include "optimization.h"

//declare global for generating random numbers
random_device rd;
mt19937 generator(rd());

//set point to vector
Vector2 set_points(double x, double y) {
	Vector2 pass_value;
	pass_value.x = x;
	pass_value.y = y;

	return pass_value;
}

//helper functions for adding, subtracting and multiplying vectors
Vector2 add(const Vector2& a, const Vector2& b) {
	Vector2 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;

	return c;
}

Vector2 substract(const Vector2& a, const Vector2& b) {
	Vector2 c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;

	return c;
}

Vector2 multiply(double s, const Vector2& b) {
	Vector2 c;
	c.x = s * b.x;
	c.y = s * b.y;

	return c;
}

double dot_product(const Vector2& a, const Vector2& b) {
	double c;
	c = (a.x*b.x) + (a.y*b.y);

	return c;
}

//helper function calculating length
double length(const Vector2& a) {
	double c;
	c = sqrt(pow(a.x, 2) + pow(a.y, 2));

	return c;
}

//We translate our problem to 2d so we take coordinates x, z
//we take x as x and y as z 
Vector2 glmToVector2(glm::vec3 a) {
	Vector2 b;
	b.x = a.x;
	b.y = a.z;

	return b;
}

namespace optimization
{

	//wall class constructor
	Wall::Wall(const Vector2& p1, const Vector2& p2, string name) {//line constructor
		Point1 = p1;
		Point2 = p2;
		WallName = name;
	}

	//calculate closest point of walls to another point
	Vector2 Wall::closest_point(const Vector2& point) {//finds closest point in line
		Vector2 cl_point, a, b;
		a = get_point1();
		b = get_point2();
		cl_point = substract(a, multiply((dot_product(substract(a, point), substract(b, a))) / (dot_product(substract(b, a), substract(b, a))), substract(b, a)));

		return cl_point;
	}

	//calulate distance of wall to point
	double Wall::distance_to(const Vector2& point) {//calculate distance between 2 points
		return length(substract(closest_point(point), point));
	}

	optimize::optimize(shape* room) {//constructor
		this->room = room;
		GetWalls();
		GetFurnitures();
		GetDoorVisibilityPriors();//The visibility of door is of high importance so there should not be any object infront of it
		visualisation::render test(room); //positive example
		HillClimbing();
	}

	optimize::~optimize() {//destructor

	}

	void optimize::GetWalls() {//get for walls where object cannot get out
		//Get length and width of floor so we can create coordinates for optimization
		for (auto child : room->children)
		{
			if (child->getType() == "floor")
			{
				l = child->getLength();
				w = child->getWidth();
			}
		}

		//create 4 walls
		Wall wall1(set_points(-w / 2.0, l / 2.0), set_points(w / 2.0, l / 2.0), "Wall1");
		Wall wall2(set_points(w / 2.0, l / 2.0), set_points(w / 2.0, -l / 2.0), "Wall2");
		Wall wall3(set_points(w / 2.0, -l / 2.0), set_points(-w / 2.0, -l / 2.0), "Wall3");
		Wall wall4(set_points(-w / 2.0, -l / 2.0), set_points(-w / 2.0, l / 2.0), "Wall4");

		room_walls.push_back(wall1);
		room_walls.push_back(wall2);
		room_walls.push_back(wall3);
		room_walls.push_back(wall4);
	}

	void optimize::GetFurnitures() {//get furnitures from tree for faster calculation
		for (auto child : room->children)
		{
			if (child->getType() == "floor")
			{
				for (int j = 0; j < child->children.size(); ++j)//iteration through the children of floor
				{
					shape* currentFurniture = child->children[j];
					priorFurnitures.insert(make_pair(currentFurniture->getName(), *currentFurniture));//
				}
			}
		}
	}

	void optimize::SetFurnitures(vector<shape> newFurnitures) {//set furnitures in room
		for (auto child : room->children)
		{
			if (child->getType() == "floor")
			{
				for (int j = 0; j < child->children.size(); ++j)
				{
					shape newFur = newFurnitures[j];
					child->children[j] = new shape(newFur);
				}
			}
		}
	}

	void optimize::GetDoorVisibilityPriors() {//get priors from the positive tree example we have

		for (auto child : room->children)
		{
			if (child->getType() == "wall" && child->children.size() > 0)//the children of wall is door,window,poster,...
			{
				for (auto door : child->children)
				{
					if (door->getType() == "door")//if the children of the wall is door
					{
						if (child->getName() == "fwall")//if the door is on front wall
						{
							door_visibility_prior = (child->getLength()*0.7) / 4;
							door_visible_space = set_points(w / 4, (l / 2) - door_visibility_prior);
						}
						else if (child->getName() == "bwall")
						{
							door_visibility_prior = (child->getLength()*0.7) / 4;
							door_visible_space = set_points(-w / 4, (-l / 2) + door_visibility_prior);
						}
						else if (child->getName() == "lwall")
						{
							door_visibility_prior = (child->getLength()*0.7) / 4;
							door_visible_space = set_points((-w / 2) + door_visibility_prior, l / 4);
						}
						else if (child->getName() == "rwall")
						{
							door_visibility_prior = (child->getLength()*0.7) / 4;
							door_visible_space = set_points((w / 2) - door_visibility_prior, -l / 4);
						}
					}
				}
			}
		}
	}

	double optimize::GetDistanceFromClosestWall(Vector2 center) {
		double closestDistance;
		//get all prior distances(distance to closest wall)
		int j = 0;
		for (auto wall : room_walls)
		{
			if (j == 0)
			{
				closestDistance = wall.distance_to(center);
				j++;
			}
			else {
				if (closestDistance > wall.distance_to(center))
				{
					closestDistance = wall.distance_to(center);
				}
			}
		}

		return closestDistance;
	}

	string optimize::GetClosestWallName(Vector2 center) {
		double closestDistance;
		string wallName;
		//we get all prior distances(distance to closest wall)
		int j = 0;
		for (auto wall : room_walls)
		{
			if (j == 0)
			{
				closestDistance = wall.distance_to(center);
				wallName = wall.get_name();
				j++;
			}
			else {
				if (closestDistance > wall.distance_to(center))
				{
					closestDistance = wall.distance_to(center);
					wallName = wall.get_name();
				}
			}
		}

		return wallName;
	}

	double optimize::GetFurnitureDiagonal(Vector2 size) {
		double wf = size.x;
		double lf = size.y;

		return sqrt(pow(lf, 2) + pow(wf, 2)) / 2;
	}

	//Prior distance cost Cd_pr
	double optimize::PriorDistanceCost(vector<shape> newFurnitures) {//calculate prior distance cost
		double Cd_pr = 0;
		double currentDistance;
		double priorDistance;

		//calculate distance to the closest wall and save them in vector d
		for (auto currentFur : newFurnitures) {

			Vector2 currentFurnitureCenter = glmToVector2(currentFur.getPosition());

			shape currentPriorFurniture = priorFurnitures[currentFur.getName()];
			Vector2 currentPriorFurnitureCenter = glmToVector2(currentPriorFurniture.getPosition());

			currentDistance = GetDistanceFromClosestWall(currentFurnitureCenter);
			priorDistance = GetDistanceFromClosestWall(currentPriorFurnitureCenter);

			Cd_pr += 1.1*sqrt(pow((currentDistance - priorDistance), 2));
		}
		return Cd_pr;
	}

	//Prior orientation cost Ctheta_pr 
	double optimize::PriorOrientationCost(vector<shape> newFurnitures) {//calculate orientation cost
		double Ctheta_pr = 0;
		for (auto currentFur : newFurnitures) {
			shape currentPriorFurniture = priorFurnitures[currentFur.getName()];

			Ctheta_pr += sqrt(pow((currentFur.getAngle() - currentPriorFurniture.getAngle()), 2)); //priorTheta
		}
		return Ctheta_pr;
	}

	//Prior pairwise distance cost Cd_pair
	double optimize::PriorPairwiseDistanceCost(vector<shape> newFurnitures) {//calculate pair distance cost
		double Cd_pair = 0;
		double currentDistance;
		double priorDistance;
		shape currentFurniture = shape();
		shape currentPairFurniture = shape();
		shape currentPriorFurniture = shape();
		shape currentPriorPairFurniture = shape();

		//calculate distance between pairwise objects
		for (auto currentFur1 : newFurnitures) {

			if (currentFur1.getPair() == "") continue;
			currentFurniture = currentFur1;
			currentPriorFurniture = priorFurnitures[currentFurniture.getName()];

			for (auto currentFur2 : newFurnitures) {

				if (currentFur2.getName() != currentFurniture.getPair()) continue;

				currentPairFurniture = currentFur2;

				currentPriorPairFurniture = priorFurnitures[currentPairFurniture.getName()];

				Vector2 currentCenter = glmToVector2(currentFurniture.getPosition());
				Vector2 pairCenter = glmToVector2(currentPairFurniture.getPosition());

				currentDistance = length(substract(currentCenter, pairCenter));

				Vector2 currentPriorCenter = glmToVector2(currentPriorFurniture.getPosition());
				Vector2 pairPriorCenter = glmToVector2(currentPriorPairFurniture.getPosition());

				priorDistance = length(substract(currentPriorCenter, pairPriorCenter));

				Cd_pair += sqrt(pow((currentDistance - priorDistance), 2));

			}
		}

		return Cd_pair;
	}

	//punish as object i overlaps with object j accesible space 
	double optimize::AccessibilityCost(vector<shape> newFurnitures) {
		double access_cost = 0;
		double Ca = 0;
		for (auto currentFur1 : newFurnitures) {
			for (auto currentFur2 : newFurnitures) {
				if (currentFur1.getName() != currentFur2.getName()) {//we dont want to check if object overlaps itself

					Vector2 iPosition = glmToVector2(currentFur1.getPosition());
					Vector2 jPosition = glmToVector2(currentFur2.getPosition());

					double iDiagonal = GetFurnitureDiagonal(glmToVector2(currentFur1.getScale()));
					double jDiagonal = GetFurnitureDiagonal(glmToVector2(currentFur2.getScale()));

					double a = length(substract(iPosition, jPosition));
					double b = 1.1*(iDiagonal + jDiagonal);

					access_cost = 1 - (a / b);
					Ca += max(0.0, access_cost);
				}
			}
		}
		return Ca;
	}

	//Visibility cost (Only implemented for door)
	double optimize::VisibilityCost(vector<shape> newFurnitures) {//TODO implement visibility cost(at least only for the door)
		double visibilityCost = 0;
		for (auto currentFur : newFurnitures)
		{
			Vector2 newPosition = glmToVector2(currentFur.getPosition());

			double distanceFromFurniture = length(substract(newPosition, door_visible_space));//distance from furniture center to door visible space rectangular center

			double sumOfDiagonals = 1.2*(GetFurnitureDiagonal(glmToVector2(currentFur.getScale())) + door_visibility_prior);
			visibilityCost += max(0.0, 1 - (distanceFromFurniture / sumOfDiagonals));
		}
		return visibilityCost;
	}

	//Overall cost function
	double optimize::TotalCost(double Ca, double Cd_pr, double Ctheta_pr, double Cv, double Cd_pair) {//calculate total system cost
		double wa = 2.0;  //weighting 
		double wd_pr = 3.0;
		double wtheta_pr = 1.0;
		double wd_pair = 1.0;
		double wv = 3.0;

		double accessibllity_cost = wa * Ca;
		double distance_cost = wd_pr * Cd_pr;
		double angle_cost = wtheta_pr * Ctheta_pr;
		double pair_cost = wd_pair * Cd_pair;
		double visibillity_cost = wv * Cv;

		double total = accessibllity_cost + distance_cost + angle_cost + pair_cost + visibillity_cost;

		return total;
	}

	double optimize::CalculateCostFunctions(vector<shape> newFurnitures) {//helper function to calculate all costs
		double Cd_pr = PriorDistanceCost(newFurnitures);
		double Ca = AccessibilityCost(newFurnitures);
		double Ctheta_pr = PriorOrientationCost(newFurnitures);
		double Cd_pair = PriorPairwiseDistanceCost(newFurnitures);//dont forget to update furntures cemters
		double Cv = VisibilityCost(newFurnitures);

		return TotalCost(Ca, Cd_pr, Ctheta_pr, Cv, Cd_pair); //Cd_pair, Ctheta_pair
	}

	//calculate acceptance probability of every random move
	/*double optimize::AcceptanceProbability(double currentTotalCost, double newTotalCost, double temp) {
		return min(exp((currentTotalCost - newTotalCost) / temp), 1.0); //Metropolis criterian 
	}*/

	vector<shape> optimize::GenerateRandomMovement(vector<shape> newFurnitures) {
		//generate random number with normal distribution
		normal_distribution<> distribution_normal(0.0, 1.0);
		//generate random number for choosing an object(candidate)
		uniform_int_distribution<> distribution_candidate(0, newFurnitures.size() - 1);

		//declaration of dp as Vector3 and a new variable to save the new p(center of object)
		Vector2 randomPosition, newPosition;
		//decalre candidate number and choose one
		int candidate;
		// candidate = distribution_candidate(generator);

		bool pass = false;

		do {
			candidate = distribution_candidate(generator);
			//set random values to dp with normal distribution 
			randomPosition = set_points(distribution_normal(generator), distribution_normal(generator));
			//replace new candidate to vector p
			newPosition = add(glmToVector2(newFurnitures[candidate].getPosition()), randomPosition);
			double currentDistance;
			//we get all prior distances(distance to closest wall)
			currentDistance = GetDistanceFromClosestWall(newPosition);

			double priorDistance = GetDistanceFromClosestWall(glmToVector2(priorFurnitures[newFurnitures[candidate].getName()].getPosition()));
			//check if we didnt extend boundaries
			if (newPosition.x > -(0.5*w) && newPosition.x < (0.5*w) && newPosition.y > -(0.5*l) && newPosition.y < (0.5*l) && currentDistance >= priorDistance)
			{
				pass = true;
			}

		} while (!pass);

		float y = newFurnitures[candidate].getPosition().y;
		newFurnitures[candidate].setPosition(newPosition.x, y, newPosition.y);
		return newFurnitures;
	}

	vector<shape> optimize::SwapRandomObjects(vector<shape> newFurnitures) {//generate random move or random swapping between 2 furnitures
		//generate random number for choosing an object(candidate)
		uniform_int_distribution<> distribution_candidate(0, newFurnitures.size() - 1);

		//declare temp variables for swapping
		glm::vec3 position1, position2;
		//declare two random candidates for swapping
		int candidate_1, candidate_2;
		candidate_1 = distribution_candidate(generator);
		candidate_2 = distribution_candidate(generator);
		//we check that candidate 1 and candidate 2 are not the same object
		while (candidate_1 == candidate_2) candidate_2 = distribution_candidate(generator);//candidate 2 should be changed

		//swap objects centers
		position1 = newFurnitures[candidate_1].getPosition();
		position2 = newFurnitures[candidate_2].getPosition();

		newFurnitures[candidate_1].setPosition(position2.x, position1.y, position2.z);
		newFurnitures[candidate_2].setPosition(position1.x, position2.y, position1.z);

		return newFurnitures;
	}


	vector<shape> optimize::GenerateRandomTheta(vector<shape> newFurnitures) {//generate random angle theta
		//generate random number with normal distribution
		normal_distribution<> distribution_normal(0.0, 1.0);
		//generate random number for choosing an object(candidate)
		uniform_int_distribution<> distribution_candidate(0, newFurnitures.size() - 1);

		double randomNumber;
		//declare candidate number and choose one
		int candidate;
		int count = 0;

		candidate = distribution_candidate(generator);
		//set random value to theta with normal distribution
		randomNumber = distribution_normal(generator);
		double oldOrientation = newFurnitures[candidate].getAngle();
		newFurnitures[candidate].setAngle(oldOrientation + randomNumber);

		while (newFurnitures[candidate].getAngle() >= 360) {
			double orientation = newFurnitures[candidate].getAngle();
			newFurnitures[candidate].setAngle(orientation - 360);
		}

		while (newFurnitures[candidate].getAngle() < 0) {
			double orientation = newFurnitures[candidate].getAngle();
			newFurnitures[candidate].setAngle(360 - orientation);
		}

		return newFurnitures;
	}

	vector<shape> optimize::FixAngle(vector<shape> newFurnitures) {
		for (int i = 0; i < newFurnitures.size(); ++i)
		{
			double priorAngle = priorFurnitures[newFurnitures[i].getName()].getAngle(); // Get prior angle
			Vector2 priorPosition = glmToVector2(priorFurnitures[newFurnitures[i].getName()].getPosition());
			double currentAngle = newFurnitures[i].getAngle();
			Vector2 currentCenter = glmToVector2(newFurnitures[i].getPosition());
			double thetaDiff = priorAngle - currentAngle;
			string currentClosestWallName = "";
			string priorClosestWallName = "";
			double initialAngleDifference = 0;

			currentClosestWallName = GetClosestWallName(currentCenter);
			priorClosestWallName = GetClosestWallName(priorPosition);

			if (priorClosestWallName == "Wall1") {
				if (priorAngle != 180.0) {
					initialAngleDifference = abs(180.0 - priorAngle);
				}
			}
			else if (priorClosestWallName == "Wall2") {
				if (priorAngle != 270.0) {
					initialAngleDifference = abs(270.0 - priorAngle);
				}
			}
			else if (priorClosestWallName == "Wall3") {
				if (priorAngle != 0.0) {
					initialAngleDifference = abs(0.0 - priorAngle);
				}
			}
			else if (priorClosestWallName == "Wall4") {
				if (priorAngle != 90.0) {
					initialAngleDifference = abs(90.0 - priorAngle);
				}
			}

			if (currentClosestWallName == "Wall1") {
				newFurnitures[i].setAngle(180.0 + initialAngleDifference + thetaDiff);
			}
			else if (currentClosestWallName == "Wall2") {
				newFurnitures[i].setAngle(270.0 + initialAngleDifference + thetaDiff);
			}
			else if (currentClosestWallName == "Wall3") {
				newFurnitures[i].setAngle(0.0 + initialAngleDifference + thetaDiff);
			}
			else if (currentClosestWallName == "Wall4") {
				newFurnitures[i].setAngle(90.0 + initialAngleDifference + thetaDiff);
			}

			while (newFurnitures[i].getAngle() >= 360) {
				double orientation = newFurnitures[i].getAngle();
				newFurnitures[i].setAngle(orientation - 360);
			}

			while (newFurnitures[i].getAngle() < 0) {
				double orientation = newFurnitures[i].getAngle();
				newFurnitures[i].setAngle(360 - orientation);
			}
		}

		return newFurnitures;
	}

	vector<shape> optimize::RandomInitialPosition(vector<shape> newFurnitures) {//generate random starting position of furnitures

		int i = 0;
		while (i < 10000) {
			newFurnitures = GenerateRandomMovement(newFurnitures);
			newFurnitures = GenerateRandomTheta(newFurnitures);
			newFurnitures = SwapRandomObjects(newFurnitures);
			i++;
		}

		return newFurnitures;
	}
	//positive examples means all the furnitures are on the right place
	void optimize::HillClimbing() {//main function for hill climbing
		//generate vectors for centers, diagonals, prior distance
		int num_evaluations = 0;
		int max_evaluations = 15000;
		vector<shape> furnitures;
		vector<shape> newFurnitures;
		double newTotalCost, currentTotalCost;
		int iteration = 0;
		int random_move; //proposed moves: translation,rotation,swapping
		uniform_real_distribution<> distribution(0.0, 1.0);//generate random number on range [0,1)
		uniform_int_distribution<> distribution_move(0, 2);//generate random number on range [0,2], for which random move to make

		for (auto fur : priorFurnitures) //get positive examples from prior furnitures
		{
			furnitures.push_back(fur.second);//second because in the map the second value is the shape
			newFurnitures.push_back(fur.second);
		}

		newFurnitures = RandomInitialPosition(newFurnitures);//give random centers and angles of objects
		furnitures = newFurnitures;
		///////////////////////////////**********************the current total cost should be negated
		currentTotalCost = -CalculateCostFunctions(newFurnitures);//calculate cost function

		while (num_evaluations < max_evaluations) {
			//generate random integer number
			random_move = distribution_move(generator);//generate random number for random move

			//generate new random position
			if (random_move == 0) {
				newFurnitures = GenerateRandomMovement(newFurnitures);
			}
			else if (random_move == 1) {
				newFurnitures = SwapRandomObjects(newFurnitures);
			}
			else {
				newFurnitures = GenerateRandomTheta(newFurnitures);//generate new angle theta
			}

			//calculate new total cost function
			////////////////////////********************also this cost will be negated
			newTotalCost = -CalculateCostFunctions(newFurnitures);

			if (newTotalCost > currentTotalCost) {
				currentTotalCost = newTotalCost;
				furnitures = newFurnitures;
			}
			else {
				newFurnitures = furnitures;
			}

			if (num_evaluations % 5000 == 0) {
				DebugSimAn(num_evaluations, currentTotalCost, newTotalCost);
				SetFurnitures(furnitures);
				visualisation::render test(room);
			}
			//update iterations
			num_evaluations++;
		}
		DebugSimAn(num_evaluations, currentTotalCost, newTotalCost);
		furnitures = FixAngle(furnitures);
		SetFurnitures(furnitures);
		visualisation::render test(room);//render final room
	}

	//for debugging pruposes
	void optimize::DebugSimAn(int iteration, double current_c, double new_c) {
		cout << "Iteration: " << iteration << endl;
		cout << "Total Cost: " << current_c << endl;
		cout << "New total cost: " << new_c << endl;
	}
}