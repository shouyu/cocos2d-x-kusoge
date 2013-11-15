#pragma once

#include "cocos2d.h"
#include "Config.h"

class EnemySprite : public cocos2d::CCSprite {
private:
	EnemySprite();
	int m_life;

protected:

public:
	CC_SYNTHESIZE_READONLY(kEnemy, m_enemyType, EnemyType);
	CC_SYNTHESIZE_READONLY(bool, m_active, Active);
	CC_SYNTHESIZE_READONLY(int, m_scoreValue, ScoreValue);

	virtual ~EnemySprite();
	virtual bool initWithEnemyType(kEnemy);
	static const char* getEnemyImageFileName(kEnemy enemyType);
	static EnemySprite* createWithEnemyType(kEnemy);
	bool checkCollide(cocos2d::CCPoint point);
	bool hurt();
	void update();
	void destroy();

	bool m_killed;
};