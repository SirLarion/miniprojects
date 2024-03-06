import styled from 'styled-components';

import { FlexContainer } from './Layout';
import { Card, CardState } from '../types';

import { cardH, cardW } from '../utils/constants';

const H = cardH;
const W = cardW;

const Base = styled.div`
    width: ${W}rem;
    height: ${H}rem;
    position: relative;
    background-color: ${props => props.theme.colors.mainDark};
    border-radius: 1rem;
`;

interface ICard {
    selected: boolean;
    hidden: boolean;
}
const CardOuter = styled.div`
    ${(p: ICard) => (
        p.selected 
            ? `z-index: 20; 
               width: ${2*W}rem; 
               height: ${2*H}rem; 
               top: 29vh; left: 41.66vw;
               padding: 1rem !important;
               box-shadow: 0px 0px 2rem rgba(0, 0, 0, 0.20);
               `
            : `width: ${W}rem; height: ${H}rem;`
    )}
    ${(p: ICard) => p.hidden? 'display: none;' : ''}
    background-color: ${props => props.theme.colors.secondary};
    position: fixed;
    padding: 0.5rem;
    border-radius: 1rem;
    align-items: flex-start;
`;

const CardInner = styled(FlexContainer)`
    width: 100%;
    height: 100%;
    background-color: white;
    border-radius: 0.5rem;
    border: 0.125rem solid #CBCBCB;
    justify-content: center; 
    align-items: center;
`;

const CardImage = styled.img`
    width: inherit;
    border-style: none;
`;

interface SpeciesProps {
    boardIndex: number;
    handleSelect: (event: React.MouseEvent) => void;
    state: CardState;
    card: Card;
}

const SpeciesCard: React.FC<SpeciesProps> = ({boardIndex, handleSelect, state, card}) => {
    return (
        <Base>
            <CardOuter 
                onClick={handleSelect} 
                selected={state.selected}
                hidden={state.removed && !state.revealed}
            >
                <CardInner>
                    <CardImage 
                        src={`/${card.sType}/species${card.sIndex}_${state.revealed ? 'back' : 'front'}.jpg`} 
                        alt="species" 
                    />
                </CardInner>
            </CardOuter>
        </Base>
    );
}

export default SpeciesCard;
