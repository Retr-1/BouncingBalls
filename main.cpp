#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "vec.h"

int randint(int a, int b) {
	// returns from [a,b)
	if (a > b)
		std::swap(a, b);

	return std::rand() % (b - a + 1) + a;
}

class Window;

class Ball {
public:
	vec2d<float> pos;
	vec2d<float> v;
	vec2d<float> a;
	float mass;
	float r;

	bool is_intersecting(const Ball& other) {
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

	static void avoid_overlap(Ball& b1, Ball& b2) {
		vec2d<float> midline = b2.pos - b1.pos;
		float d = midline.mag();
		float target_d = b1.r + b2.r;
		if (target_d <= d)
			return;
		float mv = (target_d - d) * 0.5f;
		vec2d<float> vec_mv = midline.normalized();

		b1.pos += vec_mv * (- mv);
		b2.pos += vec_mv * mv;
		//std::cout << vec_mv.str() << '\n';
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

		for (int i = 0; i <	1; i++) {
			Ball b;
			b.r = randint(10, 100);
			b.mass = 3.14 * b.r * b.r;
			b.pos.x = randint(0, ScreenWidth());
			b.pos.y = randint(0, ScreenHeight());
			balls.push_back(b);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		Clear(olc::BLACK);

		if (GetMouse(olc::Mouse::LEFT).bPressed || GetMouse(olc::Mouse::RIGHT).bPressed) {
			for (Ball& b : balls) {
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
			selected->v = dir * m; // POSSIBLE OPTIMIZATION IF NOT MULTIPLIED BY CONST
			std::cout << selected->v.str();
			selected = nullptr;
		}

		for (Ball& b : balls) {
			b.pos += b.v * fElapsedTime;
				
			if (b.v.mag() < 0.1f) {
				b.v.x = 0;
				b.v.y = 0;
			}
			else {
				b.v -= b.v.normalized() * 50.0f * fElapsedTime;
			}

			//std::cout << b.v.str() << '\n';

			/*b.pos.x = fmodf(b.pos.x + ScreenWidth(), ScreenWidth());
			b.pos.y = fmodf(b.pos.y + ScreenHeight(), ScreenHeight());*/
			if (b.pos.x > ScreenWidth() + b.r) {
				b.pos.x = -b.r;
			}
			else if (b.pos.x < -b.r) {
				b.pos.x = ScreenWidth() + b.r;
			}

			if (b.pos.y > ScreenHeight() + b.r) {
				b.pos.y = -b.r;
			}
			else if (b.pos.y < -b.r) {
				b.pos.y = ScreenHeight() + b.r;
			}
			//b.v -= b.v * 0.01f;
		}

		for (int i = 0; i < balls.size(); i++) {
			for (int j = i + 1; j < balls.size(); j++) {
				if (balls[i].is_intersecting(balls[j])) {
					Ball::avoid_overlap(balls[i], balls[j]);
					Ball::collide(balls[i], balls[j]);
				}
			}
		}

		for (Ball& b : balls) {
			b.draw(this);
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

	//vec2d<int> a(1, 2);
	//vec2d<float> b = a;
	//std::cout << b.str();

	Window win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();

	return 0;
}