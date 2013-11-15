#pragma once

#include "cocos2d.h"

class TimerSprite : public cocos2d::CCProgressTimer {
private:
	TimerSprite();

public:
	virtual ~TimerSprite();
	static TimerSprite* create();
	virtual bool init();
	CC_SYNTHESIZE_READONLY(float, m_time, Time);
	void update(float delta);
	void addTime(float dt);
};