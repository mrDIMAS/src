#pragma once

#include "Level.h"
#include "GUI.h"

class LevelIntroduction : public Level
{
private:
    string mPlainText;
    float mTextAlpha;
    float mTextAlphaTo;
    bool mShowIntro;
    ruTimerHandle mTextTimer;
public:
    explicit LevelIntroduction();
    virtual ~LevelIntroduction();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( TextFileStream & out ) final;
    virtual void OnDeserialize( TextFileStream & in ) final;
};