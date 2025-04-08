QVector<int> timeIndexes;
QVector<std::shared_ptr<Command>> timeCommands;

// 1. Сохраняем команды с TimeBind::Time и их индексы
for (int i = 0; i < this->size(); ++i) {
    if (this->at(i)->getTimeBind().getType() == TimeBind::Time) {
        timeIndexes.append(i);
        timeCommands.append(this->at(i));
    }
}

// 2. Сортируем команды по времени
std::stable_sort(timeCommands.begin(), timeCommands.end(), [](const std::shared_ptr<Command>& a,
                                                              const std::shared_ptr<Command>& b) {
    return a->getTimeBind().getDtime() < b->getTimeBind().getDtime();
});

// 3. Возвращаем отсортированные команды на исходные позиции
for (int i = 0; i < timeIndexes.size(); ++i) {
    (*this)[timeIndexes[i]] = timeCommands[i];
}