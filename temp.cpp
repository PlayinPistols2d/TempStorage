void RainbowButton::cleanupOldEmojis()
{
    if (m_activeSpawnGroups.isEmpty()) return;

    int oldestGroup = m_activeSpawnGroups.dequeue();

    auto it = m_activeParticles.begin();
    while (it != m_activeParticles.end()) {
        EmojiParticle* emoji = *it;
        if (emoji->spawnGroupId == oldestGroup) {
            emoji->deleteParticle(); // Safe cleanup
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
        moveTimer->deleteLater(); // Don't manually delete, use deleteLater()
        moveTimer = nullptr;
    }
    if (fadeAnimation) {
        fadeAnimation->stop();
        fadeAnimation->deleteLater(); // Don't manually delete
        fadeAnimation = nullptr;
    }

    hide(); // Hide the emoji immediately to remove it from rendering
    deleteLater(); // Schedule safe deletion
}