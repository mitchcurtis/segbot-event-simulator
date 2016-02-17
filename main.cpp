#include <QtCore>

class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator(const QString &filePath) :
        mOutputTextStream(stdout),
        mAngle(0),
        mSpeedLeft(0),
        mSpeedRight(0)
    {
        if (filePath == "stdin") {
            mInputTextStream.reset(new QTextStream(stdin));
        } else {
            mFile.setFileName(filePath);
            if (!mFile.open(QIODevice::ReadWrite))
                qWarning() << "Failed to open" << filePath;
        }
    }

    void start()
    {
        if (!mFile.isOpen() && !mInputTextStream)
            return;

        if (mTimer.isActive())
            return;

        connect(&mTimer, &QTimer::timeout, this, &Simulator::update);
        mTimer.start(100);
    }

private slots:
    void update()
    {
        QString line;
        if (mFile.isOpen())
            line = mFile.readLine();
        else
            line = mInputTextStream->readLine();

        if (line.isEmpty())
            return;

        if (!line.startsWith("?")) {
            qWarning() << "Status requests should start with '?'";
            return;
        }

        if (line.contains("angle")) {
            write(QString::number(mAngle));
        } else if (line.contains("speedLeft")) {
            write(QString::number(mSpeedLeft));
        } else if (line.contains("speedRight")) {
            write(QString::number(mSpeedRight));
        } else if (line.contains("distance")) {
            write(QString::number(0));
        }
    }

private:
    void write(const QString &line)
    {
        if (mFile.isOpen())
            mFile.write(qPrintable(line));
        else
            mOutputTextStream << line;
    }

    QTimer mTimer;
    QFile mFile;
    QScopedPointer<QTextStream> mInputTextStream;
    QTextStream mOutputTextStream;

    QPointF mPos;
    qreal mAngle;
    qreal mSpeedLeft;
    qreal mSpeedRight;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!a.arguments().contains("-file")) {
        qWarning() << "An input file is required; provide one with the -file parameter";
        return 1;
    }

    if (a.arguments().size() < 3) {
        qWarning() << "An input file must be specified after the -file parameter";
        return 1;
    }

    const QString filePath = a.arguments().at(2);
    Simulator simulator(filePath);
    simulator.start();

    return a.exec();
}

#include "main.moc"
