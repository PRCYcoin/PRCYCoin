// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2020 The DAPS Project developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "togglebutton.h"
#include "ui_togglebutton.h"


#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

ToggleButton::ToggleButton(QWidget* parent) : QWidget(parent),
                                              ui(new Ui::ToggleButton)
{
    ui->setupUi(this);
    ui->pushButton0->setCheckable(true);
    ui->pushButton1->setCheckable(true);
    connect(ui->pushButton0, SIGNAL(clicked()), this, SLOT(toggle()));
    connect(ui->pushButton1, SIGNAL(clicked()), this, SLOT(toggle()));
    state = false;

    update();
}

ToggleButton::~ToggleButton()
{
    delete ui;
}

void ToggleButton::setOptionA(QString label)
{
    ui->pushButton1->setText(label);
}

void ToggleButton::setOptionB(QString label)
{
    ui->pushButton0->setText(label);
}

void ToggleButton::setLayoutDirection(Qt::LayoutDirection Dir)
{
    direction = Dir;
    QWidget::setLayoutDirection(Dir);
}

bool ToggleButton::getState()
{
    return state;
}

void ToggleButton::setState(bool value)
{
    state = value;
    update();
}

void ToggleButton::paintEvent(QPaintEvent* e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    if (direction != Qt::RightToLeft)
        ui->pushButton1->move(this->width() - ui->pushButton1->width() - ui->pushButton0->x(), ui->pushButton1->y());
    else if (state)
        ui->pushButton0->move(this->width() - ui->pushButton0->width() - ui->pushButton1->x(), ui->pushButton0->y());
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ToggleButton::resizeEvent(QResizeEvent* event)
{
    resize();
}

void ToggleButton::resize()
{
    this->setFixedWidth(this->width());
    double large = this->width() * .55;
    double small = this->width() * .4;
    if (state) {
        ui->pushButton0->setFixedWidth(large);
        ui->pushButton1->setFixedWidth(small);
    } else {
        ui->pushButton0->setFixedWidth(small);
        ui->pushButton1->setFixedWidth(large);
    }
}

void ToggleButton::update()
{
    ui->pushButton0->setChecked(!state);
    ui->pushButton1->setChecked(state);

    if (state)
        ui->pushButton0->stackUnder(ui->pushButton1);
    else
        ui->pushButton1->stackUnder(ui->pushButton0);
    resize();
    this->repaint();
}

void ToggleButton::toggle()
{
    state = !state;
    Q_EMIT stateChanged(this);
    update();
}
