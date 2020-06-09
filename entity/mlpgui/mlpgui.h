#ifndef MLPGUI_H
#define MLPGUI_H

#include <entity/entity.h>
#include "mlpmainwindow.h"
#include <entity/getinfo/getinfo.h>

class MlpGui : public Entity
{
public:
    MlpGui();

    QString name();

    getInfo *_infoModule;
    bool passBtnClicked;
    bool kickBtnClicked;

private:

    void initialization();
    void loop();
    void finalization();

    MlpMainWindow *_mlpGUI;
    QMutex mutex;
};

#endif // MLPGUI_H
