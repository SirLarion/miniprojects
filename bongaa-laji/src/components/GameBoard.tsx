import { useState } from 'react';
import styled from 'styled-components';
import Draggable from 'react-draggable';

import SpeciesCard from './SpeciesCard';
import VictoryPopup from './VictoryPopup';
import Modal from './Modal';
import { FlexContainer, fullscreen } from './Layout';
import { TitleSmall, TextMedium } from './Typography';

import { speciesAmount, gridW, gridH } from '../utils/constants';
import { countForeignRemaining, getWinnerText } from '../utils/tools';
import { gameName } from '../utils/locale';
import { Session, Card, SpeciesType } from '../types';


const Game = styled(FlexContainer)`
    ${fullscreen}
    max-width: 100vw;
    max-height: 100vh;
    overflow: hidden;
    flex-direction: column;
    align-items: center;
    background-color: ${props => props.theme.colors.main};
`;

const HeaderWrapper = styled(FlexContainer)`
    width: 100vw;
    height: 10rem;
    justify-content: center;
    align-items: center;
`;

const Header = styled.div`
    display: grid;
    grid: auto / ${0.33*gridW}rem ${0.33*gridW}rem ${0.33*gridW}rem;
    align-items: center;
`;

const HeaderItem = styled(FlexContainer)`
    width: 100%;
    height: 100%;
    align-items: center;
`;

const Board = styled(FlexContainer)``;

const CardGrid = styled.div`
    width: ${gridW}rem;
    height: ${gridH}rem;
    display: grid;
    grid: auto auto auto / auto auto auto auto auto auto auto auto;
    grid-gap: 1rem;
`;

const GameTitle = styled(TitleSmall)`
    align-self: center;
    margin: auto;
`;

const RemainingText = styled(TextMedium)`
    font-family: ${props => props.theme.fonts.secondary};
    margin-left: auto;
    
`;

const PlayerText = styled(TextMedium)`
    font-family: ${props => props.theme.fonts.secondary};
    margin-right: auto;
`;


interface IReveal {
    isRevealed: boolean;
    isForeign: boolean;
}
const RevealText = styled.h2<IReveal>`
    ${p => !p.isRevealed ? 'display: none;' : ''}
    ${p => p.isForeign ? 'color: #E673AE;' : 'color: #528E3A;'}
    font-size: 5rem;
    font-family: ${props => props.theme.fonts.main};
    -webkit-text-stroke: 2px ${props => props.theme.colors.secondary};
    text-shadow: 0px 0px 1rem rgba(0, 0, 0, 0.30);
    z-index: 20; 
    position: absolute;
    bottom: 7rem;
    margin: 0 auto 0 auto;
`;

interface GameProps {
    session: Session;
    cards: Card[];
    resetGame: () => void;
}

//===================================================================/
/*
 *
 */
const GameBoard: React.FC<GameProps> = ({session, cards, resetGame}) => {

    const [isFinished, setFinished] = useState(false);
    
    const [playerState, setPlayerState] = useState(session.players);
    const [playerIndex, setPlayerIndex] = useState(0);

    const [selected, setSelected] = useState(-1);
    const [revealed, setRevealed] = useState(-1);
    const [removedCards, setRemovedCards] = useState((new Array(speciesAmount)).fill(false));

    const [isNextTurn, setNextTurn] = useState(false);

    const isRevealedForeign = (): boolean => {
        return revealed !== -1 && cards[revealed].sType === SpeciesType.Foreign;
    }

    // Handle card deselection (clicking outside card);
    const handleCardDeselect = (event: React.MouseEvent): void => {
        setSelected(-1); setRevealed(-1);

        if(isNextTurn) {
            if(countForeignRemaining(removedCards) === 0)
                setFinished(true);

            setPlayerIndex((playerIndex + 1) % session.numPlayers)
            setNextTurn(false);
        }
    }

    // Handle card selection events (clicking on card)
    const handleCardSelect = (event: React.MouseEvent, id: number): void => {
        // Stop propagation so that the Board element doesn't respond when 
        // clicking on a card
        event.stopPropagation();

        // Select card if nothing selected 
        if(selected === -1) setSelected(id);

        // Reveal (flip) card if the card that was clicked is selected
        else if(selected === id && revealed !== id) {

            // Revealed card is Foreign?
            if(revealed !== id && cards[id].sType === SpeciesType.Foreign) {
                let newRemoved = removedCards;
                let newPlayers = playerState;

                newRemoved[id] = true;
                newPlayers[playerIndex].cards.push(cards[id]);

                setRemovedCards(newRemoved);
                setPlayerState(newPlayers);
            }
            setRevealed(id);
            setNextTurn(true);
        }

        // Otherwise
        else handleCardDeselect(event);
    }

    return (
        <Game onClick={handleCardDeselect}>

            <VictoryPopup 
                isFinished={isFinished}
                getVictoryText={() => getWinnerText(playerState)}
                reset={resetGame}
            />
            <Modal active={selected !== -1} z={10} />

            <RevealText
                isRevealed={revealed !== -1}
                isForeign={isRevealedForeign()}
            >
                {isRevealedForeign() ? 'Oikein!' : 'Väärin!'}
            </RevealText>

            <HeaderWrapper>
                <Header>
                    <PlayerText>{session.players[playerIndex].name}</PlayerText>
                    <GameTitle>{gameName}</GameTitle>
                    <RemainingText>{`${countForeignRemaining(removedCards)} korttia jäljellä`}</RemainingText>
                </Header>
            </HeaderWrapper>

            <Board>
                <CardGrid>
                    {cards.map((c: Card, i: number) => (
                        <div key={i}>
                            <SpeciesCard
                                boardIndex={i}
                                handleSelect={(e: React.MouseEvent) => handleCardSelect(e, i)}
                                state={{
                                    selected: i === selected, 
                                    revealed: i === revealed,
                                    removed: removedCards[i]
                                }}
                                card={c}
                            />
                        </div>
                    ))}
                </CardGrid>
            </Board>

        </Game>
    );
}

//===================================================================/

export default GameBoard;
