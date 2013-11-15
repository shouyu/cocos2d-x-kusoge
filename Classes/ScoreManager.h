#pragma once
#include "Config.h"

class EnemySprite;

class ScoreManager
{
public:
	~ScoreManager(void);
	static ScoreManager& getInstance();

	int killEnemy(const EnemySprite*);
	int getScore() const;
	kEnemy getChainColor() const;
	int getChainCount() const;
	int getChainMiniCount() const;
	int getMaxChain() const;
	void resetScore();

private:
	ScoreManager(void);
	ScoreManager(const ScoreManager& r);
	ScoreManager& operator=(const ScoreManager& r);
	int m_score;
	int m_chain;
	int m_chain_mini;
	int m_max_chain;
	kEnemy m_chain_color;
};

