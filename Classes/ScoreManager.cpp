#include "ScoreManager.h"
#include "EnemySprite.h"
#include <algorithm>

ScoreManager::ScoreManager(void)
	: m_score(0), m_chain(1), m_chain_mini(0), m_max_chain(1)
{
}

ScoreManager& ScoreManager::getInstance() {
	static ScoreManager instance;
	return instance;
}

ScoreManager::~ScoreManager(void)
{
}

int ScoreManager::killEnemy(const EnemySprite* enemy) {
	int base = 10;
	if (m_chain_mini == 0) {
		m_chain_color = enemy->getEnemyType();
		m_chain_mini++;
	} else if (m_chain_color != enemy->getEnemyType()) {
		m_chain_color = enemy->getEnemyType();
		if (m_chain_mini != 3) m_chain = 1;
		m_chain_mini = 1;
	} else {
		m_chain_mini = m_chain_mini == 3 ? 1 : m_chain_mini + 1;
		if (m_chain_mini == 3) m_chain++;
	}
	m_score += base * m_chain;
	m_max_chain = std::max<int>(m_max_chain, m_chain);
	return base * m_chain;
}

int ScoreManager::getScore() const {
	return m_score;
}

int ScoreManager::getChainCount() const {
	return m_chain;
}

int ScoreManager::getChainMiniCount() const {
	return m_chain_mini;
}

kEnemy ScoreManager::getChainColor() const {
	return m_chain_color;
}

int ScoreManager::getMaxChain() const {
	return m_max_chain;
}

void ScoreManager::resetScore() {
	m_score = 0;
	m_chain = 1;
	m_chain_mini = 0;
	m_max_chain = 1;
}