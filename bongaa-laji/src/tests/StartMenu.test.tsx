import { render, screen, fireEvent, act } from '@testing-library/react';

import StartMenu from '../components/StartMenu';

import { startText, playerWords } from '../utils/locale';
import { minPlayers, maxPlayers } from '../utils/constants';

const mockStart = jest.fn();

const playersText = playerWords.plural;

describe('<StartMenu>', () => {

    //-----------------------------------------------------------------------//
    it('should contain a start button', () => {
        const { getAllByRole } = render(<StartMenu startSession={mockStart}/>);
        expect(getAllByRole('button').slice(-1)[0]).toHaveTextContent(startText);
    });

    //-----------------------------------------------------------------------//
    it('should contain a system for selecting the amount of players', () => {
        const { getByText } = render(<StartMenu startSession={mockStart} />);
        const textElement = getByText(new RegExp(`.${playersText}`));
        expect(textElement).toBeTruthy();

        const minusButton = textElement.children[0];
        const plusButton = textElement.children[1];

        expect(minusButton).toHaveTextContent('-');
        expect(plusButton).toHaveTextContent('+');
    });

    //-----------------------------------------------------------------------//
    it('should increase numPlayers and decrease it when pressing corresponding buttons', () => {
        const { getByText } = render(<StartMenu startSession={mockStart} />);
        const textElement = getByText(new RegExp(`.${playersText}`));

        const plusButton = textElement.children[1];
        const minusButton = textElement.children[0];

        const getNum = () => parseInt(textElement.textContent[1]);

        // Add one
        act(() => {
            fireEvent.click(plusButton);
        }); 
        expect(getNum()).toEqual(minPlayers+1);

        // Subtract one
        act(() => {
            fireEvent.click(minusButton);
        }); 
        expect(getNum()).toEqual(minPlayers);
    });

    //-----------------------------------------------------------------------//
    it('should not decrease numPlayers under minPlayers when the minus button is clicked', () => {
        const { getByText } = render(<StartMenu startSession={mockStart} />);

        const textElement = getByText(new RegExp(`.${playersText}`));
        const minusButton = textElement.children[0];

        act(() => {
            fireEvent.click(minusButton);
        }); 

        const minNum = parseInt(textElement.textContent[1]);
        expect(minNum).toEqual(minPlayers);
    });

    //-----------------------------------------------------------------------//
    it('should not increase numPlayers over maxPlayers when the plus button is clicked', () => {
        const { getByText } = render(
            <StartMenu
                startSession={mockStart}
                defaultNumPlayers={maxPlayers}
            />
        );
        const textElement = getByText(new RegExp(`.${playersText}`));
        const plusButton = textElement.children[1];

        act(() => {
            fireEvent.click(plusButton);
        }); 

        const maxNum = parseInt(textElement.textContent[1]);
        expect(maxNum).toEqual(maxPlayers);
    });

});

