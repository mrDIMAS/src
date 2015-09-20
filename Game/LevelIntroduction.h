#pragma once

#include "Level.h"
#include "GUI.h"

class LevelIntroduction : public Level {
private:
    float mTextAlpha;
    float mTextAlphaTo;
    bool mShowIntro;
    ruTextHandle mGUIText;
    ruTextHandle mGUISkipText;
	ruRectHandle mGUIBackground;
public:
    explicit LevelIntroduction();
    virtual ~LevelIntroduction();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};