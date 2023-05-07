#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QTextEdit>

#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),edit_info(new std::map<QString,QLineEdit*>())
{
    m_settings = new QSettings("sidebar.ini", QSettings::IniFormat);
    ui->setupUi(this);
    ui->verticalLayout->addSpacing(10);

    // ip set
    auto * edit_ip = new QLineEdit("",this);
    (*edit_info)["ip"]=edit_ip;
    if (m_settings->value("ip","")!=""){
        edit_ip->setText( m_settings->value("ip","").toString() );
    }
    ui->formLayout->addRow("ip",edit_ip);

    // port set
    auto * edit_port = new QLineEdit("",this);
    (*edit_info)[QString("port")]=edit_port;
    if (m_settings->value("port","")!=""){
        edit_port->setText( m_settings->value("port","").toString() );
    }
    ui->formLayout->addRow("port",edit_port);

    // key set
    auto * edit_key = new QLineEdit("",this);
    (*edit_info)[QString("key")]=edit_key;
    if (m_settings->value("key","")!=""){
        edit_key->setText( m_settings->value("key","").toString() );
    }
    ui->formLayout->addRow("key",edit_key);
    ui->formLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
    ui->verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
    // add prompt setting
    button_list = new ButtonList();
    button_list->ReadFromSettings(m_settings);
    button_setting_broad = new ButtonSettingBorad(parent);
    ui->verticalLayout->addLayout(button_setting_broad);
    button_setting_broad->ReloadSetting(button_list);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_close_button_clicked() {
    this->hide();
}

void SettingsDialog::on_save_button_clicked() {
    for (auto & item : (*this->edit_info)) {
        m_settings->setValue(item.first,item.second->text());
    }
    this->button_list->SaveToSettings(this->m_settings);
    m_settings->sync();
    emit SettingChangedSend(m_settings);
    this->hide();
}
ButtonNode::ButtonNode(const QString &prompt, const QString &name, QWidget *parent): prompt(prompt), name(name) {
    button = new QPushButton(name,parent);
}

ButtonNode::ButtonNode(const QString &prompt, const QString &name) : prompt(prompt), name(name) {
    button= nullptr;
}

void ButtonList::SaveToSettings(QSettings *settings) {
    settings->setValue("btns",list_buttons.size());
    for (int i = 0; i < list_buttons.size(); ++i) {
        auto & btn = list_buttons[i];
        {
            QString key_name = QString("btn.prompt.%1").arg(i);
            settings->setValue(key_name,btn.prompt);
        }
        {
            QString key_name = QString("btn.name.%1").arg(i);
            settings->setValue(key_name,btn.name);
        }
    }
}

void ButtonList::ReadFromSettings(QSettings *settings) {
    auto cnts = settings->value("btns","-1").toInt();
    if (cnts == -1 ){
        list_buttons.push_back(ButtonNode("请你用中文帮我解释该代码:","解释"));
        list_buttons.push_back(ButtonNode("请你将以下内容翻译为中文，其中术语可能偏向于计算机行业:","翻译"));
        list_buttons.push_back(ButtonNode("请你用中文，就以下内容进行创作:","创作"));
        list_buttons.push_back(ButtonNode("请你用中文，总结以下内容:","归纳"));
        SaveToSettings(settings);
    }else {
        list_buttons.clear();
        for (int i = 0; i < cnts; i++) {
            QString prompt_key = QString("btn.prompt.%1").arg(i);
            QString name_key = QString("btn.name.%1").arg(i);
            QString prompt = settings->value(prompt_key, "").toString();
            QString name = settings->value(name_key, "").toString();
            if (!prompt.isEmpty() && !name.isEmpty()) {
                list_buttons.push_back(ButtonNode(prompt, name));
            }
        }
    }
}

ButtonSettingBorad::ButtonSettingPair::ButtonSettingPair(QWidget *parent, const QString &labelText, const QString &prompt)
        : QHBoxLayout(parent), label_text(labelText), prompt(prompt) {
    auto * button = new QPushButton("-");
    button->setMaximumSize(25,25);
    this->addWidget(button);
    name = new QLineEdit(parent);
    name->setText(labelText);
    name->setMaximumWidth(50);
    this->addWidget(name);
    content = new QLineEdit(parent);
    content->setText(prompt);
    this->addWidget(content);
    this->addSpacing(10);
}

ButtonSettingBorad::ButtonSettingPair::~ButtonSettingPair() {
    if (content != nullptr){
        delete content;
        content= nullptr;
    }
    if (name != nullptr){
        delete name;
        name = nullptr;
    }
}

ButtonSettingBorad::ButtonSettingBorad(QWidget *parent) : QVBoxLayout(parent) {
    box = new QVBoxLayout(parent);
    add_button = new QPushButton("添加prompt",parent);
    this->addSpacing(10);
    this->addLayout(box);
    this->addWidget(add_button);
}

ButtonSettingBorad::~ButtonSettingBorad() {
    if (box != nullptr){
        delete box;
        box = nullptr;
    }
    if (add_button!= nullptr){
        delete add_button;
        add_button = nullptr;
    }

}

void ButtonSettingBorad::ReloadSetting(ButtonList *list) {
    if (list== nullptr){
        return;
    }
    QLayoutItem *child;
    while ((child = box->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    for (auto & item : list->list_buttons) {
        auto * line = new ButtonSettingPair(nullptr,item.name,item.prompt);
        box->addLayout(line);
    }
}
