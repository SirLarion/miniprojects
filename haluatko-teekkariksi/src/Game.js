import { useState } from 'react'
import styled from 'styled-components'

import Button from './Button'
import { noSelect } from './utils'

import dudeDefault from './resources/dudeDefault.png'
import dudeCorrect from './resources/dudeCorrect.png'
import dudeWrong from './resources/dudeWrong.png'

import qImg from './resources/question.png'
import questionsJSON from './resources/kys.json'

const Container = styled.div`
    width: 100vw;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    background-color: #090528;
`;

const QNAcontainer = styled.div`
    width: 100vw;
    position: fixed;
    bottom: 0;
    left: 0;
    z-index: 2;
`;

const Contestant = styled.img`
    height: 1000px;
    ${noSelect}
`;

const QText = styled.div`
    position: absolute;
    top: 2rem;
    left: 10rem;
    z-index: 69;
    font-family: 'Noto Sans';
    font-size: 3rem;
    color: white;
    ${noSelect}
`;

const QImage = styled.img`
    width: 100%;
    padding: 10px 0 10px 0;
    ${noSelect}
`;

const ButtonGrid = styled.div`
    display: grid;
    grid: auto / auto auto;
`;

const Game = ({locale, name}) => {

    const questions = questionsJSON[locale];

    const emptyQuestion = {
        Q: '',
        A: '',
        B: '',
        C: '',
        D: '',
        correct: 'NONE',
        index: 0
    }

    const bLetters = ['A', 'B', 'C', 'D'];
    const [guessed, setGuessed] = useState('');
    const [correct, setCorrect] = useState('');
    const [wasCorrect, setWasCorrect] = useState(undefined);

    const [question, setQuestion] = useState(emptyQuestion);

    const isCorrectGuess = () => {
        if(guessed !== '' && correct !== '') {
            return guessed === correct ? 1 : -1;
        }
        return 0;
    }

    const isQuestionInitialized = () => {
        return question['Q'] !== '' 
            && question['A'] !== ''
            && question['B'] !== ''
            && question['C'] !== ''
            && question['D'] !== ''
            && question.corerct !== 'NONE'
    }

    const handleButtonClick = event => {
        const source = event.target.parentElement.id;
        if(isQuestionInitialized()) {
            if(guessed === source) {
                setCorrect(question.correct);
                setWasCorrect(guessed === question.correct);
            }
            setGuessed(source);
        }
    }

    const handleContainerClick = event => {
        const i = question.index;
        if(correct !== ''){
            setQuestion({...emptyQuestion, index: i+1});
            setGuessed('');
            setCorrect('');
            setWasCorrect(undefined);
            return;
        }
        else if(question['Q'] === ''){
            setQuestion({...question, Q: questions[i]['Q']});
            return;
        }
        else if(question['A'] === ''){
            setQuestion({...question, A: questions[i]['A']});
            return;
        }
        else if(question['B'] === ''){
            setQuestion({...question, B: questions[i]['B']});
            return;
        }
        else if(question['C'] === ''){
            setQuestion({...question, C: questions[i]['C']});
            return;
        }
        else if(question['D'] === ''){
            setQuestion({...question, D: questions[i]['D'], correct: questions[i].correct});
            return;
        }
    }

    return (
        <Container onClick={handleContainerClick}>
            <Contestant 
                src={wasCorrect === true ? dudeCorrect : (wasCorrect === false ? dudeWrong : dudeDefault)} 
            />
            <QNAcontainer>
                <QText>
                    {question['Q']}
                </QText>
                <QImage src={qImg} />
                <ButtonGrid>
                    {bLetters.map(letter => (
                        <div key={letter}>
                            <Button 
                                text={question[letter]} 
                                id={letter} 
                                guessed={guessed} 
                                correct={correct}
                                handleClick={handleButtonClick} 
                            />
                        </div>
                    ))}
                </ButtonGrid>
            </QNAcontainer>
        </Container>
    );
}

export default Game;
