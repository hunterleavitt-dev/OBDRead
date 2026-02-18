#ifndef READINESSPARSER_H
#define READINESSPARSER_H

#include <QObject>
#include "core/dto/ReadinessResult.h"

/**
 * @brief The ReadinessParser class
 * Parses OBD-II Mode 01 PID 01 response for readiness monitor status.
 */
class ReadinessParser : public QObject
{
    Q_OBJECT

public:
    explicit ReadinessParser(QObject *parent = nullptr);
    ~ReadinessParser();

    /**
     * @brief Parses raw OBD-II Mode 01 PID 01 response for readiness monitors.
     * @param rawData The raw hex response from the adapter (e.g., "41 01 XX XX XX XX").
     * @return ReadinessResult with monitor statuses populated.
     */
    ReadinessResult parseReadinessResponse(const QByteArray &rawData);
};

#endif // READINESSPARSER_H
