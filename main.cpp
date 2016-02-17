#include <QtCore>

class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator(QFile *file) :
        mFile(file)
    {

    }

    void start()
    {
        if (mTimer.isActive())
            return;

        connect(&mTimer, &QTimer::timeout, this, &Simulator::update);
        mTimer.start(100);
    }

private slots:
    void update()
    {
        QByteArray line = mFile->readLine();
        if (line.isEmpty())
            return;

        if (!line.startsWith("?")) {
            qWarning() << "Status requests should start with '?'";
            return;
        }
    }

private:
    QTimer mTimer;
    QFile *mFile;

    QPointF mPos;
    qreal mRotation;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!a.arguments().contains("-file")) {
        qWarning() << "An input file is required; provide one with the -file parameter";
        return 1;
    }

    if (a.arguments().size() < 2) {
        qWarning() << "An input file must be specified after the -file parameter";
        return 1;
    }

    const QString filePath = a.arguments().at(1);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open" << filePath;
        return 1;
    }

    Simulator simulator(&file);
    simulator.start();

    return a.exec();
}

#include "main.moc"
