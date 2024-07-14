#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "vec.h"

class Window;

class Ball {
public:
	vec2d<int> pos;
	vec2d<float> v;
	vec2d<float> a;
	float mass;
	float r;

	bool is_touching(const Ball& other) {
		float distance = (pos - other.pos).mag();
		return distance <= r + other.r;
	}

	bool is_inside(int x, int y) {
		return (pos - vec2d<int>(x, y)).mag() <= r;
	}

	void draw(Window* canvas);

	static void collide(Ball& b1, Ball& b2) {
		auto pos_diff = b2.pos - b1.pos;
		auto part = (pos_diff * 2 * ((b2.v - b1.v) * pos_diff)) / ((b1.mass + b2.mass) * (pos_diff * pos_diff));
		b1.v += part * b2.mass;
		b2.v += part * -b1.mass;
	}
};


// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
private:
	std::vector<Ball> balls;
	Ball* selected = nullptr;
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
		Ball b1, b2, b3;
		b1.pos.x = 100;
		b1.pos.y = 200;
		b1.mass = 10;
		b1.r = 20;

		b2.pos.x = 200;
		b2.pos.y = 500;
		b2.mass = 100;
		b2.r = 100;

		b3.pos.x = 500;
		b3.pos.y = 300;
		b3.mass = 50;
		b3.r = 50;

		balls.push_back(b1);
		balls.push_back(b2);
		balls.push_back(b3);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		Clear(olc::BLACK);

		if (GetMouse(olc::Mouse::LEFT).bPressed) {
			for (Ball& b : balls) {
				if (b.is_inside(GetMouseX(), GetMouseY())) {
					selected = &b;
					break;
				}
			}
		}

		if (GetMouse(olc::Mouse::LEFT).bReleased) {
			selected = nullptr;
		}

		if (selected != nullptr) {
			selected->pos.x = GetMouseX();
			selected->pos.y = GetMouseY();
		}

		for (Ball& b : balls) {
			b.draw(this);
		}

		return true;
	}
};

void Ball::draw(Window* canvas) {
	canvas->DrawCircle(olc::vi2d(pos.x, pos.y), r);
}

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

	Window win;
	if (win.Construct(600, 600, 1, 1))
		win.Start();

	return 0;
}