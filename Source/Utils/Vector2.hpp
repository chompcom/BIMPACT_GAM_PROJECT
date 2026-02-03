/*!
\file   Vector2.hpp
\author Josiah Hong
\par    email: hong.j\@digipen.edu
\par    DigiPen login: hong.j
\par    Section A
\brief  
    Vector class that has operator overloading for cleaner
    and clearer code writing.
*/
#pragma once
#include <cmath>
#include <stdexcept>

// Josiah's implementation of the Vector class to make it look more cool and easier to use.
class Vector2 {
public:
    float x, y;

    Vector2() : x{ 0.0f }, y{ 0.0f } {}

    Vector2(float x, float y) : x{ x }, y{ y } {}

    Vector2(const Vector2& vec) : x{ vec.x }, y{ vec.y } {}

    Vector2(int x, int y) : x{ static_cast<float>(x) }, y{ static_cast<float>(y) } {}

    Vector2(int x, float y) : x{ static_cast<float>(x) }, y{ y } {}

    Vector2(float x, int y) : x{ (x) }, y{ static_cast<float>(y) } {}
    // ADDITION

    const Vector2 operator+(const Vector2 &rhs) const{
        return Vector2(this->x + rhs.x, this->y + rhs.y);
    }

    const Vector2& operator+=(const Vector2& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }


    // SUBTRACTION

    const Vector2 operator-() const {
        return Vector2(-this->x, -this->y);
    }

    const Vector2 operator-(const Vector2 &rhs) const{
        return Vector2(this->x - rhs.x, this->y - rhs.y);
    }

    const Vector2& operator-=(const Vector2& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    // COMPARISON

    bool operator==(const Vector2& rhs) const {
        return rhs.x == this->x && rhs.y == this->y;
    }

    bool operator!=(const Vector2& rhs) const {
        return !(*this == rhs);
    }

    // MULTIPLY BY MAGNITUDE

    const Vector2& operator*=(float magnitude) {
        this->x *= magnitude;
        this->y *= magnitude;
        return *this;
    }

    const Vector2 operator*(float magnitude) const {
        return Vector2(magnitude * this->x, magnitude * this->y);
    }

    const Vector2 operator*(double magnitude) const {
        return Vector2(static_cast<float>(magnitude * this->x), static_cast<float>(magnitude * this->y));
    }

    const Vector2 operator*(int magnitude) const {
        return Vector2(static_cast<float>(magnitude * this->x), static_cast<float>(magnitude * this->y));
    }

    const Vector2& operator*=(int magnitude) {
        *this = *this * static_cast<float>(magnitude);
        return *this;
    }

    const Vector2& operator*=(double magnitude) {
        *this = *this * static_cast<float>(magnitude);
        return *this;
    }

    // DIVIDE BY MAGNITUDE

    const Vector2 operator/(float magnitude) const {
        if (magnitude == 0.0f) 
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / magnitude, this->y / magnitude);
    }

    const Vector2 operator/(int magnitude) const {
        if (magnitude == 0)
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / static_cast<float>(magnitude), this->y / static_cast<float>(magnitude));
    }

    const Vector2 operator/(double magnitude) const {
        if (magnitude == 0.0)
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / static_cast<float>(magnitude), this->y / static_cast<float>(magnitude));
    }

    const Vector2& operator/=(float magnitude) {
        *this = *this / magnitude;
        return *this;
    }

    const Vector2& operator/=(int magnitude) {
        *this = *this / static_cast<float>(magnitude);
        return *this;
    }

    const Vector2& operator/=(double magnitude) {
        *this = *this / static_cast<float>(magnitude);
        return *this;
    }

    // VECTOR FUNCTIONS

    float Dot(const Vector2 &vector) const{
        return (this->x * vector.x)+(this->y*vector.y);
    }

    float DistanceSq(const Vector2 &destination) const {
        return (destination.x - this->x)*(destination.x - this->x) + (destination.y - this->y) * (destination.y - this->y);
    }

    float Distance(const Vector2 &destination) const{
        return sqrtf(this->DistanceSq(destination));
    }

    float Length() const {
        return sqrtf((this->x * this->x) + (this->y * this->y));
    }

    float LengthSq() const {
        return this->x * this->x + this->y * this->y;
    }

    const Vector2 Normalized() const{
        if (*this == Vector2(0,0)) return Vector2(0,0);
        return Vector2(this->x / this->Length(), this->y / this->Length());
    }

    // Returns the value in between two vectors
    const Vector2& Clamp(const Vector2& min, const Vector2& max) const {
        float myx = min.x < this->x ? min.x : (this->x > max.x ? max.x : this->x);
        float myy = min.y < this->y ? min.y : (this->y > max.y ? max.y : this->y);
        return Vector2(myx, myy);
    }

    //Returns the vector with its length never exceeding the length provided
    const Vector2& ClampLength(float length) {
        return this->LengthSq() > length * length ? this->Normalized() * length : *this;
    }
    //Returns the vector with its length never exceeding the length provided
    const Vector2& ClampLength(double length) {
        return this->ClampLength(static_cast<float>(length));
    }
    //Returns the vector with its length never exceeding the length provided
    const Vector2& ClampLength(int length) {
        return this->ClampLength(static_cast<float>(length));
    }
};