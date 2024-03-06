import { useState } from 'react'
import styled from 'styled-components'

import Game from './Game'
import { noSelect } from './utils'

import dudeDefault from './resources/dudeDefault.png'
import EngFlag from './resources/eng.png'
import FinFlag from './resources/fin.jpg'

const Container = styled.div`
    width: 100vw;
    height: 100vh;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    background-color: #090528;
    color: #090528;
`;

const SubContainer = styled.div`
    height: 30vh;
    display: flex;
    flex-direction: column;
    justify-content: space-around;
    align-items: center;

`;

const NameInput = styled.input`
    width: 10em;
    margin-left: 7em;
    background: none;
	color: ghostwhite;
	border: none;
	padding: 0;
	font: inherit;
    font-size: 4rem;
	cursor: pointer;
	outline: inherit;
}
`;

const Button = styled.div`
    padding: 0.2rem 1rem 0.2rem 1rem;
    background-color: ghostwhite;
    ${noSelect}
`;

const LangButton = styled.button`
    padding: 0;
    z-index: 2;
    position: fixed;
    top: 3rem;
    right: 3rem;
    ${noSelect}
`;

const Flag = styled.img`
    width: 4rem;
`;

const Contestant = styled.img`
    height: 1000px;
    z-index: 1;
    position: fixed;
    top: 10vh;
    left: 20vw;
`;

const Init = () => {

    const [started, setStarted] = useState(false);
    const [name, setName] = useState('Boring default name :/');
    const [players, setPlayers] = useState(0);
    const [isEnglish, setEnglish] = useState(false);

    if(!started) {
        return (
            <Container>
                <Contestant src={dudeDefault} />
                <SubContainer>
                    <NameInput 
                        value={name}
                        onChange={({target}) => setName(target.value)}
                    />
                    <Button onClick={() => setStarted(true)}>Start</Button>
                </SubContainer>
                <LangButton onClick={() => setEnglish(!isEnglish)}>
                    <Flag src={isEnglish ? EngFlag : FinFlag} />
                </LangButton>
            </Container>
        );
    }
    else {
        return <Game locale={isEnglish ? 'eng' : 'fin'} name={name} />
    }
}

export default Init;
