/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "behaviour_attacker.h"

#include <utils/freeangles/freeangles.h>

#define KICK_POWER 8.0f
#define RECEIVER_INVALID_ID 200
#define MAX_DIST_KICK 2.5f

QString Behaviour_Attacker::name() {
    return "Behaviour_Attacker";
}

Behaviour_Attacker::Behaviour_Attacker() {
}

void Behaviour_Attacker::configure() {
    usesSkill(_sk_kick = new Skill_Kick());
    usesSkill(_sk_goto = new Skill_GoToLookTo());
    usesSkill(_sk_push = new Skill_PushBall());

    addTransition(0, _sk_goto, _sk_kick);
    addTransition(1, _sk_kick, _sk_goto);

    addTransition(2, _sk_goto, _sk_push);
    addTransition(2, _sk_kick, _sk_push);
    addTransition(3, _sk_push, _sk_kick);

    setInitialSkill(_sk_goto);

    _state = STATE_PUSH;


    _timer = new MRCTimer(1000.0f);
};

void Behaviour_Attacker::run() {
    if(player()->canKickBall() == false){ // if can't kick ball
        //_state = STATE_WAIT;
    }

    switch(_state){
    case STATE_PUSH:{
        enableTransition(2);
        _bestReceiver = RECEIVER_INVALID_ID;
        _kickPosition = loc()->ourGoal();

        //Position bestKickPosition = getBestKickPosition(); //original

        //mudanca p/ testemlp
        Position oppGoal;
        oppGoal.setPosition(-5.9, -0.2, 0);
        Position bestKickPosition = player()->position();
        //fim mudanca p/ testemlp

        if(!bestKickPosition.isUnknown()){ // checar isso dps

            //mudanca p/ testemlp
            _kickPosition = oppGoal;
            cout << "[PUSH] FOUND BEST KICK POSITION" << endl;
            //fim mudanca p/ testemlp

            //_kickPosition = bestKickPosition; //original
        }else{
            // descobrindo o melhor receiver
            _mutex.lock();

            _bestReceiver = getBestReceiver();
            if(_bestReceiver != RECEIVER_INVALID_ID){

                //mudanca p/ testemlp
                cout << "[PUSH] FOUND BEST RECEIVER" << endl;
                //fim mudanca p/ testemlp

                Position receiverPosition = PlayerBus::ourPlayer(_bestReceiver)->position();
                if(!receiverPosition.isUnknown()){
                    _kickPosition = receiverPosition;
                }
            //mudanca p/ testemlp
            }else{
                cout << "[PUSH] DIDN'T FIND BEST RECEIVER" << endl;
            }
            //fim mudanca p/ testemlp

            _mutex.unlock();
        }
        // Setting push position
        _sk_push->setDestination(_kickPosition);

        // Emitting signal for receivers
        Position behindBall = WR::Utils::threePoints(loc()->ball(), _kickPosition, 0.2, GEARSystem::Angle::pi);
        if(_bestReceiver != RECEIVER_INVALID_ID && player()->distBall() < 0.3f && isBehindBall(behindBall) == false){
            emit goingToShoot(_bestReceiver);
        }

        // Change state to kick

        // check if ball is in front
        Angle anglePlayerBall = player()->angleTo(loc()->ball());
        float diff = WR::Utils::angleDiff(anglePlayerBall, player()->orientation());
        bool ans = (diff <= atan(0.7)); // atan(0.7) aprox = 35 degree

        float distToKickPosition = player()->distanceTo(_kickPosition);
        if(distToKickPosition < MAX_DIST_KICK && player()->distBall() < 0.3f && ans){
            _timer->update(); // reset timer

            //mudanca p/ testemlp
            cout << "[PUSH] GOING TO KICK!!!!!!" << endl;
            //fim mudanca p/ testemlp

            _state = STATE_KICK;

        //mudanca p/ testemlp
        }else{            
            cout << "[PUSH] CANT KICK!" << endl;
        }
        //fim mudanca p/ testemlp
    }
    break;
    case STATE_KICK:{
        //finding mlp input
        _mutex.lock();
        getNeededPlayers();
        // mlp calling
        float *mlpDecision = MLP::forward(neededPlayers);

        cout << "[MLP] " << mlpDecision[0] << " " << mlpDecision[1] << endl;

        enableTransition(2);
        if(_bestReceiver != RECEIVER_INVALID_ID){
            if(PlayerBus::ourPlayerAvailable(_bestReceiver) && mlpDecision[0] == 0.0 && mlpDecision[1] == 1.0){
                _kickPosition = PlayerBus::ourPlayer(_bestReceiver)->position();// update to avoid delay
            }
        }
        _mutex.unlock();

        // Enable kick
        if(player()->isLookingTo(_kickPosition, 0.015)){
            _sk_kick->setAim(_kickPosition);
            _sk_kick->setIsPass(false);
            enableTransition(3);
        }

        // check if ball is in front
        Angle anglePlayerBall = player()->angleTo(loc()->ball());
        float diff = WR::Utils::angleDiff(anglePlayerBall, player()->orientation());
        bool ans = (diff <= atan(0.7)); // atan(0.7) aprox = 35 degree

        if(!ans || player()->distBall() > 0.4f){
            // emit ball kicked
            if(_bestReceiver != RECEIVER_INVALID_ID){
                emit shooted(_bestReceiver);
            }

            _state = STATE_PUSH;
        }

        if(_timer->getTimeInSeconds() >= 0.2f){
            _state = STATE_PUSH;
        }
    }
    break;
    }
}

quint8 Behaviour_Attacker::getBestReceiver(){
    quint8 bestRcv = RECEIVER_INVALID_ID;
    double dist = INFINITY;
    for(int x = 0; x < _recvs.size(); x++){
        if(PlayerBus::ourPlayerAvailable(_recvs.at(x))){
            double distToAtk = sqrt(pow(player()->position().x() - PlayerBus::ourPlayer(_recvs.at(x))->position().x(), 2) + pow(player()->position().y() - PlayerBus::ourPlayer(_recvs.at(x))->position().y(), 2));
            if(distToAtk < dist){
                dist = distToAtk;
                bestRcv = _recvs.at(x);
            }
        }
    }

    return bestRcv;
}

bool Behaviour_Attacker::isBehindBall(Position posObjective){
    Position posBall = loc()->ball();
    Position posPlayer = player()->position();
    float anglePlayer = WR::Utils::getAngle(posBall, posPlayer);
    float angleDest = WR::Utils::getAngle(posBall, posObjective);
    float diff = WR::Utils::angleDiff(anglePlayer, angleDest);

    return (diff>GEARSystem::Angle::pi/2.0f);
}

Position Behaviour_Attacker::getBestKickPosition(){
    const Position goalRightPost = loc()->ourGoalRightPost();
    const Position goalLeftPost = loc()->ourGoalLeftPost();
    const Position goalCenter = loc()->ourGoal();

    // calculating angles
    float minAngle = WR::Utils::getAngle(loc()->ball(), goalRightPost);
    float maxAngle = WR::Utils::getAngle(loc()->ball(), goalLeftPost);

    // generating list of freeAngles to goal
    QList<FreeAngles::Interval> freeAngles = FreeAngles::getFreeAngles(loc()->ball(), minAngle, maxAngle);

    float largestAngle, largestMid;
    // get the largest interval
    if(freeAngles.size() == 0){
        return Position(false, 0.0, 0.0, 0.0); // debugar isso dps
    }else{
        QList<FreeAngles::Interval>::iterator it;
        for(it = freeAngles.begin(); it != freeAngles.end(); it++){
            if(it->obstructed()) continue;
            float initAngle = it->angInitial();
            float endAngle = it->angFinal();
            WR::Utils::angleLimitZeroTwoPi(&initAngle);
            WR::Utils::angleLimitZeroTwoPi(&endAngle);

            float dif = endAngle - initAngle;
            WR::Utils::angleLimitZeroTwoPi(&dif);
            if(dif > largestAngle){
                largestAngle = dif;
                largestMid = endAngle - dif/2;
            }
        }
    }

    // Triangularization
    float x = goalCenter.x() - loc()->ball().x();
    float tg = tan(largestMid);
    float y = tg * x;

    // Impact point
    float pos_y = loc()->ball().y() + y;
    Position impactPosition(true, goalCenter.x(), pos_y, 0.0);

    // Check if impact position has space for ball radius
    const float distImpactPos = WR::Utils::distance(loc()->ball(), impactPosition);
    const float radiusAngle = largestAngle/2.0;
    const float distR = radiusAngle * distImpactPos;

    if(distR < (1.5 * 0.025)){ // 1.5 * raioDaBola (ruido ft. tristeza)
        return Position(false, 0.0, 0.0, 0.0); // bola n passa, debugar isso dps
    }

    return impactPosition;
}

void Behaviour_Attacker::getNeededPlayers(){
    float minorDist, dist;

    PlayerAccess *kicker, *ally, *op, *opGoalie;
    //ourTeam = player()->team();
    //theirTeam = player()->team()->opTeam();

    //QHash<quint8, Player *> allyPlayersList = ourTeam->avPlayers();
    //QHash<quint8, Player *> opPlayersList = ourTeam->avPlayers();

    //pegando o jogador aliado com a posse de bola
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::ourPlayerAvailable(id)){  //verifica se o id é valido
            if(PlayerBus::ourPlayer(id)->hasBallPossession()){
                kicker = PlayerBus::ourPlayer(id);
            }
        }
    }

    minorDist = 9999.0;
    //aliado mais perto da bola
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::ourPlayerAvailable(id)){
            dist = PlayerBus::ourPlayer(id)->distanceTo(kicker->position());
            if(dist < minorDist && PlayerBus::ourPlayer(id)->playerId() != kicker->playerId()){
                ally = PlayerBus::ourPlayer(id);
                minorDist = dist;
            }
        }
    }
    minorDist = 9999.0;
    //pegando o goleiro adversario
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::theirPlayerAvailable(id)){
            dist = PlayerBus::theirPlayer(id)->distOurGoal();
            if(dist < minorDist){
                opGoalie = PlayerBus::theirPlayer(id);
                minorDist = dist;
            }
        }
    }

    minorDist = 9999.0;
    //adversario mais perto da bola
    for(quint8 id = 0; id < MRCConstants::_qtPlayers; id++){
        if(PlayerBus::theirPlayerAvailable(id) && id != opGoalie->playerId()){
            dist = PlayerBus::theirPlayer(id)->distanceTo(kicker->position());
            if(dist < minorDist){
                op = PlayerBus::theirPlayer(id);
                minorDist = dist;
            }
        }
    }

    /*alocando array de dados dos 4 jogadores para a mlp
     *ordem dos jogadores: kicker, ally, op, opGoalie
     *ordem dos dados: pos.x, pos.y */

    neededPlayers[0] = kicker->position().x();
    neededPlayers[1] = kicker->position().y();

    neededPlayers[2] = ally->position().x();
    neededPlayers[3] = ally->position().y();

    neededPlayers[4] = op->position().x();
    neededPlayers[5] = op->position().y();

    neededPlayers[6] = opGoalie->position().x();
    neededPlayers[7] = opGoalie->position().y();
}
