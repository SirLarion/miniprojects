import { render, screen, fireEvent, act } from '@testing-library/react';

import SpeciesCard from '../components/SpeciesCard';
import { Card, SpeciesType } from '../types';
import * as Tools from '../utils/tools';

describe('<SpeciesCard>', () => {
    //-----------------------------------------------------------------------//
    it('should match the standard snapshot', () => {
        const testCard: Card = { sIndex: 3, sType: SpeciesType.Native }
        const { container } = render(
            <SpeciesCard  
                boardIndex={3}
                handleSelect={jest.fn()}
                state={{selected: false, revealed: false}}
                card={testCard}
            />
        );
        expect(container).toMatchSnapshot();
    });

    //-----------------------------------------------------------------------//
    it('should contain a visible image by default', () => {
        const testCard: Card = { sIndex: 3, sType: SpeciesType.Native }
        const { getByAltText } = render(
            <SpeciesCard  
                boardIndex={3}
                handleSelect={jest.fn()}
                state={{selected: false, revealed: false}}
                card={testCard}
            />
        );
        expect(getByAltText('species')).toBeVisible();
    });

    //-----------------------------------------------------------------------//
    it('should call handleSelect when clicked', () => {
        const mockClick = jest.fn();
        const testCard: Card = { sIndex: 3, sType: SpeciesType.Native }
        const { container } = render(
            <SpeciesCard  
                boardIndex={3}
                handleSelect={mockClick}
                state={{selected: false, revealed: false}}
                card={testCard}
            />
        );
        fireEvent.click(container.firstChild.firstChild);
        expect(mockClick.mock.calls.length).toBe(1);
    });


});
