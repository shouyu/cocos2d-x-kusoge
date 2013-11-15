#pragma once

#include "cocos2d.h"
#include "EnemySprite.h"
#include "Config.h"

class MainScene : public cocos2d::CCLayer
{
private:
	int m_life;
	float m_respawnTime;
	bool m_playingWarnTimer;
	unsigned int m_warnTimerId;
	std::vector<cocos2d::CCSprite*> m_chain_sprite;

	void removeInactiveEnemy();
	void showBackground();
	void showLabel();
	void showProgress(float);
	void checkGameover();
	void showReset();
	cocos2d::CCLabelTTF* showPopupLabel(const char*, cocos2d::CCPoint);
	const char* getSEFileName(int chain);

protected:
	enum kTag {
		kTagBackground,
		kTagMainScene,
		kTagScore,
		kTagChain,
		kTagProgress,
		kTagTime
	};
	void initForVariables();
	std::list<EnemySprite*> enemies;
	void spawnEnemy();
	void removeEnemy(cocos2d::CCNode*);
	void releaseAfterAction(cocos2d::CCNode*);
	void menuResetCallback(cocos2d::CCObject*);

public:
	virtual bool init();
	virtual void update(float delta);
	static cocos2d::CCScene* scene();
	CREATE_FUNC(MainScene);

	virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
};