#include "processcategory.h"
#include "myimagebutton.h"

ProcessCategory::ProcessCategory(int tabIndex, QWidget *parent)
    : QWidget(parent)
    ,width(26)
    ,height(26)
    ,activeIndex(tabIndex)
{
    setFixedSize(width * 3, height);

    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    activeProcessButton = new MyImageButton(this);
    activeProcessButton->setCheckable(true);
    activeProcessButton->setObjectName("ActiveProcessBtn");

    userProcessButton = new MyImageButton(this);
    userProcessButton->setCheckable(true);
    userProcessButton->setObjectName("UserProcessBtn");

    allProcessButton = new MyImageButton(this);
    allProcessButton->setCheckable(true);
    allProcessButton->setObjectName("AllProcessBtn");

    if (activeIndex == 0) {
        activeProcessButton->setChecked(true);
        userProcessButton->setChecked(false);
        allProcessButton->setChecked(false);
    }
    else if (activeIndex == 2) {
        activeProcessButton->setChecked(false);
        userProcessButton->setChecked(false);
        allProcessButton->setChecked(true);
    }
    else {
        activeProcessButton->setChecked(false);
        userProcessButton->setChecked(true);
        allProcessButton->setChecked(false);
    }


    connect(activeProcessButton, &MyImageButton::clicked, this, [=] {
        activeIndex = 0;
        emit this->activeWhoseProcessList(activeIndex);
        activeProcessButton->setChecked(true);
        userProcessButton->setChecked(false);
        allProcessButton->setChecked(false);
    });
    connect(userProcessButton, &MyImageButton::clicked, this, [=] {
        activeIndex = 1;
        emit this->activeWhoseProcessList(activeIndex);
        activeProcessButton->setChecked(false);
        userProcessButton->setChecked(true);
        allProcessButton->setChecked(false);
    });
    connect(allProcessButton, &MyImageButton::clicked, this, [=] {
        activeIndex = 2;
        emit this->activeWhoseProcessList(activeIndex);
        activeProcessButton->setChecked(false);
        userProcessButton->setChecked(false);
        allProcessButton->setChecked(true);
    });

    layout->addWidget(activeProcessButton);
    layout->addWidget(userProcessButton);
    layout->addWidget(allProcessButton);
    this->setLayout(layout);
}

ProcessCategory::~ProcessCategory()
{
    delete activeProcessButton;
    delete userProcessButton;
    delete allProcessButton;
    delete layout;
}
