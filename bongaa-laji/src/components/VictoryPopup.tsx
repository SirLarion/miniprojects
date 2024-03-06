import styled from 'styled-components';

import Modal from './Modal';

import { Button, FlexContainer, noSelect, fullscreen } from './Layout';
import { TextSmall, TextBig } from './Typography'; 

const TextContainer = styled(FlexContainer)`
    ${fullscreen}
    color: ${props => props.theme.colors.secondary};
    flex-direction: column;
    text-align: center;
    align-items: center;
    justify-content: center;
`;

const NewGame = styled(Button)`
    margin: 3rem;
`;

const Title = styled.h1`
    font-family: ${props => props.theme.fonts.main};
    font-size: 10rem;
    line-height: 10rem;
    color: inherit;
    margin: 0;
    text-shadow: 0px 5px 5px rgba(0, 0, 0, 0.40);
    ${noSelect}
`;

const Subtext = styled.h2`
    font-family: ${props => props.theme.fonts.secondary};
    font-size: 2rem;
    color: inherit;
    margin: 0;
    text-shadow: 0px 3px 3px rgba(0, 0, 0, 0.40);
    ${noSelect}
`;

interface VictoryProps {
    isFinished: boolean;
    getVictoryText: () => string[];
    reset: () => void;
}
//===================================================================/
/*
 *
 */
const VictoryPopup: React.FC<VictoryProps> = ({isFinished, getVictoryText, reset}) => {
    const victoryText = getVictoryText();
    return (
        <Modal active={isFinished} z={30}>
            <TextContainer>
                <Title>{victoryText[0]}</Title>
                <Subtext>{victoryText[1]}</Subtext>
                <NewGame onClick={reset}>
                    <TextBig isFlipped={true}>Uusi peli</TextBig>
                </NewGame>
            </TextContainer>
        </Modal>        
    );
}

//===================================================================//

export default VictoryPopup;
