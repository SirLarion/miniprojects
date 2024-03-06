import { screen } from '@testing-library/react';

import { render, fireEvent, act } from '@testing-library/react';

import GameBoard from '../components/GameBoard';

import { defaultSession, defaultCards } from '../utils/constants';
import { shuffleCards } from '../utils/tools';

const testCards = shuffleCards(defaultCards, 'abc123');

describe('<GameBoard>', () => {
    //-----------------------------------------------------------------------//
    it('should match the standard snapshot when card shuffling is seeded', () => {
        const { container } = render(<GameBoard session={defaultSession} cards={testCards}/>);
        expect(container).toMatchSnapshot();
    });

    //-----------------------------------------------------------------------//
    it('should contain 24 Card components', () => {
        const { container } = render(<GameBoard session={defaultSession} cards={testCards}/>);
        const grid = container.firstChild.childNodes[1];
        expect(grid.childNodes.length).toBe(24);
    });

    //-----------------------------------------------------------------------//
    it('should handle card selection correctly', () => {
        const { container } = render(<GameBoard session={defaultSession} cards={testCards}/>);
        const grid = container.firstChild.childNodes[1];
        const card = grid.childNodes[6].firstChild;
        act(() => {
            fireEvent.click(card);
        });
        expect(container).toMatchSnapshot();
    });

    //-----------------------------------------------------------------------//
    it('should handle card revealing correctly', () => {
        const { container } = render(<GameBoard session={defaultSession} cards={testCards}/>);
        const grid = container.firstChild.childNodes[1];
        const card = grid.childNodes[22].firstChild;
        act(() => {
            fireEvent.click(card);
        });
        act(() => {
            fireEvent.click(card);
        });
        expect(container).toMatchSnapshot();
    });

    //-----------------------------------------------------------------------//
    it('should handle card deselection correctly', () => {
        const { container } = render(<GameBoard session={defaultSession} cards={testCards}/>);
        const grid = container.firstChild.childNodes[1];
        const card = grid.childNodes[8].firstChild;
        act(() => {
            fireEvent.click(card);
        });
        act(() => {
            fireEvent.click(container.firstChild);
        });
        expect(container).toMatchSnapshot();
    });

});
