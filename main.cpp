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
	vec2d<float> old_pos;
	vec2d<float> v;
	vec2d<float> a;
	float mass;
	float r;
	float sim_time_remaining;

	bool is_intersecting(const Ball& other) {
		float distance = (pos - other.pos).mag();
		return distance <= r + other.r;
	}

	bool is_inside(int x, int y) {
		return (pos - vec2d<int>(x, y)).mag() <= r;
	}

	void adjust_sim_time() {
		float intended_speed = v.mag();
		if (intended_speed == 0.0f) {
			return;
		}
		float actual_travelled = (pos - old_pos).mag();
		float actual_time = actual_travelled / intended_speed;
		sim_time_remaining -= actual_time;
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

	static bool is_inside(vec2d<float> circle, float r, int mx, int my) {
		float d = (circle - vec2d<float>(mx, my)).mag();
		return d <= r;
	}

};


class Capsule {
public:
	float r;
	vec2d<float> start;
	vec2d<float> end;

	void draw(Window* canvas);

	void collide(Ball& ball) {
		auto u = end - start;
		auto cap_to_ball = ball.pos - start;
		auto u_normalized = u.normalized();
		float t = std::min(1.0f, std::max(0.0f, cap_to_ball.dot(u_normalized) / u.mag()));
		auto mid = u * t + start;
		
		// static
		auto v = mid - ball.pos;
		float d = v.mag();
		if (ball.r + r < d)
			return;

		float mv = ball.r + r - d;
		mv *= 0.5;
		v.normalize();
		ball.pos -= v * mv;


		// dynamic
		float horizontal = -ball.v.dot(v);
		auto n_v = v.perpendicular();
		float vertical = ball.v.dot(n_v);
		ball.v = v * horizontal + n_v * vertical;
	}
};

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
private:
	std::vector<Ball> balls;
	Ball* selected = nullptr;

	std::vector<Capsule> capsules;
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

		for (int i = 0; i <	20; i++) {
			Ball b;
			b.r = randint(10, 100);
			b.mass = 3.14 * b.r * b.r;
			b.pos.x = randint(0, ScreenWidth());
			b.pos.y = randint(0, ScreenHeight());
			balls.push_back(b);
		}

		Capsule c;
		c.start.x = 100;
		c.start.y = 100;
		c.end.x = 300;
		c.end.y = 300;
		c.r = 20;
		capsules.emplace_back(c);

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
			selected->v = dir * (m * 5.0f); // POSSIBLE OPTIMIZATION IF NOT MULTIPLIED BY CONST
			//std::cout << selected->v.str();
			selected = nullptr;
		}


		if (GetMouse(olc::Mouse::LEFT).bPressed) {
			for (Capsule& c : capsules) {
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

		const int n_epochs = 4;
		const float f_sim_time = fElapsedTime / (float)n_epochs;
		const int n_sim_steps = 15;

		for (int e = 0; e < n_epochs; e++) {
			for (Ball& b : balls) {
				b.sim_time_remaining = f_sim_time;
			}

			for (int s = 0; s < n_sim_steps; s++) {

				// update ball pos
				for (Ball& b : balls) {
					if (b.sim_time_remaining <= 0.0f)
						continue;

					b.old_pos = b.pos;
					b.pos += b.v * b.sim_time_remaining;

					b.a = b.v * -0.5f;
					b.a.y += 100.0f;

					b.v += b.a * b.sim_time_remaining;
					//b.v -= b.v.normalized() * 20.0f * b.sim_time_remaining; // OLD WAY OF DRAG

					if (b.v.dot(b.v) < 0.1f) {
						b.v.x = 0;
						b.v.y = 0;
					}

					//std::cout << b.v.str() << '\n';

					
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
					
				}

				for (int i = 0; i < balls.size(); i++) {
					for (int j = i + 1; j < balls.size(); j++) {
						if (balls[i].is_intersecting(balls[j])) {
							Ball::avoid_overlap(balls[i], balls[j]);
						}
					}
				}

				for (Ball& b : balls) {
					for (Capsule& c : capsules) {
						c.collide(b);
					}
				}

				for (Ball& b : balls) {
					b.adjust_sim_time();
				}

				for (int i = 0; i < balls.size(); i++) {
					for (int j = i + 1; j < balls.size(); j++) {
						if (balls[i].is_intersecting(balls[j])) {
							Ball::collide(balls[i], balls[j]);
						}
					}
				}
			}
		}

		for (Ball& b : balls) {
			b.draw(this);
		}

		for (Capsule& c : capsules) {
			c.draw(this);
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

void Capsule::draw(Window* canvas) {
	canvas->FillCircle(olc::vi2d(start.x, start.y), r, olc::DARK_GREY);
	canvas->FillCircle(olc::vi2d(end.x, end.y), r, olc::DARK_GREY);

	auto normal = end - start;
	std::swap(normal.x, normal.y);
	normal.x *= -1;
	normal.normalize();
	normal *= r;

	auto line1a = start + normal;
	auto line1b = end + normal;
	auto line2a = start - normal;
	auto line2b = end - normal;

	canvas->DrawLine(olc::vi2d(line1a.x, line1a.y), olc::vi2d(line1b.x, line1b.y));
	canvas->DrawLine(olc::vi2d(line2a.x, line2a.y), olc::vi2d(line2b.x, line2b.y));
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