import styled from 'styled-components';

import { noSelect } from './Layout';

export const Title = styled.h1`
    font-family: ${props => props.theme.fonts.main};
    color: ${props => props.theme.colors.secondary};
    ${noSelect}
`;

export const TitleSmall = styled(Title)`
    margin: 0;
    font-size: 3.8rem;
    text-shadow: 0px 2px 2px rgba(0, 0, 0, 0.25);
`;

export const TitleBig = styled(Title)`
    margin: 1rem;
    font-size: 6rem;
    text-shadow: 0px 4px 4px rgba(0, 0, 0, 0.25);
`;


export const Text = styled.p<{ isFlipped?: boolean }>`
    margin: 0;
    font-family: ${props => props.theme.fonts.main};
    ${props => (
        props.isFlipped 
            ? `color: ${props.theme.colors.main};`
            : `color: ${props.theme.colors.secondary}; text-shadow: 0px 2px 2px rgba(0, 0, 0, 0.15);`
        )
    }
    ${noSelect}
`;

export const TextSmall = styled(Text)`
    font-size: 2rem;
`;

export const TextMedium = styled(Text)`
    font-size: 2.5rem;
`;

export const TextBig = styled(Text)`
    font-size: 3.5rem;
`;

