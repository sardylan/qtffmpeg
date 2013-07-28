#ifndef FFPROCESS_HPP
#define FFPROCESS_HPP

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class ffProcess : public QObject
{
    Q_OBJECT
public:
    explicit ffProcess(QObject *parent = 0);
    ~ffProcess();
    void start(QString, QStringList);
    
private:
    QProcess* process;

public slots:
    void stateChanged(QProcess::ProcessState state);
    void error(QProcess::ProcessError error);
};

#endif // FFPROCESS_HPP
