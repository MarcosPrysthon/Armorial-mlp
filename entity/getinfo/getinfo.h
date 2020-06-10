#ifndef GETINFO_H
#define GETINFO_H

#include <entity/entity.h>
#include <entity/player/playeraccess.h>
#include <entity/contromodule/mrcteam.h>
#include <fstream>

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

    /*funcao chamada quando um botao for pressionado
    escreve input+output no dataset */
    void writeOnDataset(std::string output);

private:
    //arquivo dataset
    std::fstream dataset;

    //armazenam informacoes dos jogadores em campo
    playerInfo kicker, allyGoalie, oppGoalie;
    playerInfo *ally;
    playerInfo *opp;

    //funcao chamada para captar informacoes, atualizar informacoes dos jogadores e gerar inputLine
    void fillInfo();

    //funcao usada para ordenar vetores de aliados e oponentes segundo a distToKicker
    void teamSort(playerInfo *team, int qtTeam);

    //funcao usada para gerar a inputLine
    void makeInputLine();
    std::string inputLine;

    //funcoes usada para "uniformizar" coordenadas e orientacoes(como se fosse sempre o time da esquerda)
    void fixInfo();
    playerInfo calc(playerInfo player);

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

