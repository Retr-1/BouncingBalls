#pragma once
#include "olcPixelGameEngine.h"
#include "vec.h"
#include "physics.h"

class Window : public OlcPhysicsWindow
{
private:
	Ball* selected = nullptr;
	Capsule* cap_selected = nullptr;
	bool cap_start = false;

public:
	Window()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		OlcPhysicsWindow::OnUserCreate();
		// Called once at the start, so create things here
		//Ball b1, b2, b3, b4, b5;
		//b1.pos.x = 100;
		//b1.pos.y = 200;
		//b1.mass = 10;
		//b1.r = 20;

		//b2.pos.x = 200;
		//b2.pos.y = 500;
		//b2.mass = 100;
		//b2.r = 100;

		//b3.pos.x = 500;
		//b3.pos.y = 300;
		//b3.mass = 50;
		//b3.r = 50;

		//b4.pos.x = 300;
		//b4.pos.y = 100;
		//b4.r = 120;
		//b4.mass = 1000;

		//b5.pos.x = 0;
		//b5.pos.y = 20;
		//b5.r = 90;
		//b5.mass = 700;

		//balls.push_back(b1);
		//balls.push_back(b2);
		//balls.push_back(b3);
		//balls.push_back(b4);
		//balls.push_back(b5);

		for (int i = 0; i < 20; i++) {
			Ball b;
			b.r = randint(10, 100);
			b.mass = 3.14 * b.r * b.r;
			b.pos.x = randint(0, ScreenWidth());
			b.pos.y = randint(0, ScreenHeight());
			engine.balls.push_back(b);
		}

		Capsule c;
		c.start.x = 100;
		c.start.y = 100;
		c.end.x = 300;
		c.end.y = 300;
		c.r = 20;
		engine.capsules.emplace_back(c);

		StaticBall sball;
		sball.r = 50;
		sball.pos = vec2d<float>(300, 300);
		engine.static_balls.push_back(sball);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		Clear(olc::BLACK);

		if (GetMouse(olc::Mouse::LEFT).bPressed || GetMouse(olc::Mouse::RIGHT).bPressed) {
			for (Ball& b : engine.balls) {
				if (b.is_inside(GetMouseX(), GetMouseY())) {
					selected = &b;
					break;
				}
			}
		}

		if (selected && GetMouse(olc::Mouse::LEFT).bHeld) {
			selected->pos.x = GetMouseX();
			selected->pos.y = GetMouseY();
		}

		if (selected && GetMouse(olc::Mouse::RIGHT).bHeld) {
			DrawLine(olc::vi2d(selected->pos.x, selected->pos.y), GetMousePos(), olc::BLUE);
		}

		if (GetMouse(olc::Mouse::LEFT).bReleased) {
			selected = nullptr;
		}

		if (selected && GetMouse(olc::Mouse::RIGHT).bReleased) {
			vec2d<float> dir(selected->pos.x - GetMouseX(), selected->pos.y - GetMouseY());
			float m = dir.mag();
			dir.normalize();
			selected->v = dir * (m * 5.0f); // POSSIBLE OPTIMIZATION IF NOT MULTIPLIED BY CONST
			//std::cout << selected->v.str();
			selected = nullptr;
		}


		if (GetMouse(olc::Mouse::LEFT).bPressed) {
			for (Capsule& c : engine.capsules) {
				if (Ball::is_inside(c.start, c.r, GetMouseX(), GetMouseY())) {
					cap_selected = &c;
					cap_start = true;
				}
				else if (Ball::is_inside(c.end, c.r, GetMouseX(), GetMouseY())) {
					cap_selected = &c;
					cap_start = false;
				}
			}
		}

		if (GetMouse(olc::Mouse::LEFT).bHeld && cap_selected) {
			if (cap_start) {
				cap_selected->start.x = GetMouseX();
				cap_selected->start.y = GetMouseY();
			}
			else {
				cap_selected->end.x = GetMouseX();
				cap_selected->end.y = GetMouseY();
			}
		}

		if (GetMouse(olc::Mouse::LEFT).bReleased) {
			cap_selected = nullptr;
		}

		engine.update(fElapsedTime);

		for (Ball& b : engine.balls) {
			draw(b);
		}

		for (Capsule& c : engine.capsules) {
			draw(c);
		}

		for (StaticBall& sball : engine.static_balls) {
			draw(sball);
		}

		// PRINTING TOTAL KINETIC ENERGY
		//long double total_energy = 0;
		//for (Ball& b : balls) {
		//	total_energy += 0.5f * b.mass * powf(b.v.mag(), 2);
		//	//total_energy += b.mass * b.v.mag();
		//}
		//std::cout << total_energy << '\n';

		return true;
	}
};