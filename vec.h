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
};