import styled from 'styled-components';
import { useState } from 'react';

import { startText, playerWords, gameName } from '../utils/locale';
import { 
    minPlayers, 
    maxPlayers,
    defaultSession
} from '../utils/constants';

import { Button, FlexContainer, fullscreen, noSelect } from './Layout';
import { TitleBig, TextSmall, TextMedium } from './Typography';

import { Session, Player } from '../types';

import Plus from '../resources/plus.svg';
import Minus from '../resources/minus.svg';

const FlexColumn = styled(FlexContainer)`
    ${fullscreen} 
    flex-direction: column;
    justify-content: center;
    align-items: center;
`;

const FlexRow = styled(FlexContainer)`
    justify-content: space-around;
    align-items: center;
`;

const PlayerDiv = styled.div`
    padding: 1rem; 
`;


const OpButton = styled(Button)`
    padding: 0.66rem;
    width: 3.5rem;
    height: 3.5rem;
    border-radius: 0.33rem;
`;

const ButtonImg = styled.img`
    width: 90%;
    height: 90%;
    ${noSelect}
`;


interface StartProps {
    startSession: (sess: Session) => void;
    defaultNumPlayers?: number | undefined;
}
//===================================================================/
/*
 *
 */
const StartMenu: React.FC<StartProps> = ({startSession, defaultNumPlayers}) => {

    const initNum = defaultNumPlayers ? defaultNumPlayers : defaultSession.numPlayers;
    const [session, setSession] = useState({...defaultSession, numPlayers: initNum});

    // Handler for increasing the amount of players if the plus button 
    // was clicked or decreasing it if the minus button was clicked
    const handleNumChange = (event: React.MouseEvent): void => {
        event.preventDefault();
        console.log(event.currentTarget);
        const sign = event.currentTarget.id;
        if(sign) {
            const delta = sign === '+' ? 1 : -1;
            const newNum = session.numPlayers + delta;
            const withinLimits = newNum >= minPlayers && newNum <= maxPlayers;
            
            // Check that the change would not make the amount of players too 
            // big or small
            if(withinLimits) {
                const newSession: Session = {
                    started: false,
                    numPlayers: newNum,
                    players: session.players
                };

                if(delta > 0) {
                    const newPlayer: Player = {
                        name: `${playerWords.singular} ${newNum}`,
                        cards: []
                    }
                    newSession.players.push(newPlayer);
                }
                else newSession.players.pop();

                setSession(newSession);
            }
        }
    }

    //--- RENDER ---//
    return (
        <FlexColumn>
            <TitleBig>{gameName}</TitleBig>
            <FlexRow>
                <OpButton
                    id="-"
                    onClick={handleNumChange}
                >
                    <ButtonImg src={Minus} />
                </OpButton>
                    <TextSmall>{`${session.numPlayers} ${playerWords.plural}`}</TextSmall>
                <OpButton
                    id="+"
                    onClick={handleNumChange}
                >
                    <ButtonImg src={Plus} />
                </OpButton>
            </FlexRow>
            <FlexRow>
                { 
                    session.players.map((player: Player, i: number) => {
                        return (
                            <PlayerDiv>
                                <TextSmall>{player.name}</TextSmall>
                            </PlayerDiv>
                        );
                    })
                }
            </FlexRow>
            <Button
                onClick={() => startSession(session)}
                isFramed={true}
            >
                <TextSmall>{startText}</TextSmall>
            </Button>
        </FlexColumn>
    );
}

//===================================================================/

export default StartMenu;
