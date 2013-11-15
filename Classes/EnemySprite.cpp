#include "EnemySprite.h"
#include <iostream>

using namespace cocos2d;

EnemySprite::EnemySprite() {
}

EnemySprite::~EnemySprite() {
}

EnemySprite* EnemySprite::createWithEnemyType(kEnemy enemyType) {
	EnemySprite* pRet = new EnemySprite();
	if (pRet && pRet->initWithEnemyType(enemyType)) {
		pRet->autorelease();
		return pRet;
	}
	else {
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool EnemySprite::initWithEnemyType(kEnemy enemyType) {
	if (!CCSprite::initWithFile(getEnemyImageFileName(enemyType))) {
		return false;
	}

	m_enemyType = enemyType;
	m_active = true;
	m_life = 1;
	m_killed = false;
	this->setScale(0.8);

	return true;
}

const char* EnemySprite::getEnemyImageFileName(kEnemy enemyType) {
	switch (enemyType) {
	case kEnemyBlue:
		//return "Icon-72-blue.png";
		return "ika.png";

	case kEnemyRed:
		//return "Icon-72-red.png";
		return "maguro.png";

	default:
		CCAssert(false, "Invalid EnemyType");
		return "";
	}
}

bool EnemySprite::checkCollide(CCPoint point) {
	if (!m_active) return false;
	return this->boundingBox().containsPoint(point);
}

bool EnemySprite::hurt() {
	if (m_killed) return false;
	m_life--;
	m_killed = m_life <= 0;
	return m_killed;
}

void EnemySprite::update() {
	CCPoint p = this->getPosition();
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();

	if (p.x < 0 || p.x > screenSize.width ||
		p.y < -1 * this->getContentSize().height / 2 || p.y > screenSize.height) {
			m_active = false;
	}
}

void EnemySprite::destroy() {
}