#ifndef PROCESSCATEGORY_H
#define PROCESSCATEGORY_H

#include <QWidget>
#include <QHBoxLayout>

class MyImageButton;

class ProcessCategory : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProcessCategory(int tabIndex, QWidget *parent = 0);
    ~ProcessCategory();
    
signals:
    void activeWhoseProcessList(int index);

private:
    int width;
    int height;
    int activeIndex;
    QHBoxLayout *layout;
    MyImageButton *activeProcessButton;
    MyImageButton *userProcessButton;
    MyImageButton *allProcessButton;
};

#endif // PROCESSCATEGORY_H
