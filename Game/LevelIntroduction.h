#pragma once

#include "Level.h"
#include "GUIProperties.h"

class LevelIntroduction : public Level {
private:
    float mTextAlpha;
    float mTextAlphaTo;
    bool mShowIntro;
    ruText * mGUIText;
    ruText * mGUISkipText;
	ruRect * mGUIBackground;
public:
    explicit LevelIntroduction();
    virtual ~LevelIntroduction();
    virtual void DoScenario() final;
    virtual void Show() final;
    virtual void Hide() final;
    virtual void OnSerialize( SaveFile & out ) final;
    virtual void OnDeserialize( SaveFile & in ) final;
};