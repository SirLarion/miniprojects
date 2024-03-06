import { render } from '@testing-library/react';

import BongaaApp from '../BongaaApp';

describe('<BongaaApp>', () => {
    it('should contain the right amount of children', () => {
        const { container } = render(<BongaaApp />);
        const correctAmount = 1;
        expect(container.firstChild.children.length).toBe(correctAmount);
    });
});
