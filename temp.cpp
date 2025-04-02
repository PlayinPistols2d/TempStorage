void RainbowButton::cleanupOldEmojis()
{
    if (m_activeSpawnGroups.isEmpty()) return;

    int oldestGroup = m_activeSpawnGroups.dequeue();

    auto it = m_activeParticles.begin();
    while (it != m_activeParticles.end()) {
        EmojiParticle* emoji = *it;
        if (emoji->spawnGroupId == oldestGroup) {
            emoji->deleteParticle(); // Fully remove everything
            it = m_activeParticles.erase(it);
        } else {
            ++it;
        }
    }
}




void EmojiParticle::deleteParticle()
{
    if (moveTimer) {
        moveTimer->stop();
        delete moveTimer;
        moveTimer = nullptr;
    }
    if (fadeAnimation) {
        fadeAnimation->stop();
        delete fadeAnimation;
        fadeAnimation = nullptr;
    }

    deleteLater(); // Fully remove the object
}