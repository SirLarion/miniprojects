import React from 'react';
import { useState } from 'react';
import styled from 'styled-components';

import StartMenu from './components/StartMenu';
import GameBoard from './components/GameBoard';
import { fullscreen } from './components/Layout';

import { defaultSession, defaultCards } from './utils/constants';
import { shuffleCards } from './utils/tools';
import { Session, Card } from './types';

const Main = styled.div`
    ${fullscreen}
    background-color: ${props => props.theme.colors.main};
`;

const sessionCards: Card[] = shuffleCards(defaultCards);


//===================================================================/
/*
 *
 */
const BongaaApp: React.FC = () => {

    const [session, setSession] = useState(defaultSession);

    return (
        <Main>
            {!session.started
            ? <StartMenu 
                startSession={
                    (startedSession: Session) => setSession({...startedSession, started: true})
                }
              />
            : <GameBoard 
                session={session} 
                cards={sessionCards}
                resetGame={() => setSession(defaultSession)}
              />
            }
        </Main>
    );
}

//===================================================================/

export default BongaaApp;
