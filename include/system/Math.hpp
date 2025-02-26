#ifndef __MB_MATH_H__
#define __MB_MATH_H__

#include <utility>
#include <cmath>

namespace mb::Math {

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

        Vec2<T>& operator=(Vec2<T>& o){ 
            x = o.x;
            y = o.y;
            return *this;
        }

        static float Dist(Vec2<T> a, Vec2<T> b){
            return std::sqrt(std::pow<T>(b.x - a.x, 2) + std::pow<T>(b.y - a.y, 2));
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

        Vec3<T> operator-(Vec3<T>& o){
            Vec3<T> v;
            v.x = x - o.x;
            v.y = y - o.y;
            v.z = z - o.z;
            return v;
        }

        Vec3<T>& operator=(Vec3<T>& o){ 
            x = o.x;
            y = o.y;
            z = o.z;
            return &this;
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
}

#endif