#include "TimerSprite.h"

using namespace cocos2d;

TimerSprite::TimerSprite() : m_time(30.0f) { }
TimerSprite::~TimerSprite() { }

TimerSprite* TimerSprite::create() {
	TimerSprite* pRet = new TimerSprite();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	} else {
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool TimerSprite::init() {
	CCSprite* sp = CCSprite::create("ProgressBar.png");
	if (!this->initWithSprite(sp)) {
		return false;
	}
	
	this->setType(kCCProgressTimerTypeBar);
	this->setBarChangeRate(ccp(0, 1.0f));
	this->setMidpoint(ccp(0.5f, 0));

	return true;
}

void TimerSprite::update(float dt) {
	m_time -= dt;
	if (m_time < 0.0f) m_time = 0.0f;
	float percent = m_time / 30.0f * 100.0f;
	this->setPercentage(percent);
}

void TimerSprite::addTime(float dt) {
	m_time += dt;
	if (m_time > 30.0f) m_time = 30.0f;
	if (m_time < 0.0f) m_time = 0.0f;
}