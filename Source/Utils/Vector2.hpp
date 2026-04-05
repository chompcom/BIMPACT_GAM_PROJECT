/* Start Header ************************************************************************/
/*!
\file		Vector2.hpp
\author 	Hong Josiah Qin, hong.j, 2501239
\par  		hong.j@digipen.edu
\brief		Vector class that has operator overloading for cleaner
    and clearer code writing.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <stdexcept>

// Josiah's implementation of the Vector class to make it look more cool and easier to use.
class Vector2 {
public:
    float x, y;

    //! \brief Default constructor of Vector2. Sets x and y to zero.
    Vector2() : x{ 0.0f }, y{ 0.0f } {}

    //! \brief Constructor with parameters x and y to slot in to their respective data members
    Vector2(float x, float y) : x{ x }, y{ y } {}

    //! \brief Copy constructor
    Vector2(const Vector2& vec) : x{ vec.x }, y{ vec.y } {}

    //! \brief Allow for int input for lazy typing
    Vector2(int x, int y) : x{ static_cast<float>(x) }, y{ static_cast<float>(y) } {}

    //! \brief Allow for int input for lazy typing
    Vector2(int x, float y) : x{ static_cast<float>(x) }, y{ y } {}

    //! \brief Allow for int input for lazy typing
    Vector2(float x, int y) : x{ (x) }, y{ static_cast<float>(y) } {}
    // ADDITION

    //! \brief Adds two vectors together
    const Vector2 operator+(const Vector2 &rhs) const{
        return Vector2(this->x + rhs.x, this->y + rhs.y);
    }

    //! \brief Adds the vector with another vector
    Vector2& operator+=(const Vector2& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }


    // SUBTRACTION

    //! \brief Negates the vector
    const Vector2 operator-() const {
        return Vector2(-this->x, -this->y);
    }

    //! \brief Subtracts two vectors
    const Vector2 operator-(const Vector2 &rhs) const{
        return Vector2(this->x - rhs.x, this->y - rhs.y);
    }

    //! \brief Subtracts the vector by another
    Vector2& operator-=(const Vector2& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    // COMPARISON

    //! \brief Checks if two vectors have the same values
    bool operator==(const Vector2& rhs) const {
        return rhs.x == this->x && rhs.y == this->y;
    }

    //! \brief Checks if two vectors don't have the same values
    bool operator!=(const Vector2& rhs) const {
        return !(*this == rhs);
    }

    // MULTIPLY BY MAGNITUDE

    //! \brief Scales the vector by a magnitude
    Vector2& operator*=(float magnitude) {
        this->x *= magnitude;
        this->y *= magnitude;
        return *this;
    }

    //! \brief Scales the vector by a magnitude
    const Vector2 operator*(float magnitude) const {
        return Vector2(magnitude * this->x, magnitude * this->y);
    }

    //! \brief Scales the vector by a magnitude with lazy typing
    const Vector2 operator*(double magnitude) const {
        return Vector2(static_cast<float>(magnitude * this->x), static_cast<float>(magnitude * this->y));
    }

    //! \brief Scales the vector by a magnitude with lazy typing
    const Vector2 operator*(int magnitude) const {
        return Vector2(static_cast<float>(magnitude * this->x), static_cast<float>(magnitude * this->y));
    }

    //! \brief Scales the vector by a magnitude with lazy typing
    Vector2& operator*=(int magnitude) {
        *this = *this * static_cast<float>(magnitude);
        return *this;
    }

    //! \brief Scales the vector by a magnitude with lazy typing
    Vector2& operator*=(double magnitude) {
        *this = *this * static_cast<float>(magnitude);
        return *this;
    }

    // DIVIDE BY MAGNITUDE

    //! \brief Scales down the vector by a magnitude
    //! \brief Throws overflow_error when dividing by zero
    const Vector2 operator/(float magnitude) const {
        if (magnitude == 0.0f) 
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / magnitude, this->y / magnitude);
    }
    //! \brief Scales down the vector by a magnitude with lazy typing
    //! \brief Throws overflow_error when dividing by zero
    const Vector2 operator/(int magnitude) const {
        if (magnitude == 0)
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / static_cast<float>(magnitude), this->y / static_cast<float>(magnitude));
    }
    //! \brief Scales down the vector by a magnitude with lazy typing
    //! \brief Throws overflow_error when dividing by zero
    const Vector2 operator/(double magnitude) const {
        if (magnitude == 0.0)
            throw std::overflow_error("Divide by zero exception");
        return Vector2(this->x / static_cast<float>(magnitude), this->y / static_cast<float>(magnitude));
    }
    //! \brief Scales down the vector by a magnitude
    //! \brief Throws overflow_error when dividing by zero
    Vector2& operator/=(float magnitude) {
        *this = *this / magnitude;
        return *this;
    }
    //! \brief Scales down the vector by a magnitude with lazy typing
    //! \brief Throws overflow_error when dividing by zero
    Vector2& operator/=(int magnitude) {
        *this = *this / static_cast<float>(magnitude);
        return *this;
    }
    //! \brief Scales down the vector by a magnitude with lazy typing
    //! \brief Throws overflow_error when dividing by zero
    Vector2& operator/=(double magnitude) {
        *this = *this / static_cast<float>(magnitude);
        return *this;
    }

    // VECTOR FUNCTIONS

    //! \brief Returns the dot product of this vector with the other
    float Dot(const Vector2 &vector) const{
        return (this->x * vector.x)+(this->y*vector.y);
    }

    //! \brief Returns the square distance between this vector and the destination vector
    float DistanceSq(const Vector2 &destination) const {
        return (destination.x - this->x)*(destination.x - this->x) + (destination.y - this->y) * (destination.y - this->y);
    }

    //! \brief Returns the distance between this vector and the destination vector
    float Distance(const Vector2 &destination) const{
        return sqrtf(this->DistanceSq(destination));
    }

    //! \brief Returns the length of the vector. Also known as magnitude.
    float Length() const {
        return sqrtf((this->x * this->x) + (this->y * this->y));
    }

    //! \brief Returns the square length of the vector. Also known as magnitude.
    float LengthSq() const {
        return this->x * this->x + this->y * this->y;
    }

    //! \brief Returns a normalized form of the vector.
    const Vector2 Normalized() const{
        if (*this == Vector2(0,0)) return Vector2(0,0);
        return Vector2(this->x / this->Length(), this->y / this->Length());
    }

    //! \brief Returns the value clamped in between two vectors
    const Vector2& Clamp(const Vector2& min, const Vector2& max) const {
        float myx = min.x < this->x ? min.x : (this->x > max.x ? max.x : this->x);
        float myy = min.y < this->y ? min.y : (this->y > max.y ? max.y : this->y);
        return Vector2(myx, myy);
    }

    //! \brief Returns the absolute value of the vector
    Vector2 Abs() const {
        float newx = x < 0 ? -x : x;
        float newy = y < 0 ? -y : y;

        return Vector2{ newx, newy};
    }


};


#endif //VECTOR_HPP