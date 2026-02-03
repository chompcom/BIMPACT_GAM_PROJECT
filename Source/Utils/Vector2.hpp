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

// Josiah's implementation of the Vector class to make it look more cool and easier to use.
class Vector2 {
public:
    float x, y;

    Vector2() {
        this->x = 0.0f;
        this->y = 0.0f;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2(const Vector2 &vec){
        this->x = vec.x;
        this->y = vec.y;
    }

    Vector2(int x, int y){
        this->x = static_cast<float>(x);
        this->y = static_cast<float>(y);
    }

    Vector2(int x, float y) {
        this->y = y;
        this->x = static_cast<float>(x);
    }

    Vector2(float x, int y){
        this->y = static_cast<float>(y);
        this->x = x;

    }

    ~Vector2() {};

    

    const Vector2 operator+(const Vector2 &rhs) const{
        return Vector2(this->x + rhs.x, this->y + rhs.y);
    }

    const Vector2& operator+=(const Vector2& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }
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

    const Vector2& operator*=(float magnitude) {
        this->x *= magnitude;
        this->y *= magnitude;
        return *this;
    }

    bool operator==(const Vector2& rhs) const {
        return rhs.x == this->x && rhs.y == this->y;
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
        *this = *this * magnitude;
        return *this;
    }

    const Vector2& operator*=(double magnitude) {
        *this = *this * magnitude;
        return *this;
    }

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
        return sqrtf(this->x * this->x + this->y * this->y);
    }

    const Vector2 Normalized() const{
        if (*this == Vector2(0,0)) return Vector2(0,0);
        return Vector2(this->x / this->Length(), this->y / this->Length());
    }
    
};
