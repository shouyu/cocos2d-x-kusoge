#include <string>
#include <sstream>
#include <iomanip>
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "EnemySprite.h"
#include "ScoreManager.h"
#include "TimerSprite.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

CCScene* MainScene::scene() {
	CCScene* scene = CCScene::create();
	MainScene* layer = MainScene::create();
	scene->addChild(layer, 0, kTagMainScene);
	return scene;
}

bool MainScene::init() {
	if (!CCLayer::init()) {
		return false;
	}

	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	initForVariables();

	showBackground();
	showLabel();
	showProgress(0.0f);
	spawnEnemy();

	this->scheduleUpdate();

	return true;
}

void MainScene::showBackground() {
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();
	CCSprite* bg = CCSprite::create("background.jpg");
	bg->setAnchorPoint(ccp(0, 0));
	bg->setPosition(ccp(0, 0));
	float scale = screenSize.height / bg->getContentSize().height;
	bg->setScale(scale);
	this->addChild(bg, -1, kTagBackground);
}

void MainScene::showLabel() {
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();
	CCLabelTTF* scoreLabel = static_cast<CCLabelTTF*>(this->getChildByTag(kTagScore));
	if (!scoreLabel) {
		scoreLabel = CCLabelTTF::create("Score: 0", "Verdana", 18,
			CCSizeMake(screenSize.width, 50), kCCTextAlignmentLeft);
		scoreLabel->setAnchorPoint(ccp(0.0, 1.0));
		scoreLabel->setColor(ccc3(255, 255, 255));
		scoreLabel->setPosition(ccp(0, screenSize.height));
		this->addChild(scoreLabel, 100, kTagScore);
	}
	ostringstream os;
	int score = ScoreManager::getInstance().getScore();
	int chain = ScoreManager::getInstance().getChainCount();
	int chain_mini = ScoreManager::getInstance().getChainMiniCount();
	kEnemy chain_color = ScoreManager::getInstance().getChainColor();
	os << "Score: " << score << " Chain: " << chain;
	scoreLabel->setString(os.str().c_str());
	vector<CCSprite*>::iterator iter = m_chain_sprite.begin(); 
	for (int i = 0; i < m_chain_sprite.size(); i++) {
		if (i >= (int)chain_color * 3 && i < chain_mini + 3 * (int)chain_color) {
			if (m_chain_sprite[i]->getOpacity() != 255) {
				m_chain_sprite[i]->setOpacity(255);
				CCAction* act = CCBlink::create(0.3f, 2);
				m_chain_sprite[i]->runAction(act);
			}
		} else {
			m_chain_sprite[i]->setOpacity(0);
		}
	}
}

void MainScene::showProgress(float dt) {
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();
	CCLabelTTF* label = static_cast<CCLabelTTF*>(this->getChildByTag(kTagTime));
	if (!label) {
		label = CCLabelTTF::create("30.0s", "Verdana", 18);
		label->setAnchorPoint(ccp(1.0, 0));
		label->setPosition(ccp(screenSize.width - 5, 5));
		this->addChild(label, 100, kTagTime);
	}
	TimerSprite* timer = static_cast<TimerSprite*>(this->getChildByTag(kTagProgress));
	if (!timer) {
		timer = TimerSprite::create();
		float scale = 
			(screenSize.height - label->boundingBox().getMaxY() - 5 * 2) /
			timer->getContentSize().height;
		timer->setScaleY(scale);
		timer->setPosition(ccp(screenSize.width - 7.5, label->boundingBox().getMaxY() + timer->boundingBox().size.height / 2));
		this->addChild(timer, 100, kTagProgress);
	}
	timer->update(dt);
	ostringstream os;
	os << std::fixed << std::setprecision(1) << timer->getTime() << "s";
	label->setString(os.str().c_str());

	if (timer->getTime() < 5.0f && !m_playingWarnTimer) {
		m_warnTimerId = SimpleAudioEngine::sharedEngine()->playEffect("time.wav", true);
		m_playingWarnTimer = true;
		CCSprite* bg = static_cast<CCSprite*>(this->getChildByTag(kTagBackground));
		bg->runAction(
			CCRepeatForever::create(
			    CCSequence::create(
					CCTintBy::create(.5f, 0, -255, -255),
					CCTintBy::create(.5f, 0, 255, 255),
					NULL)));
	} else if (timer->getTime() >= 5.0f && m_playingWarnTimer) {
		SimpleAudioEngine::sharedEngine()->stopEffect(m_warnTimerId);
		m_playingWarnTimer = false;
		CCSprite* bg = static_cast<CCSprite*>(this->getChildByTag(kTagBackground));
		bg->stopAllActions();
		bg->setColor(ccc3(255, 255, 255));
	}
}

void MainScene::checkGameover() {
	TimerSprite* timer = static_cast<TimerSprite*>(this->getChildByTag(kTagProgress));
	if (timer->getTime() <= 0.0f) {
		// GameOver
		this->unscheduleUpdate();
		this->setTouchEnabled(false);
		CCSprite* bg = static_cast<CCSprite*>(this->getChildByTag(kTagBackground));
		bg->stopAllActions();
		bg->setColor(ccc3(255, 255, 255));
		bg->runAction(CCTintBy::create(.5f, -200, -200, -200));
		SimpleAudioEngine::sharedEngine()->stopAllEffects();
		m_playingWarnTimer = false;

		// Show Score Label
		CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();
		ostringstream os;
		os << "Score: " << ScoreManager::getInstance().getScore() << std::endl
			<< "Max Chain: " << ScoreManager::getInstance().getMaxChain();
		CCLabelTTF* label = CCLabelTTF::create(os.str().c_str(), "Verdana", 24);
		label->setPosition(ccp(screenSize.width / 2, screenSize.height / 2));
		label->setOpacity(0);
		this->addChild(label, 1000);
		label->runAction(CCFadeIn::create(1.5f));

		// Show Reset Label
		CCMenuItemFont* reset = CCMenuItemFont::create("start new sushi", this, menu_selector(MainScene::menuResetCallback));
		reset->setAnchorPoint(ccp(0.5f, 1.0f));
		reset->setPosition(ccp(screenSize.width / 2, label->boundingBox().getMinY() - 10));
		CCMenu* menu = CCMenu::create(reset, NULL);
		menu->setPosition(CCPointZero);
		this->addChild(menu);
	}
}

void MainScene::update(float delta) {
	removeInactiveEnemy();
	m_respawnTime -= delta;
	if (m_respawnTime <= 0.0f) {
		spawnEnemy();
		float r = (float)rand()/(float)RAND_MAX;
		m_respawnTime = .8f + 1.0f * r;
	}
	showLabel();
	showProgress(delta);
	checkGameover();
}

void MainScene::removeInactiveEnemy() {
	TimerSprite* timer = static_cast<TimerSprite*>(this->getChildByTag(kTagProgress));
	std::list<EnemySprite*>::iterator it = enemies.begin();
	while(it != enemies.end()) {
		(*it)->update();
		if (!(*it)->getActive()) {
			timer->addTime(-1.0f);
			SimpleAudioEngine::sharedEngine()->playEffect("miss.wav");
			this->showPopupLabel("-1.0s", ccp((*it)->getPositionX(), 10));
			(*it)->removeFromParentAndCleanup(true);
			it = enemies.erase(it);
			continue;
		}
		it++;
	}
}

void MainScene::initForVariables() {
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getDesignResolutionSize();
	srand((unsigned)time(NULL));

	m_life = 0;
	m_respawnTime = 0;
	m_playingWarnTimer = false;
	m_warnTimerId = -1;
	ScoreManager::getInstance().resetScore();

	for (int type = 0; type < kEnemyCount; type++) {
		for (int i = 0; i < 3; i++) {
			CCSprite* sp = CCSprite::create(
				EnemySprite::getEnemyImageFileName((kEnemy)type));
			sp->setOpacity(0);
			sp->setAnchorPoint(ccp(1.0, 1.0));
			sp->setScale(0.3f);
			float width = sp->boundingBox().size.width;
			sp->setPosition(ccp(screenSize.width - 4 - (width + 4) * (2 - i), screenSize.height - 4));
			this->addChild(sp);
			m_chain_sprite.push_back(sp);
		}
	}

	for (int i = 0; i < 3; i++) {
		SimpleAudioEngine::sharedEngine()->preloadEffect(getSEFileName(i));
	}
	SimpleAudioEngine::sharedEngine()->preloadEffect("miss.wav");
	SimpleAudioEngine::sharedEngine()->preloadEffect("time.wav");
}

const char* MainScene::getSEFileName(int chain) {
	switch (chain) {
	case 1:
		return "gem-0.wav";
	case 2:
		return "gem-2.wav";
	case 3:
		return "gem-4.wav";
	default:
		return "gem-0.wav";
	}
}

void MainScene::spawnEnemy() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	for(int i = 0; i < 3; i++) {
		if (enemies.size() >= 5) {
			break;
		}
	    kEnemy enemyType = (kEnemy)(rand() % kEnemyCount);
		EnemySprite* enemy = EnemySprite::createWithEnemyType(enemyType);
        float r = (float)rand()/(float)RAND_MAX;
		CCSize enemySize = enemy->boundingBox().size;
		CCPoint enemyPos = ccp(
			(int)(enemySize.width / 2 + (winSize.width - enemySize.width) * r),
			winSize.height);
		enemy->setPosition(enemyPos);

		r = (((float)rand() / (float)RAND_MAX) * 4.0f) + 4.0f;
		CCPoint offset = CCPointMake(0, -1 * winSize.height - enemySize.height);
		CCMoveBy* move = CCMoveBy::create(r, offset);

		enemies.push_back(enemy);
		this->addChild(enemy);
		enemy->runAction(move);
	}
}

bool MainScene::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
	CCPoint touchPoint = this->convertTouchToNodeSpace(pTouch);
	EnemySprite* hitEnemy = NULL;
	list<EnemySprite*>::reverse_iterator it = enemies.rbegin();
	while(it != enemies.rend()) {
		if ((*it)->checkCollide(touchPoint)) {
		    hitEnemy = *it;
			break;
		}
		it++;
	}
	if (hitEnemy) {
		if (hitEnemy->hurt()) {
			// add score
			int score = ScoreManager::getInstance().killEnemy(hitEnemy);

			// remove animation
			CCSequence* anim = CCSequence::create(
				CCSpawn::create(
				CCEaseOut::create(CCScaleBy::create(0.3f, 1.5, 1.5), 2),
				CCEaseOut::create(CCFadeOut::create(0.3f), 2),
				NULL),
				CCCallFuncN::create(this, callfuncN_selector(MainScene::removeEnemy)),
				NULL
				);
			hitEnemy->runAction(anim);

			// Play SE
			SimpleAudioEngine::sharedEngine()->playEffect(
				getSEFileName(ScoreManager::getInstance().getChainMiniCount()));

			// Show Additional Score Label
			ostringstream os_score;
			os_score << "+" << score;
			CCPoint enemyPos = hitEnemy->getPosition();
			CCLabelTTF* additionalScoreLabel = 
				this->showPopupLabel(os_score.str().c_str(), enemyPos);

			// Show Chain Label
			if (ScoreManager::getInstance().getChainMiniCount() == 3) {
				ostringstream os_chain;
				os_chain << ScoreManager::getInstance().getChainCount() << "chain! +";
				float time = ceil(ScoreManager::getInstance().getChainCount() / 10.0f) * 0.5f;
				// max add 3s
				if (time >= 3.0f) time = 3.0f;
				os_chain << std::fixed << std::setprecision(1) << time << "s";
				this->showPopupLabel(os_chain.str().c_str(), 
					ccp(enemyPos.x,
					enemyPos.y + additionalScoreLabel->getContentSize().height));

				// add time
				TimerSprite* timer = static_cast<TimerSprite*>(this->getChildByTag(kTagProgress));
				timer->addTime(time);
			}
		}
	}

	return true;
}

void MainScene::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) {
	
}

void MainScene::removeEnemy(CCNode* node) {
	EnemySprite* e = static_cast<EnemySprite*>(node);
	if (e) {
		enemies.remove(e);
		e->removeFromParentAndCleanup(true);
	}
}

CCLabelTTF* MainScene::showPopupLabel(const char* str, CCPoint p) {
	CCLabelTTF* label = CCLabelTTF::create(
		str, "Verdana", 18.0f);
	label->setPosition(p);
	this->addChild(label);
	label->runAction(
		CCSequence::create(
		CCEaseOut::create(CCMoveBy::create(0.5, ccp(0, 10)), 2),
		CCEaseOut::create(CCFadeOut::create(0.5), 2),
		CCCallFuncN::create(this, callfuncN_selector(MainScene::releaseAfterAction)),
		NULL));
	return label;
}

void MainScene::releaseAfterAction(CCNode* node) {
	node->removeFromParentAndCleanup(true);
}

void MainScene::menuResetCallback(CCObject* obj) {
	CCDirector::sharedDirector()->replaceScene(
		static_cast<CCScene*>(MainScene::create()->scene()));
}

void MainScene::showReset() {
	
}