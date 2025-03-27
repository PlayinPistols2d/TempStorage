#include "mcihexconverter.h"
#include "qdebug.h"
#include "qendian.h"
#include <QJsonArray>
#include <QtMath>
#include <QDateTime>
#include <QDataStream>

extern QStringList allowedTypes;

#define cmdvalue "value"
#define cmdtype "type"

MciHexConverter::MciHexConverter() {}

QByteArray MciHexConverter::convertToHexBytes(double value, const QString& type, int wordCount) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Вместо int теперь две ветки: "dint" (прежняя логика int)
    // и "rint" (с «обратным кодом»).
    if (type == "dint") {
        qint32 intValue = static_cast<qint32>(value);
        stream << intValue;
    } else if (type == "rint") {
        qint32 intValue = static_cast<qint32>(value);
        // Если требуется именно инвертирование бит (one’s complement):
        intValue = ~intValue;
        // Если нужно было просто -intValue (two’s complement), 
        // то закомментируйте строку выше и раскомментируйте следующую:
        // intValue = -intValue;
        stream << intValue;
    } else if (type == "uint") {
        quint32 uintValue = static_cast<quint32>(value);
        stream << uintValue;
    } else if (type == "float") {
        float floatValue = static_cast<float>(value);
        stream.writeRawData(reinterpret_cast<const char*>(&floatValue), 4);
    } else if (type == "double") {
        stream.writeRawData(reinterpret_cast<const char*>(&value), 8);
    }

    // Дополнение нулями, если байт меньше, чем нужно слов.
    if (data.size() < wordCount * 2) {
        data.append(QByteArray(wordCount * 2 - data.size(), 0)); 
    }

    return data;
}

QVector<quint16> MciHexConverter::applyEndianSwap(const QVector<quint16>& words) {
    QVector<quint16> swappedWords;

    if (words.size() > 1) {
        for (int i = words.size() - 1; i >= 0; --i) {
            swappedWords.append(words[i]);
        }
    } else {
        swappedWords = words;
    }

    return swappedWords;
}

QStringList MciHexConverter::convert(RawKpi &raw)
{
    QStringList hexs;
    for(auto i = raw.begin(); i < raw.end(); )
    {
        qDebug() << "TEST" << i->getName();

        JsonValidator validator = JsonValidator::from(i->getCurrentGuiInfo());
        QString type = validator.get<QString>(cmdtype, "");

        // Важно: убедитесь, что allowedTypes содержит "dint" и "rint", а не "int".
        if(allowedTypes.contains(type))
        {
            QVector<quint16> result;
            QByteArray buffer;
            QDataStream stream(&buffer, QIODevice::WriteOnly);
            stream.setByteOrder(QDataStream::LittleEndian);

            int currentWord = i->getParams().isEmpty() ? -1 : i->getParams().first().getStartWord();
            quint16 tempWord = 0;

            for (int j = 0; j < i->getParams().size(); ++j)
            {
                auto param =  i->getParameter(j);

                // Обработка "пробелов" между словами:
                if (param.getStartWord() > currentWord + 1) {
                    int gap = param.getStartWord() - (currentWord + 1);
                    for (int k = 0; k < gap; ++k) {
                        result.append(0x0000);
                    }
                }

                int wordCount = param.getEndWord() - param.getStartWord() + 1;
                QByteArray hexData = convertToHexBytes(param.getValue(), param.getTp(), wordCount);

                if (param.getStartWord() == param.getEndWord()) {
                    // Параметр умещается в 1 слово (16 бит).
                    if (param.getStartBit() == 0 && param.getEndBit() == 15) {
                        // Полное слово
                        result.append(qFromLittleEndian<quint16>(
                                         reinterpret_cast<const uchar*>(hexData.constData())));
                        tempWord = 0;  // Очистка буфера
                    } else {
                        // Частичное слово
                        int bitSize = param.getEndBit() - param.getStartBit() + 1;
                        quint16 mask = (1 << bitSize) - 1;
                        quint16 value = qFromLittleEndian<quint16>(
                                            reinterpret_cast<const uchar*>(hexData.constData()))
                                        & mask;

                        tempWord |= (value << param.getStartBit());

                        // Проверяем, есть ли еще параметры в том же слове:
                        bool hasMoreParamsInSameWord = false;
                        if (j + 1 < i->getParams().size()) {
                            if (i->getParameter(j+1).getStartWord() == param.getStartWord()) {
                                hasMoreParamsInSameWord = true;
                            }
                        }

                        // Если это последний параметр в данном слове — добавляем в результат.
                        if (!hasMoreParamsInSameWord) {
                            result.append(tempWord);
                            tempWord = 0;
                        }
                    }
                } else {
                    // Параметр занимает несколько слов
                    for (int jByte = 0; jByte < hexData.size(); jByte += 2) {
                        quint16 word = qFromLittleEndian<quint16>(
                                           reinterpret_cast<const uchar*>(hexData.constData() + jByte));
                        result.append(word);
                    }
                }

                currentWord = param.getEndWord();
            }

            // Если tempWord остался непустым — добавим его в результат
            if (tempWord != 0) {
                result.append(tempWord);
            }

            // Применяем общую энд swap-операцию, если нужно
            applyEndianSwap(result);

            // Переводим 16-битные слова в шестнадцатеричные строки
            auto prepWords = convertWordsToHexStrings(result);
            for (const auto &str : prepWords) {
                hexs.append(str);
            }

            i = raw.erase(i);
        }
        else {
            i++;
        }
    }
    return hexs;
}

QStringList MciHexConverter::convertWordsToHexStrings(const QVector<quint16> &words)
{
    QStringList list;
    for (quint16 w : words) {
        QString hexStr = QString("%1").arg(w, 4, 16, QChar('0'));
        list << hexStr;
    }
    return list;
}
