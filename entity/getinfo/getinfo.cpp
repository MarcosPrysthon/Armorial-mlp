#include "getinfo.h"
#include <entity/locations.h>
#include <entity/player/player.h>
#include <entity/player/playerbus.h>
#include "const/constants.h"
#include "const/mlp.h"
#include <string>

QString getInfo::name(){
    return "GETINFO Module";
}

getInfo::getInfo() : Entity(Entity::ENT_GETINFO)
{
}

void getInfo::setTeams(MRCTeam *ourTeam, MRCTeam *theirTeam){
    teamMutex.lock();
    _ourTeam = ourTeam;
    _theirTeam = theirTeam;
    teamMutex.unlock();
}

void getInfo::Kicker(){
    bool foundKicker = false;
    //Determinar o jogador com a bola (kicker)
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::ourPlayerAvailable(id)){  //verifica se o id é valido
            if(PlayerBus::ourPlayer(id)->hasBallPossession()){  //verifica se tem a bola
                foundKicker = true;
                kicker.valid = true;
                kicker.id = PlayerBus::ourPlayer(id)->playerId();
                kicker.position = PlayerBus::ourPlayer(id)->position();
                kicker.orientation = PlayerBus::ourPlayer(id)->orientation();
                //std::cout << "[GETINFO] KICKER " << kicker.position.x() << " - " << kicker.position.y() <<std::endl; // printa as posiçoes x e y do kicker
            }
        }
    }
    if(!foundKicker){
        kicker.valid = false;
    }
}

void getInfo::OppGoalie(){
    //Determinar quem é o goleiro(OppGoalie)

    float  menor_dist = 9999.0;
    float  dist;

    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::theirPlayerAvailable(id)){    //verifica se o id é valido
            dist = PlayerBus::theirPlayer(id)->distOurGoal();   //gol do time adversario
            if(dist < menor_dist){
                menor_dist = dist;
                oppGoalie.valid = true;
                oppGoalie.id = PlayerBus::theirPlayer(id)->playerId();
                oppGoalie.position = PlayerBus::theirPlayer(id)->position();
                oppGoalie.orientation = PlayerBus::theirPlayer(id)->orientation();
                oppGoalie.distToKicker = PlayerBus::theirPlayer(id)->distanceTo(kicker.position);
            }
        }
    }

    if(oppGoalie.valid){    //verifica se foi encontrado o oppGoalie
        // printa as posiçoes x e y do oppgoalie
        //std::cout << "[GETINFO] OPPGOALIE READY" << std::endl;
    }else{
        oppGoalie.valid = false;
    }
}

void getInfo::AllyGoalie(){
    //Determinar quem é o goleiro aliado (AllyGoalie)

    float  menor_dist = 9999.0;
    float  dist;
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::ourPlayerAvailable(id)){    //verifica se o id é valido
            dist = PlayerBus::ourPlayer(id)->distOurGoal();   //gol do time
            if(dist < menor_dist){
                menor_dist = dist;
                allyGoalie.valid = true;
                allyGoalie.id = PlayerBus::ourPlayer(id)->playerId();
                allyGoalie.position = PlayerBus::ourPlayer(id)->position();
                allyGoalie.orientation = PlayerBus::ourPlayer(id)->orientation();
                allyGoalie.distToKicker = PlayerBus::ourPlayer(id)->distanceTo(kicker.position);
            }
        }
    }

    if(allyGoalie.valid){    //verifica se foi encontrado o allyGoalie
        // printa as posiçoes x e y do allygoalie
        //std::cout << "[GETINFO] ALLYGOALIE READY" << std::endl;
    }else{
        allyGoalie.valid = false;
    }
}

void getInfo::Ally(){
    int i=0;
    //guardar informacoes dos aliados em campo (exceto kicker e goleiro)
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::ourPlayerAvailable(id) && id != kicker.id && id != allyGoalie.id){
            ally[i].valid = true;
            ally[i].id = PlayerBus::ourPlayer(id)->playerId();
            ally[i].position = PlayerBus::ourPlayer(id)->position();
            ally[i].orientation = PlayerBus::ourPlayer(id)->orientation();
            ally[i].distToKicker = PlayerBus::ourPlayer(id)->distanceTo(kicker.position);
            i++;
            qtAlly++;
        }
    }
    //std::cout << "[GETINFO] ALLY READY" << std::endl;
}

void getInfo::Opp(){
    int i=0;
    //guardar informacoes dos aliados em campo (exceto goleiro)
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::theirPlayerAvailable(id) && id != oppGoalie.id){
            opp[i].valid = true;
            opp[i].id = PlayerBus::theirPlayer(id)->playerId();
            opp[i].position = PlayerBus::theirPlayer(id)->position();
            opp[i].orientation = PlayerBus::theirPlayer(id)->orientation();
            opp[i].distToKicker = PlayerBus::theirPlayer(id)->distanceTo(kicker.position);
            i++;
            qtOpp++;
        }
    }
    //std::cout << "[GETINFO] OPP READY" << std::endl;
}

void getInfo::fillInfo(){
    //std::cout << std::endl;

    playerMutex.lock();

    //inicializando variaveis
    qtAlly = 0;
    qtOpp = 0;
    inputLine.clear();
    //inicializando informacoes dos jogadores com valores default
    kicker.valid = false;
    kicker.id = 99;
    kicker.position.setPosition(0, 0, 0);
    kicker.orientation.setValue(0);
    allyGoalie = kicker;
    oppGoalie = kicker;
    for(int i = 0; i < MRCConstants::_qtPlayers - 1; i++){
        opp[i] = kicker;
        if(i < MRCConstants::_qtPlayers - 2){
            ally[i] = kicker;
        }
    }
    //captando informacoes dos jogadores em campo
    Kicker();
    OppGoalie();
    AllyGoalie();
    Opp();
    Ally();
    teamSort(ally, qtAlly);
    teamSort(opp, qtOpp);
    fixInfo();
    if(kicker.valid) testMLP();
    //std::cout << std::endl;

    playerMutex.unlock();
}

void getInfo::makeInputLine(){
/*
 * ordem das entradas:
 * dados do kicker, dados do ally0, dados do opp0, dados do oppGoalie,
 * dados do ally1, dados do ally2, dados do ally3, dados do allyGoalie,
 * dados do opp1, dados do opp2, dados do opp3, dados do opp4;
*/
    //kicker
    inputLine += std::to_string(kicker.position.x()) + " " + std::to_string(kicker.position.y()) + " " + std::to_string(kicker.orientation.value()) + " ";
    //ally0
    inputLine += std::to_string(ally[0].position.x()) + " " + std::to_string(ally[0].position.y()) + " " + std::to_string(ally[0].orientation.value()) + " ";
    //opp0
    inputLine += std::to_string(opp[0].position.x()) + " " + std::to_string(opp[0].position.y()) + " " + std::to_string(opp[0].orientation.value()) + " ";
    //oppGoalie
    inputLine += std::to_string(oppGoalie.position.x()) + " " + std::to_string(oppGoalie.position.y()) + " " + std::to_string(oppGoalie.orientation.value()) + " ";
    //ally1
    inputLine += std::to_string(ally[1].position.x()) + " " + std::to_string(ally[1].position.y()) + " " + std::to_string(ally[1].orientation.value()) + " ";
    //ally2
    inputLine += std::to_string(ally[2].position.x()) + " " + std::to_string(ally[2].position.y()) + " " + std::to_string(ally[2].orientation.value()) + " ";
    //ally0
    inputLine += std::to_string(ally[3].position.x()) + " " + std::to_string(ally[3].position.y()) + " " + std::to_string(ally[3].orientation.value()) + " ";
    //allyGoalie
    inputLine += std::to_string(allyGoalie.position.x()) + " " + std::to_string(allyGoalie.position.y()) + " " + std::to_string(allyGoalie.orientation.value()) + " ";
    //opp1
    inputLine += std::to_string(opp[1].position.x()) + " " + std::to_string(opp[1].position.y()) + " " + std::to_string(opp[1].orientation.value()) + " ";
    //opp2
    inputLine += std::to_string(opp[2].position.x()) + " " + std::to_string(opp[2].position.y()) + " " + std::to_string(opp[2].orientation.value()) + " ";
    //opp3
    inputLine += std::to_string(opp[3].position.x()) + " " + std::to_string(opp[3].position.y()) + " " + std::to_string(opp[3].orientation.value()) + " ";
    //opp4
    inputLine += std::to_string(opp[4].position.x()) + " " + std::to_string(opp[4].position.y()) + " " + std::to_string(opp[4].orientation.value()) + " ";

    //std::cout << "[GETINFO] INPUT LINE READY" << std::endl;
}

void getInfo::writeOnDataset(std::string output){
    playerMutex.lock();
    if(kicker.valid){
        makeInputLine();
        string datasetLine = inputLine + output;
        //abrindo arquivo dataset
        dataset.open(datasetPath, std::fstream::out | std::fstream::app);
        if(!dataset.is_open()){
            std::cout << "[GETINFO] Erro ao abrir arquivo" << std::endl;
        }else{
            //escrevendo linha
            dataset << datasetLine;
        }
        dataset.close();
        //fechando arquivo

        std::cout << "[GETINFO] line written" << std::endl;
    }else{
        std::cout << "[GETINFO] line discarded: our team hasn't ball possession" << std::endl;
    }
    playerMutex.unlock();
}

void getInfo::teamSort(playerInfo *team, int qtTeam){
    int i, j;
    bool swapped;

    //bubble sort para ordenar os jogadores segundo a distToKicker
    for(i = 0; i < qtTeam-1; i++){
        swapped = false;
        for(j = 0; j < qtTeam-1-i; j++){
            if(team[j].distToKicker > team[j+1].distToKicker){
                playerInfo playerTeam;
                playerTeam = team[j];
                team[j] = team[j+1];
                team[j+1]= playerTeam;
                swapped = true;
            }
        }
        if(!swapped) i = qtTeam;
    }
}

playerInfo getInfo::calc(playerInfo player){
    float x, y, z, pi = 3.141593;

    x = player.position.x();
    y = player.position.y();
    z = player.position.z();
    player.position.setPosition(-1*x, -1*y, z);
    player.orientation.setValue(player.orientation.value() - pi);
    if(player.orientation.value() < 0){
        player.orientation.setValue(2*pi + player.orientation.value());
    }

    return player;
}

void getInfo::fixInfo(){
    /*
     * O objetivo eh gerar um dataset com situacoes de passe e chute a gol
     * que tenham ocorrido em regioes espaciais similares
     * para facilitar o treinamento da MLP.
     * Por isso, consideramos que todas as situacoes aconteceram em jogadas
     * de ataque de times que sao sempre o time da esquerda (e atacam o time da direita).
     * (para isso, "convertemos" as coordenadas e orientacoes como se o time fosse o time da esquerda)
    */
    if(_ourTeam->fieldSide().isRight()){
        //kicker
        kicker = calc(kicker);
        //oppGoalie
        oppGoalie = calc(oppGoalie);
        if(oppGoalie.position.x() < 0){
            Position defaultPos(true, 18, 0, 0);
            oppGoalie.position = defaultPos;
        }
        //allyGoalie
        allyGoalie = calc(allyGoalie);
        if(allyGoalie.position.x() > 0){
            Position defaultPos(true, -18, 0, 0);
            oppGoalie.position = defaultPos;
        }
        //ally vector
        for(int i=0; i<qtAlly; i++){
            ally[i] = calc(ally[i]);
            if(ally[i].position.x() < 0){
                Position defaultPos(true, -18, 0, 0);
                ally[i].position = defaultPos;
            }
        }
        //opp vector
        for(int i=0; i<qtOpp; i++){
            opp[i] = calc(opp[i]);
            if(opp[i].position.x() < 0){
                Position defaultPos(true, 18, 0, 0);
                opp[i].position = defaultPos;
            }
        }
    }else{
        //se os aliados nao estiverem no lado adversario: posicao default
        for(int i=0; i<qtAlly; i++){
            if(ally[i].position.x() < 0){
                Position defaultPos(true, -18, 0, 0);
                ally[i].position = defaultPos;
            }
        }
        //se os oponentes nao estiverem na defesa (lado deles): posicao default
        for(int i=0; i<qtOpp; i++){
            if(opp[i].position.x() < 0){
                Position defaultPos(true, 18, 0, 0);
                opp[i].position = defaultPos;
            }
        }
        //se o goleiro adversario nao for identificado: posicao default
        if(oppGoalie.position.x() < 0){
            Position defaultPos(true, 18, 0, 0);
            oppGoalie.position = defaultPos;
        }
        //se o goleiro aliado nao for identificado (nao esta no nosso lado do campo): posicao default
        if(allyGoalie.position.x() > 0){
            Position defaultPos(true, -18, 0, 0);
            oppGoalie.position = defaultPos;
        }
    }

}

void getInfo::testMLP(){
    float inMlp[inLength] = {kicker.position.x(), kicker.position.y(), ally[0].position.x(), ally[0].position.y(),
                     opp[0].position.x(), opp[0].position.y(), oppGoalie.position.x(), oppGoalie.position.y(),
                     ally[1].position.x(), ally[1].position.y(), opp[1].position.x(), opp[1].position.y()};
    float* mlpDecision;
    mlpDecision = MLP::forward(inMlp);
    cout << "[GETINFO MLP] ";
    for(int i=0;i<outputLength;i++){
        cout << mlpDecision[i] << " ";
    }
    cout << endl;
}

void getInfo::initialization(){
    //alocar vetor para guardar informacoes dos jogadores aliados (exceto kicker e goleiro)
    ally = static_cast<playerInfo*>(malloc((MRCConstants::_qtPlayers - 2)*(sizeof(playerInfo))));
    if(ally == NULL){
        std::cout << "[GETINFO] Erro alocacao;" << std::endl;
    }

    //alocar vetor para guardar informacoes dos jogadores oponentes (exceto goleiro)
    opp = static_cast<playerInfo*>(malloc((MRCConstants::_qtPlayers - 1)*(sizeof(playerInfo))));
    if(opp == NULL){
        std::cout << "[GETINFO] Erro alocacao;" << std::endl;
    }

    std::cout << "[GETINFO] Entity Started\n";
}

void getInfo::loop(){
    fillInfo();
}

void getInfo::finalization(){
    free(ally);
    free(opp);
    std::cout << "[GETINFO] Entity Stopped\n";
}

