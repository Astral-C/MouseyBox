#ifndef __MB_MATH_H__
#define __MB_MATH_H__

#include <utility>
#include <cmath>

namespace mb::Math {

    constexpr double PI = 3.14159265358979323846;

    template <typename T>
    struct Line2D;
    template <typename T>
    struct Circle2D;

    template <typename T>
    struct Vec2 {
        T x;
        T y;

        T dot(Vec2<T>& o){
            return (x * o.x) + (y * o.y);
        }

        Vec2<T> operator+(Vec2<T>& o){
            Vec2<T> v;
            v.x = x + o.x;
            v.y = y + o.y;
            return v;
        }

        Vec2<T> operator-(Vec2<T>& o){
            Vec2<T> v;
            v.x = x - o.x;
            v.y = y - o.y;
            return v;
        }

        Vec2<T> operator-(Vec2<T> o){
            Vec2<T> v;
            v.x = x - o.x;
            v.y = y - o.y;
            return v;
        }

        Vec2<T>& operator*(T s){
            x *= s;
            y *= s;
            return *this;
        }

        Vec2<T>& operator=(Vec2<T> o){
            x = o.x;
            y = o.y;
            return *this;
        }

        void Normalize(){
            double l = (x*x)+(y*y);
            if(l != 0.0f){
                x = x / l;
                y = y / l;
            }
        }

        static float Dist(Vec2<T> a, Vec2<T> b){
            return std::sqrt(std::pow<T>(b.x - a.x, 2) + std::pow<T>(b.y - a.y, 2));
        }
    };

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    class Matrix2x2 {
        T rows[2][2];
    public:
        Matrix2x2() { rows[0][0] = 0; rows[0][1] = 0; rows[1][0] = 0; rows[1][1] = 0; }
        Matrix2x2(T a, T b, T c, T d) { rows[0][0] = a; rows[0][1] = b; rows[1][0] = c; rows[1][1] = d; }

        T* operator[](int r) { return rows[r]; }

        Vec2<T> operator*(Vec2<T> v){
            return {(rows[0][0] * v.x) + (rows[0][1] * v.y), (rows[1][0] * v.x) + (rows[1][1] * v.y)};
        }

        Matrix2x2<T> Inverse(){
            Matrix2x2<T> inverted;

            float determinant = 1.0f/((rows[0][0]*rows[1][1])-(rows[0][1]*rows[1][0]));
            inverted[0][0] = rows[1][1] * determinant;
            inverted[0][1] = -rows[0][1] * determinant;
            inverted[1][0] = -rows[1][0] * determinant;
            inverted[1][1] = rows[0][0] * determinant;
            return inverted;
        }
    };

    template <typename T>
    struct Vec3 {
        T x;
        T y;
        T z;

        T dot(Vec3<T>& o){
            return (x * o.x) + (y * o.y) + (z * o.z);
        }

        T length(){
            return std::sqrt(std::pow(x,2) + std::pow(y,2) + std::pow(z,2));
        }

        Vec3<T> operator/=(double o){
            Vec3<T> v;
            v.x = x / o;
            v.y = y / o;
            v.z = z / o;
            return v;
        }

        Vec3<T> operator+(Vec3<T>& o){
            Vec3<T> v;
            v.x = x + o.x;
            v.y = y + o.y;
            v.z = z + o.z;
            return v;
        }

        Vec3<T> operator-(Vec3<T>& o){
            Vec3<T> v;
            v.x = x - o.x;
            v.y = y - o.y;
            v.z = z - o.z;
            return v;
        }

        Vec3<T>& operator=(Vec3<T> o){
            x = o.x;
            y = o.y;
            z = o.z;
            return *this;
        }
    };

    template <typename T>
    struct Vec4 {
        T x;
        T y;
        T z;
        T w;
    };

    template <typename T>
    struct Line2D {
        std::pair<Vec2<T>, Vec2<T>> mPoints;

        Line2D(){}

        Line2D(T a, T b, T c, T d){
            mPoints = { Vec2<T>(a, b), Vec2<T>(c, d) };
        }

        Line2D(Vec2<T> p1, Vec2<T> p2){
            mPoints.first = p1;
            mPoints.second = p2;
        }

        bool IsColliding(Vec2<T>& p){
            T d1 = Vec2<T>::Dist(p, mPoints.first);
            T d2 = Vec2<T>::Dist(p, mPoints.second);

            T len = Vec2<T>::Dist(mPoints.first, mPoints.second);
            float buffer = 0.1f;

            if(d1 + d2 >= len - buffer && d1 + d2 <= len + buffer){
                return true;
            }

            return false;
        }

        bool IsColliding(Circle2D<T> c){
            if(c.IsColliding(mPoints.first) || c.IsColliding(mPoints.second)) return true;

            float len = Vec2<float>::Dist(mPoints.first, mPoints.second);

            float dot = (((c.mCenter.x - mPoints.first.x) * (mPoints.second.x - mPoints.first.x)) + ((c.mCenter.y - mPoints.first.y) * (mPoints.second.y - mPoints.first.y))) / std::pow(len, 2);

            float closestX = mPoints.first.x + (dot * (mPoints.second.x - mPoints.first.x));
            float closestY = mPoints.first.y + (dot * (mPoints.second.y - mPoints.first.y));

            bool onSeg = IsColliding(Vec2<float>(closestX, closestY));
            if(!onSeg) return false;

            float dist = Vec2<float>::Dist(Vec2<float>(closestX, closestY), c.mCenter);

            if(dist <= c.mRadius) return true;

            return false;
        }
    };

    template <typename T>
    struct Circle2D {
        T mRadius;
        Vec2<T> mCenter;

        Circle2D(){}

        Circle2D(T a, T b, T c){
            mCenter = { a,  b };
            mRadius = c;
        }

        Circle2D(Vec2<T> center, T radius){
            mCenter = center;
            mRadius = radius;
        }

        bool IsColliding(Vec2<T>& p){
            return Vec2<T>::Dist(p, mCenter) < mRadius;
        }

        bool IsColliding(Line2D<T> l){
            if(IsColliding(l.mPoints.first) || IsColliding(l.mPoints.second)) return true;

            float len = Vec2<float>::Dist(l.mPoints.first, l.mPoints.second);

            float dot = (((mCenter.x - l.mPoints.first.x) * (l.mPoints.second.x - l.mPoints.first.x)) + ((mCenter.y - l.mPoints.first.y) * (l.mPoints.second.y - l.mPoints.first.y))) / std::pow(len, 2);

            float closestX = l.mPoints.first.x + (dot * (l.mPoints.second.x - l.mPoints.first.x));
            float closestY = l.mPoints.first.y + (dot * (l.mPoints.second.y - l.mPoints.first.y));

            Vec2<float> closest(closestX, closestY);
            bool onSeg = l.IsColliding(closest);
            if(!onSeg) return false;

            float dist = Vec2<float>::Dist(Vec2<float>(closestX, closestY), mCenter);

            if(dist <= mRadius) return true;

            return false;
        }
    };

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    T Lerp(T a, T b, double t){
        return (1 - t) * a + t * b;
    }

}

#endif
