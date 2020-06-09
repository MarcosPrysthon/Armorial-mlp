#ifndef GETINFO_H
#define GETINFO_H

#include <entity/entity.h>
#include <entity/player/playeraccess.h>
#include <entity/contromodule/mrcteam.h>

class playerInfo{
public:
    bool valid;
    quint8 id;
    Position position;
    Angle orientation;
    float distToKicker;
};

class getInfo : public Entity
{

public:
    getInfo();

    QString name();

    void setTeams(MRCTeam *ourTeam , MRCTeam *theirTeam);

    //funcao chamada para captar informacoes, atualizar informacoes dos jogadores e gerar inputLine
    void fillInfo();

private:
    //armazenam informacoes dos jogadores em campo
    playerInfo kicker, allyGoalie, oppGoalie;
    playerInfo *ally;
    playerInfo *opp;

    //funcao usada para ordenar vetores de aliados e oponentes segundo a distToKicker
    void teamSort(playerInfo *team, int qtTeam);

    //funcao usada para gerar a inputLine
    std::string makeInputLine();

    void Kicker();
    void OppGoalie();
    void AllyGoalie();
    void Ally();
    void Opp();
    void initialization();
    void loop();
    void finalization();


    MRCTeam *_ourTeam;
    MRCTeam *_theirTeam;

    //VARS
    int qtAlly, qtOpp;
    QMutex teamMutex;
    QMutex playerMutex;
};

#endif // GETINFO_H

