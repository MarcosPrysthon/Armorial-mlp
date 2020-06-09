#include <iostream>

#include "mlpgui.h"
#include <entity/entity.h>


QString MlpGui::name(){
    return "MLPGUI Module";
}

MlpGui::MlpGui() : Entity(Entity::ENT_MLPGUI){
    _mlpGUI = new MlpMainWindow();
    _mlpGUI->show();
}

void MlpGui::initialization(){
    std::cout << "[MLPGUI] Entity started" << std::endl;
}

void MlpGui::loop(){

    mutex.lock();

    passBtnClicked = _mlpGUI->passBtnClicked;
    kickBtnClicked = _mlpGUI->kickBtnClicked;

    if(passBtnClicked){
        _infoModule->fillInfo();
        _mlpGUI->resetVars();
        std::cout << "debugando" << std::endl;
    } else if(kickBtnClicked){
        _infoModule->fillInfo();
        _mlpGUI->resetVars();
        std::cout << "debugando" << std::endl;
    }

    mutex.unlock();
}

void MlpGui::finalization(){
    std::cout << "[MLPGUI] Entity stopped" << std::endl;
}
