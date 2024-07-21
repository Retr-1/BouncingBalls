#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "vec.h"
#include "physics.h"

int randint(int a, int b) {
	// returns from [a,b)
	if (a > b)
		std::swap(a, b);

	return std::rand() % (b - a + 1) + a;
}


// Override base class with your custom functionality

class Window : public OlcPhysicsWindow
{
public:
	Window()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		OlcPhysicsWindow::OnUserCreate();


		const int n_balls = 10;//(ScreenWidth() - gap) / (2*r+gap);
		const float gap = 30;
		const float row_gap = 20;
		const float d = (ScreenWidth()-(n_balls-1)*gap) / n_balls;
		const float r = d / 2;
		const int n_rows = 5;
		
		float y = 300;
		for (int i = 0; i < n_rows; i++) {
			float x = i % 2 == 0 ? r : gap/2+d;
			for (int j = 0; j < n_balls-i%2; j++) {
				StaticBall sball;
				sball.r = r;
				sball.pos = vec2d<float>(x, y);
				engine.static_balls.emplace_back(sball);

				x += d + gap;
			}
			y += d + row_gap;
		}

		const float fall_r = r / 3;
		for (int i = fall_r; i < 200; i+=fall_r*2+20) {
			for (int j = fall_r; j < ScreenWidth(); j+=fall_r*2+20) {
				Ball b;
				b.r = fall_r;
				b.mass = 10;
				b.pos.x = j;
				b.pos.y = i;
				engine.balls.emplace_back(b);
			}
		}

		const int n_pockets = 5;
		const int pocket_width = ScreenWidth() / n_pockets;
		const float cap_r = 10;
		const int cap_height = 100;
		for (int x = cap_r; x < ScreenWidth(); x += pocket_width) {
			Capsule cap_left;
			cap_left.start.x = x;
			cap_left.start.y = ScreenHeight();
			cap_left.end.x = x;
			cap_left.end.y = ScreenHeight() - cap_height;
			cap_left.r = cap_r;

			Capsule cap_right;
			cap_right.start.x = x + pocket_width - cap_r*2;
			cap_right.start.y = ScreenHeight() - cap_height;
			cap_right.end.x = x + pocket_width - cap_r*2;
			cap_right.end.y = ScreenHeight();
			cap_right.r = cap_r;

			Capsule cap_down;
			cap_down.start.y = ScreenHeight() - cap_r;
			cap_down.start.x = x;
			cap_down.end.x = x + pocket_width;
			cap_down.end.y = ScreenHeight() - cap_r;
			cap_down.r = cap_r;

			engine.capsules.emplace_back(cap_left);
			engine.capsules.emplace_back(cap_right);
			engine.capsules.emplace_back(cap_down);
		}

		engine.gravity = true;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		engine.update(fElapsedTime);
		draw();
		return true;
	}
};


int main()
{
	//vec2d<float> a(5, 4);
	//vec2d<float> b(3, 1);
	//std::cout << (a + b).str();
	//std::cout << (a - b).str();
	//std::cout << (a / 3).str();
	//std::cout << (a * b) << "\n";
	//a += b;
	//std::cout << a.str();

	//vec2d<float> m(10, 5);
	//vec2d<int> n(3, 4);
	//std::cout << (m - n).str() << "\n";
	//std::cout << (m * n) << "\n";
	//std::cout << (n * m) << "\n";

	//int a = 5;
	//float b = 3;
	//std::cout << a / b;

	//vec2d<int> h(7, 5);
	//std::cout << (h / 3.0f).str();

	//vec2d<std::string> c;
	//std::cout << c.str();

	//Ball b;

	//vec2d<int> a(1, 2);
	//vec2d<float> b = a;
	//std::cout << b.str();

	Window win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();

	return 0;
}