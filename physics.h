#pragma once
#include "vec.h"
#include "olcPixelGameEngine.h"
#include <utility>


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

		b1.pos += vec_mv * (-mv);
		b2.pos += vec_mv * mv;
		//std::cout << vec_mv.str() << '\n';
	}

	static bool is_inside(vec2d<float> circle, float r, int mx, int my) {
		float d = (circle - vec2d<float>(mx, my)).mag();
		return d <= r;
	}

};

class StaticBall {
public:
	float r;
	vec2d<float> pos;

	void collide(Ball& ball) {
		// static
		auto v = pos - ball.pos;
		float d = v.mag();
		if (ball.r + r < d)
			return;

		float mv = ball.r + r - d;
		mv *= 0.5;
		v.normalize();
		ball.pos -= v * mv;


		// dynamic
		float horizontal = -2 * ball.v.dot(v);
		ball.v += v * horizontal;
	}
};

class Capsule {
public:
	float r;
	vec2d<float> start;
	vec2d<float> end;

	void collide(Ball& ball) {
		auto u = end - start;
		auto cap_to_ball = ball.pos - start;
		auto u_normalized = u.normalized();
		float t = std::min(1.0f, std::max(0.0f, cap_to_ball.dot(u_normalized) / u.mag()));
		auto mid = u * t + start;

		StaticBall sball;
		sball.r = r;
		sball.pos = mid;
		sball.collide(ball);
	}
};

class PhysicsEngine {
public:
	std::vector<Ball> balls;
	std::vector<StaticBall> static_balls;
	std::vector<Capsule> capsules;
	float width, height;
	
	bool gravity = false;

	const int n_epochs = 4;
	const int n_sim_steps = 15;


	void update(float fElapsedTime) {
		const float f_sim_time = fElapsedTime / (float)n_epochs;

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
					if (gravity)
						b.a.y += 100.0f;

					b.v += b.a * b.sim_time_remaining;
					//b.v -= b.v.normalized() * 20.0f * b.sim_time_remaining; // OLD WAY OF DRAG

					if (b.v.dot(b.v) < 0.1f) {
						b.v.x = 0;
						b.v.y = 0;
					}

					if (b.pos.x > width + b.r) {
						b.pos.x = -b.r;
					}
					else if (b.pos.x < -b.r) {
						b.pos.x = width + b.r;
					}

					if (b.pos.y > height + b.r) {
						b.pos.y = -b.r;
					}
					else if (b.pos.y < -b.r) {
						b.pos.y = height + b.r;
					}

				}

				// static collisions
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

					for (StaticBall& sball : static_balls) {
						sball.collide(b);
					}
				}

				for (Ball& b : balls) {
					b.adjust_sim_time();
				}

				// dynamic collisions
				for (int i = 0; i < balls.size(); i++) {
					for (int j = i + 1; j < balls.size(); j++) {
						if (balls[i].is_intersecting(balls[j])) {
							Ball::collide(balls[i], balls[j]);
						}
					}
				}
			}
		}
	}
};

class OlcPhysicsWindow : public olc::PixelGameEngine {
protected:
	PhysicsEngine engine;

	bool OnUserCreate() override {
		engine.width = ScreenWidth();
		engine.height = ScreenHeight();
		return true;
	}

	void draw(Capsule& cap) {
		FillCircle(olc::vi2d(cap.start.x, cap.start.y), cap.r, olc::DARK_GREY);
		FillCircle(olc::vi2d(cap.end.x, cap.end.y), cap.r, olc::DARK_GREY);

		auto normal = cap.end - cap.start;
		std::swap(normal.x, normal.y);
		normal.x *= -1;
		normal.normalize();
		normal *= cap.r;

		auto line1a = cap.start + normal;
		auto line1b = cap.end + normal;
		auto line2a = cap.start - normal;
		auto line2b = cap.end - normal;

		DrawLine(olc::vi2d(line1a.x, line1a.y), olc::vi2d(line1b.x, line1b.y));
		DrawLine(olc::vi2d(line2a.x, line2a.y), olc::vi2d(line2b.x, line2b.y));
	}

	void draw(Ball& ball) {
		DrawCircle(olc::vi2d(ball.pos.x, ball.pos.y), ball.r);
	}

	void draw(StaticBall& ball) {
		DrawCircle(olc::vi2d(ball.pos.x, ball.pos.y), ball.r);
	}
};