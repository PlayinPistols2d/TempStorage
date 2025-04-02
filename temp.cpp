void RainbowButton::cleanupOldEmojis()
{
    if (m_activeSpawnGroups.isEmpty()) return;

    int oldestGroup = m_activeSpawnGroups.dequeue();
    auto it = m_activeParticles.begin();
    
    while (it != m_activeParticles.end()) {
        if ((*it)->spawnGroupId == oldestGroup) {
            (*it)->startFadeOut();
            it = m_activeParticles.erase(it); // Remove it immediately
        } else {
            ++it;
        }
    }
}


const int MAX_TOTAL_EMOJIS = 50;  // Adjust as needed

void RainbowButton::spawnEmojis(const QPoint& clickPos)
{
    static QStringList emojis = { "🎉", "🥳", "✨", "🔥", "💖", "🎊", "😃", "🌟", "🎁", "👾"};

    if (m_activeParticles.size() >= MAX_TOTAL_EMOJIS) {
        cleanupOldEmojis();
    }

    if (m_activeSpawnGroups.size() >= 3) {
        cleanupOldEmojis();
    }

    m_currentSpawnGroup++;
    m_activeSpawnGroups.enqueue(m_currentSpawnGroup);

    QPoint globalPos = mapTo(window(), clickPos);

    for (int i = 0; i < 10; ++i) {
        if (m_activeParticles.size() >= MAX_TOTAL_EMOJIS) break; // Prevent overload

        QPoint spawnPos = globalPos + QPoint(
                              QRandomGenerator::global()->bounded(-15, 16),
                              QRandomGenerator::global()->bounded(-15, 16)
                              );

        auto *emoji = new EmojiParticle(
            emojis[QRandomGenerator::global()->bounded(emojis.size())],
            spawnPos,
            window()
            );
        emoji->spawnGroupId = m_currentSpawnGroup;
        emoji->show();

        m_activeParticles.append(emoji);
        connect(emoji, &QObject::destroyed, this, [this, emoji]() {
            m_activeParticles.removeAll(emoji);
        });
    }
}