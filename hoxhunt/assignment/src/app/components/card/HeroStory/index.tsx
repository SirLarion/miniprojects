import React from 'react';
import styled from 'styled-components'

import { Paragraph } from '../../Typography';

const lineHeight = 3.5;

const StoryContainer = styled.div`
    font-size: 3vw;
    padding: 3.2vw 3.2vw 0 3.2vw;
    @media (min-width: 780px) {
        font-size: 16px;
        line-height: 2em;
        padding: 25px 25px 0 25px;
    }
`;

// Styling for the actual paragraph containing the hero backstory.
// First letter is large with a different styling for that storybook feel
const StoryParagraph = styled.p`
    color: #191919;
    font-weight: 400;
    letter-spacing: 0.65px;
    font-family: "Montserrat";
    margin: 0;
    ::first-letter {
        float: left;
        font-family: "Merriweather";
        font-weight: 200;
        padding-top: 10px;
        margin-right: 6px;
        line-height: 4em;
        font-size: 4em;
    }
`;

interface IHeroStoryProps {
    story: string;
}

//=====================================================================================//
/*
 * Container for the backstory of a hero displayed in a storybook-esque style
 */
export const HeroStory: React.FC<IHeroStoryProps> = ({story}) => {
    const trimmed = story.split('\n\n').map((s: string) => s.trim()).join(' ');
    return (
        <StoryContainer>
            <StoryParagraph>{trimmed}</StoryParagraph>
        </StoryContainer>
    );
};
//=====================================================================================//
