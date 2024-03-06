import styled from 'styled-components'

import { noSelect } from './utils'

import bDefault from './resources/buttonDefault.png'
import bGuess from './resources/buttonGuess.png'
import bCorrect from './resources/buttonCorrect.png'

const Wrapper = styled.div`
    width: 50vw;
    position: relative;
    padding: 3px 0 3px 0;
    ${noSelect}
`;

const BImage = styled.img`
    width: 100%;
`;

const TextContainer = styled.div`
    width: 30vw;
    position: absolute;
    top: 1rem;
    left: 10rem;
    z-index: 69;
    font-family: 'Noto Sans';
    font-size: 2.3rem;
    color: ${p => p.color};
    ${noSelect}
`;



const Button = ({text, id, guessed, correct, handleClick}) => {
    console.log(text);

    const getBImg = () => {
        if(id === correct) 
            return bCorrect
        else if(id === guessed) 
            return bGuess
        else 
            return bDefault
    }
    return (
        <Wrapper id={id} onClick={handleClick}>
            <TextContainer color={id === guessed && id !== correct ? "#161616" : "white"}>
                {`${id}: ${text}`}
            </TextContainer>
            <BImage draggable={false} src={getBImg()} />
        </Wrapper>
    );
}

export default Button;
