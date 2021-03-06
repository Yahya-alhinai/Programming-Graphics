/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"
#include <time.h>

//car
constexpr auto CAR_SPEED = 0.3;
constexpr auto CAR_ROTATION = 0.04;
float Car_accel = 1;

//Bounds
constexpr auto CEIL_BOUND = 35;
constexpr auto GROUND_BOUND = 0;
constexpr auto FRONT_BOUND = 50;
constexpr auto BACK_BOUND = -50;
constexpr auto LEFT_BOUND = -40;
constexpr auto RIGHT_BOUND = 40;
constexpr auto BOUNCE_BACK = 0.01;

//Ball
constexpr auto BALL_MAX_SPEED = 5;
constexpr auto G = 0.0098;

float BALL_SPEED[3] = {((rand() % 101) - 50.0) / 50.0, ((rand() % 101) - 50.0) / 50.0, ((rand() % 101) - 50.0) / 50.0};

CarSoccer::CarSoccer() : GraphicsApp(1024, 768, "Car Soccer")
{
	// Define a search path for finding data files (images and shaders)
	searchPath_.push_back(".");
	searchPath_.push_back("./data");
	searchPath_.push_back(DATA_DIR_INSTALL);
	searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer()
{
}

Vector3 CarSoccer::joystick_direction()
{
	Vector3 dir;

	if (IsKeyDown(GLFW_KEY_LEFT))
	{
		car_.set_angle(car_.angle() + CAR_ROTATION);
	}
	if (IsKeyDown(GLFW_KEY_RIGHT))
	{
		car_.set_angle(car_.angle() - CAR_ROTATION);
	}

	if (IsKeyDown(GLFW_KEY_UP))
	{
		dir[2] -= CAR_SPEED * cos(car_.angle());
		dir[0] -= CAR_SPEED * sin(car_.angle());
		if (Car_accel < 2)
			Car_accel *= 1.02;
	}
	else if (Car_accel > 1)
	{
		Car_accel *= 0.95;
		dir[2] -= CAR_SPEED * cos(car_.angle());
		dir[0] -= CAR_SPEED * sin(car_.angle());
	}

	if (IsKeyDown(GLFW_KEY_DOWN))
	{
		dir[2] += CAR_SPEED * cos(car_.angle());
		dir[0] += CAR_SPEED * sin(car_.angle());
		//std::cout << car_.position() << std::endl;
	}

	while ((car_.position()[2] > FRONT_BOUND))
		car_.set_position(car_.position() + Vector3(0, 0, -BOUNCE_BACK));

	while (car_.position()[2] < BACK_BOUND)
		car_.set_position(car_.position() + Vector3(0, 0, BOUNCE_BACK));

	while (car_.position()[0] > RIGHT_BOUND)
		car_.set_position(car_.position() + Vector3(-BOUNCE_BACK, 0, 0));

	while (car_.position()[0] < LEFT_BOUND)
		car_.set_position(car_.position() + Vector3(BOUNCE_BACK, 0, 0));

	return dir;
}

void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers)
{
	if (key == GLFW_KEY_SPACE)
	{
		for (int i = 0; i < 3; i++)
		{
			BALL_SPEED[i] = ((rand() % 101) - 50.0) / 50.0;
		}
	}
}

void CarSoccer::UpdateSimulation(double timeStep)
{
	// Here's where you shound do your "simulation", updating the positions of the
	// car and ball as needed and checking for collisions.  Filling this routine
	// in is the main part of the assignment.
	timeStep = 100 * timeStep;

	//Air Friction + Gravity
	BALL_SPEED[0] *= 0.995;
	BALL_SPEED[1] = BALL_SPEED[1] * 0.99 - G;
	BALL_SPEED[2] *= 0.995;

	// Car acceleration
	car_.set_position(car_.position() + joystick_direction() * timeStep * Car_accel);
	ball_.set_position(ball_.position() + Vector3(BALL_SPEED[0] * timeStep, BALL_SPEED[1] * timeStep, BALL_SPEED[2] * timeStep));

	if ((car_.position() - ball_.position()).Length() <= car_.collision_radius() + ball_.radius())
	{
		//adjust the two Spheres to be just touching
		while ((car_.position() - ball_.position()).Length() <= car_.collision_radius() + ball_.radius())
			ball_.set_position(ball_.position() + 0.001 * Vector3(ball_.position()- car_.position()));

		//reflect based on the collision normal
		BALL_SPEED[0] = 3 * abs((Car_accel - 0.9)) * (ball_.position()[0] - car_.position()[0]) / (car_.collision_radius() + ball_.radius());
		BALL_SPEED[1] = 3 * abs((Car_accel - 0.9)) * (ball_.position()[1] - car_.position()[1]) / (car_.collision_radius() + ball_.radius());
		BALL_SPEED[2] = 3 * abs((Car_accel - 0.9)) * (ball_.position()[2] - car_.position()[2]) / (car_.collision_radius() + ball_.radius());
	}

	if (ball_.position()[0] - ball_.radius() < LEFT_BOUND || ball_.position()[0] + ball_.radius() > RIGHT_BOUND)
	{
		while (ball_.position()[0] - ball_.radius() < LEFT_BOUND)
			ball_.set_position(ball_.position() + Vector3(0.02, 0, 0));
		while (ball_.position()[0] + ball_.radius() > RIGHT_BOUND)
			ball_.set_position(ball_.position() + Vector3(-0.02, 0, 0));

		BALL_SPEED[0] *= -0.9;
	}

	if (ball_.position()[1] < ball_.radius() || ball_.position()[1] + ball_.radius() > CEIL_BOUND)
	{
		while (ball_.position()[1] < ball_.radius())
			ball_.set_position(ball_.position() + Vector3(0, 0.02, 0));
		while (ball_.position()[1] + ball_.radius() > CEIL_BOUND)
			ball_.set_position(ball_.position() + Vector3(0, -0.02, 0));

		BALL_SPEED[1] *= -0.9;
	}

	if (ball_.position()[2] - ball_.radius() < BACK_BOUND || ball_.position()[2] + ball_.radius() > FRONT_BOUND)
	{
		if (abs(ball_.position()[0]) <= 10 && ball_.position()[1] <= 10)
		{
			car_.Reset();
			Car_accel = 1;
			BALL_SPEED[0] = 0;
			BALL_SPEED[1] = 0;
			BALL_SPEED[2] = 0;
			ball_.Reset();
		}
		while (ball_.position()[2] - ball_.radius() < BACK_BOUND)
			ball_.set_position(ball_.position() + Vector3(0, 0, 0.02));
		while (ball_.position()[2] + ball_.radius() > FRONT_BOUND)
			ball_.set_position(ball_.position() + Vector3(0, 0, -0.02));

		BALL_SPEED[2] *= -0.9;
	}
}

void CarSoccer::InitOpenGL()
{
	// Set up the camera in a good position to see the entire field
	projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
	modelMatrix_ = Matrix4::LookAt(Point3(0, 60, 70), Point3(0, 0, 10), Vector3(0, 1, 0));

	// Set a background color for the screen
	glClearColor(0.8, 0.8, 0.8, 1);

	// Load some image files we'll use
	fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
	crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}

void CarSoccer::DrawUsingOpenGL()
{
	srand(time(NULL));

	// Draw the crowd as a fullscreen background image
	quickShapes_.DrawFullscreenTexture(Color(1, 1, 1), crowdTex_);

	// Draw the field with the field texture on it.
	Color col(16.0 / 255.0, 46.0 / 255.0, 9.0 / 255.0);
	Matrix4 M = Matrix4::Translation(Vector3(0, -0.201, 0)) * Matrix4::Scale(Vector3(50, 1, 60));
	quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
	M = Matrix4::Translation(Vector3(0, -0.2, 0)) * Matrix4::Scale(Vector3(40, 1, 50));
	quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1, 1, 1), fieldTex_);

	// Draw the car
	Color carcol(0.8, 0.2, 0.2);
	Matrix4 Mcar =
		 Matrix4::Translation(car_.position() - Point3(0, 0, 0)) *
		 Matrix4::RotationY(car_.angle()) *
		 Matrix4::Scale(car_.size()) *
		 Matrix4::Scale(Vector3(0.5, 0.5, 0.5));
	quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);

	// Draw the ball
	Color ballcol(1, 1, 1);
	Matrix4 Mball =
		 Matrix4::Translation(ball_.position() - Point3(0, 0, 0)) *
		 Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
	quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);

	// Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
	// flattens the sphere into a pancake, which we then draw just a bit
	// above the ground plane.
	Color shadowcol(0.2, 0.4, 0.15);
	Matrix4 Mshadow =
		 Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
		 Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
		 Matrix4::RotationX(90);
	quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);


	// You should add drawing the goals and the boundary of the playing area
	// using quickShapes_.DrawLines()

	Color goal_1_col(25, 25, 112);
	std::vector<Point3> goal_1;
	goal_1.push_back(Point3(10, 0, 50));
	goal_1.push_back(Point3(-10, 0, 50));
	goal_1.push_back(Point3(-10, 10, 50));
	goal_1.push_back(Point3(10, 10, 50));
	goal_1.push_back(Point3(10, 0, 50));
	for (int i = 8; i > -10; i -= 2)
	{
		goal_1.push_back(Point3(i, 0, 50));
		goal_1.push_back(Point3(i, 10, 50));
		i -= 2;
		goal_1.push_back(Point3(i, 10, 50));
		goal_1.push_back(Point3(i, 0, 50));
	}

	for (int i = 2; i < 10; i += 2)
	{
		goal_1.push_back(Point3(-10, i, 50));
		goal_1.push_back(Point3(10, i, 50));
		i += 2;
		goal_1.push_back(Point3(10, i, 50));
		goal_1.push_back(Point3(-10, i, 50));
	}
	goal_1.push_back(Point3(10, 8, 50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal_1_col, goal_1, QuickShapes::LinesType::LINE_LOOP, 0.1);

	Color goal_2_col(25, 25, 112);
	std::vector<Point3> goal_2;
	goal_2.push_back(Point3(10, 0, -50));
	goal_2.push_back(Point3(-10, 0, -50));
	goal_2.push_back(Point3(-10, 10, -50));
	goal_2.push_back(Point3(10, 10, -50));
	goal_2.push_back(Point3(10, 0, -50));

	for (int i = 8; i > -10; i -= 2)
	{
		goal_2.push_back(Point3(i, 0, -50));
		goal_2.push_back(Point3(i, 10, -50));
		i -= 2;
		goal_2.push_back(Point3(i, 10, -50));
		goal_2.push_back(Point3(i, 0, -50));
	}

	for (int i = 2; i < 10; i += 2)
	{
		goal_2.push_back(Point3(-10, i, -50));
		goal_2.push_back(Point3(10, i, -50));
		i += 2;
		goal_2.push_back(Point3(10, i, -50));
		goal_2.push_back(Point3(-10, i, -50));
	}
	goal_2.push_back(Point3(10, 8, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal_2_col, goal_2, QuickShapes::LinesType::LINE_LOOP, 0.1);

	Color fieldCol(255, 255, 255);
	std::vector<Point3> field_lines;
	field_lines.push_back(Point3(40, 0, -50));
	field_lines.push_back(Point3(40, 0, 50));
	field_lines.push_back(Point3(40, 35, 50));
	field_lines.push_back(Point3(40, 0, 50));
	field_lines.push_back(Point3(-40, 0, 50));
	field_lines.push_back(Point3(-40, 35, 50));
	field_lines.push_back(Point3(-40, 0, 50));
	field_lines.push_back(Point3(-40, 0, -50));
	field_lines.push_back(Point3(40, 0, -50));
	field_lines.push_back(Point3(40, 35, -50));
	field_lines.push_back(Point3(40, 35, 50));
	field_lines.push_back(Point3(-40, 35, 50));
	field_lines.push_back(Point3(-40, 35, -50));
	field_lines.push_back(Point3(-40, 0, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, fieldCol, field_lines, QuickShapes::LinesType::LINE_LOOP, 0.1);

}