#include <graphics/Camera.hpp>
#include <graphics/Circle.hpp>
#include <system/Log.hpp>

namespace mb::Graphics {

Circle::Circle(){
    mType = RenderableType::Rect;
}

void Circle::Draw(SDL_Renderer* r, Camera* cam) {
    float scalex, scaley;
    uint8_t tr, tg, tb, ta;
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_GetRenderDrawColor(r, &tr, &tg, &tb, &ta);
    SDL_SetRenderDrawColor(r, mOverlayColor.r, mOverlayColor.g, mOverlayColor.b, mOverlayColor.a);
    SDL_RenderGetScale(r, &scalex, &scaley);
    SDL_RenderSetScale(r, 1, 1); 

    //if(!mStatic){
    //    curX -= cam->mRect.x;
    //    curY -= cam->mRect.y;
    //}

    for(int curX = -mRadius; curX < mRadius; curX++){
        for(int curY = -mRadius; curY < mRadius; curY++){
            //if(mDrawFilled){
                if(mb::Math::Vec2<int>::Dist(mb::Math::Vec2<int>(curX, curY), mb::Math::Vec2<int>(0,0)) < mRadius){
                    SDL_RenderDrawPoint(r, curX + mDrawRect.x, curY + mDrawRect.y);
                }
            //} else {
            //    if((abs(mDrawRect.x - curX) < mRadius  && abs(mDrawRect.x - curX) > mRadius - mThickness) && (abs(mDrawRect.y - curY) < mRadius  && abs(mDrawRect.y - curY) > mRadius - mThickness)){
            //        SDL_RenderDrawPoint(r, curX, curY);
            //    }
            //}
        }   
    }


    SDL_SetRenderDrawColor(r, tr, tg, tb, ta);
    SDL_RenderSetScale(r, scalex, scaley);
}

Circle::~Circle(){}

}