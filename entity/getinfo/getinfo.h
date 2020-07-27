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

class point{
public:
    float x, y;
};

class triangle{
public:
    point A, B, C;
    float AB, BC, CA;
    float area;
    void calcSidesAndArea(){
        float s;
        AB = sqrt(pow((B.y - A.y), 2) + pow((B.x - A.x), 2));
        BC = sqrt(pow((C.y - B.y), 2) + pow((C.x - B.x), 2));
        CA = sqrt(pow((A.y - C.y), 2) + pow((A.x - C.x), 2));
        s = (AB+BC+CA)/2;
        area = sqrt(qAbs(s*(s-AB)*(s-BC)*(s-CA)));
    }
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
    //path para o arquivo do dataset
    QString QdatasetPath = QDir().filePath("../entity/getinfo/dataset.txt");
    std::string datasetPath = QdatasetPath.toUtf8().constData();

    //arquivo dataset
    std::fstream dataset;

    //armazenam informacoes dos jogadores em campo
    playerInfo kicker, allyGoalie, oppGoalie;
    playerInfo *ally;
    playerInfo *opp;
    playerInfo *oppObst;

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

    //encontrar jogadores adversarios que estejam entre o kicker e o gol
    void OppObst();

    void Kicker();
    void OppGoalie();
    void AllyGoalie();
    void Ally();
    void Opp();
    void initialization();
    void loop();
    void finalization();
    void testMLP();

    MRCTeam *_ourTeam;
    MRCTeam *_theirTeam;

    //VARS
    int qtAlly, qtOpp, qtOppObst;
    QMutex teamMutex;
    QMutex playerMutex;
};

#endif // GETINFO_H

