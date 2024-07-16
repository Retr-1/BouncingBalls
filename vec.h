#pragma once
template <typename T>
struct vec2d {
	T x;
	T y;

	vec2d(T x = 0, T y = 0) {
		this->x = x;
		this->y = y;
	}

	template <typename U>
	vec2d(vec2d<U>& other) {
		x = other.x;
		y = other.y;
	}

	template <typename U>
	vec2d operator+(const vec2d<U>& obj) {
		return vec2d<T>(x + obj.x, y + obj.y);
	}

	template <typename U>
	void operator+=(const vec2d<U>& obj) {
		x += obj.x;
		y += obj.y;
	}

	template <typename U>
	vec2d operator-(const vec2d<U>& obj) {
		return vec2d<T>(x - obj.x, y - obj.y);
	}

	template <typename U>
	vec2d<U> operator/(const U& div) {
		return vec2d<U>(x / div, y / div);
	}

	template <typename U>
	T operator*(const vec2d<U>& obj) {
		return dot(obj);
	}

	template <typename U>
	vec2d<T> operator*(const U& mult) {
		return vec2d<T>(x * mult, y * mult);
	}
	
	template <typename U>
	void operator*=(const U& mult) {
		x *= mult;
		y *= mult;
	}

	template <typename U>
	void operator/=(const U& mult) {
		x /= mult;
		y /= mult;
	}

	template <typename U>
	void operator-=(const vec2d<U>& other) {
		x -= other.x;
		y -= other.y;
	}


	//T operator*(const vec2d<float>& obj) {
	//	return dot(obj);
	//}

	//T operator*(const vec2d<int>& obj) {
	//	return dot(obj);
	//}

	std::string str() {
		return std::to_string(x) + " " + std::to_string(y) + "\n";
	}

	float mag() {
		return sqrt(x * x + y * y);
	}

	template <typename U>
	T dot(const vec2d<U>& other) {
		return x * other.x + y * other.y;
	}

	void normalize() {
		float m = mag();
		x /= m;
		y /= m;
	}

	vec2d<float> normalized() {
		float m = mag();
		return vec2d<float>(x / m, y / m);
	}
};