//
// Created by 48449 on 2023/4/15.
//

#ifndef CHATGPT_SIDEBAR_DEALCHATGPT_H
#define CHATGPT_SIDEBAR_DEALCHATGPT_H
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QSettings>
class DealChatGpt {
public:
    DealChatGpt();
    QTextEdit * out;

    void TalkWithGpt(QString text,QTextEdit * out);
    QNetworkAccessManager* networkManager;
    QSettings * m_setting;
public:
    void setupNetworkManager(QObject *parent = nullptr);
    virtual void fetchAnswerFromGPT3(const QString& text) = 0;
    virtual void showSetting() = 0;
};


#endif //CHATGPT_SIDEBAR_DEALCHATGPT_H
