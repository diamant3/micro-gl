#pragma once
#include <cstdint>

namespace microgl {

    template<typename number>
    struct vec2 {
        number x, y;
        vec2() = default;
        vec2(const number & x_, const number & y_) {
            x = x_;
            y = y_;
        }

        template<typename F>
        explicit vec2(const vec2<F> & a) {
            this->x = static_cast<number>(a.x);
            this->y = static_cast<number>(a.y);
        }

        vec2 operator-(const vec2 & a) const {
            return vec2{this->x-a.x, this->y - a.y};
        }

        vec2 operator+(const vec2 & a) const {
            return vec2{this->x + a.x, this->y + a.y};
        }

        vec2 operator*(const vec2 & a) const {
            return {this->x*a.x, this->y*a.y};
        }

        number dot(const vec2 & a) const {
            return (this->x*a.x + this->y*a.y);
        }

        vec2 orthogonalLeft() const {
            return {this->y, -this->x};
        }

        vec2 orthogonalRight() const {
            return {-this->y, this->x};
        }

        vec2 square() const {
            return {this->x*this->x, this->y*this->y};
        }

        number sum() const {
            return this->x+this->y;
        }

        vec2 operator*(const number & val) const {
            return {this->x*val, this->y*val};
        }
        vec2 operator*(const signed & val) const {
            return {this->x*val, this->y*val};
        }

        vec2 operator<<(const int & a) const {
            return vec2<number>{this->x*(1<<a), this->y*(1<<a)};
        }

        vec2 operator-() const {
            return vec2<number>{-this->x, -this->y};
        }

        vec2 operator>>(const int & a) const {
            return vec2<number>{this->x/(1<<a), this->y/(1<<a)};
        }

        vec2 operator/(const number & val) const {
            return vec2<number>{this->x/val, this->y/val};
        }

        vec2 operator/(const vec2 & val) const {
            return vec2<number>{this->x/val.x, this->y/val.y};
        }

        bool operator==(const vec2 & rhs) const {
            return this->x==rhs.x && this->y==rhs.y;
        }
        bool operator!=(const vec2 & rhs) const {
            return !(*this==rhs);
        }

        vec2 & operator=(const vec2 & a) {
            this->x = a.x; this->y = a.y;
            return *this;
        }

    };

}
