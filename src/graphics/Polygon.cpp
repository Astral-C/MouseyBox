#include <graphics/Camera.hpp>
#include <graphics/Polygon.hpp>

namespace mb::Graphics {

Polygon::Polygon(){
    mType = RenderableType::Polygon;
}

void Polygon::Draw(SDL_Renderer* r, Camera* cam) {
    float scalex, scaley;
    uint8_t tr, tg, tb, ta;
    SDL_GetRenderDrawColor(r, &tr, &tg, &tb, &ta);
    SDL_SetRenderDrawColor(r, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b, mOverlayColor.a);
    SDL_RenderGetScale(r, &scalex, &scaley);
    SDL_RenderSetScale(r, mScale, mScale);
    for(int p = 1; p < mPoints.size(); p++){
        SDL_RenderDrawLine(r, (mPoints[p-1].x + mDrawRect.x) - cam->mRect.x, (mPoints[p-1].y + mDrawRect.y) - cam->mRect.y, (mPoints[p].x + mDrawRect.x) - cam->mRect.x, (mPoints[p].y + mDrawRect.y) - cam->mRect.y);
    }
    SDL_RenderDrawLine(r, (mPoints.front().x + mDrawRect.x) - cam->mRect.x, (mPoints.front().y + mDrawRect.y) - cam->mRect.y, (mPoints.back().x + mDrawRect.x) - cam->mRect.x, (mPoints.back().y + mDrawRect.y) - cam->mRect.y);
    SDL_SetRenderDrawColor(r, tr, tg, tb, ta);
    SDL_RenderSetScale(r, scalex, scaley);
}

bool Polygon::IsColliding(Polygon& other){
    Polygon* a = this;
    Polygon* b = &other;

    for (size_t ply = 0; ply < 2; ply++){
        if(ply == 1){
            Polygon* t = a;
            a = b;
            b = t;
        }
        for (size_t p = 0; p < a->mPoints.size(); p++){
            Math::Vec2<float> p1 = a->mPoints[p];
            Math::Vec2<float> p2 = a->mPoints[(p + 1) % a->mPoints.size()];
            Math::Vec2<float> axis = {-(p2.y - p1.y), (p2.x - p1.x)};

            float min_ply1 = INFINITY;
            float max_ply1 = -INFINITY;
            for (size_t i = 0; i < a->mPoints.size(); i++){
                Math::Vec2<float> sp = a->mPoints[i];
                float d = sp.dot(axis);
                if(d < min_ply1){
                    min_ply1 = d;
                }
                if(d > max_ply1){
                    max_ply1 = d;
                }
            }
            
            float min_ply2 = INFINITY;
            float max_ply2 = -INFINITY;
            for (size_t i = 0; i < b->mPoints.size(); i++){
                Math::Vec2<float> sp = b->mPoints[i];
                float d = sp.dot(axis);
                if(d < min_ply2){
                    min_ply2 = d;
                }
                if(d > max_ply2){
                    max_ply2 = d;
                }
            }

            if(!(max_ply2 >= min_ply1 && max_ply1 >= min_ply2)){
                return false;
            }
            
        } 
    }

    return true;
}

Polygon::~Polygon(){}

}