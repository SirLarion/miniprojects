import React from 'react';
import styled from 'styled-components'

import { HeroStory } from '../HeroStory';
import { HeroInfo } from '../HeroInfo';
import { HeroType } from '../../../types';

import { FlexContainer, noSelect } from '../../Layout';


// If viewport is above 1400px (i.e. normal 1920p screen),
// change card layout
const cardResponsive = ' \
    @media (min-width: 1400px) { \
        width: initial; \
        height: inherit; \
    } \
';

// Card elements
//=================================/
const Card = styled.div`
    width: 100%;
    max-width: 100vw;
    display: flex;
    flex-direction: column;
    margin-left: auto;
    margin-right: auto;
    border-radius: 30px;
    @media (min-width: 780px) {
        width: 75%;
    }
    @media (min-width: 1400px) {
        height: 60vh;
        flex-direction: row;
    }
`;

const HeroImage = styled.img`
    width: 100%;
    ${cardResponsive}
`;

const TextContainer = styled.div`
    background: #f0f0f0;
    height: 60vh;
    ${cardResponsive}
    padding: 40px;
`;

const TextHeader = styled(FlexContainer)`
    justify-content: space-between;
	font-family: "Montserrat";
	font-weight: 800;
    font-size: 3.2vw;
    line-height: 39px;
    letter-spacing: 1.15px;
	color: #001147;
    @media (min-width: 780px) {
        font-size: 25px;
    }
    ${noSelect}
`;
//=================================/



// Nav elements
//=================================/
const Nav = styled(FlexContainer)`padding-right: 50px;`;

interface INavButton {
    active: boolean;
}

const NavButton = styled.span`
    background: none;
	border: none;
	padding: 0;
	font: inherit;
	cursor: pointer;
	outline: inherit;
    color:  ${(p: INavButton)=> p.active ? '#001147':'#7C85A0'};
    &:hover {
        color: #001147;
    }
`;

const Spacer = styled.div`
    padding: 0 8px 0 8px;
    align-items: center;
`;
//=================================/



interface IHeroCardProps {
    hero: HeroType;
}

enum Tab {
    Info,
    Story
}

//=====================================================================================//
/*
 * Container for information on one 'hero'. Displays an image of the hero along with 
 * a description, backstory and the skills and attributes of the hero.
 *
 * On a wide screen, display image and text next to each other and on a thin screen,
 * display image on top of text
 */
export const HeroCard: React.FC<IHeroCardProps> = ({hero}) => {
    // Which tab is currently selected? 
    const [tab, setTab] = React.useState(Tab.Info);
    return (
        <Card>
            <HeroImage 
                src={hero.imgUrl} 
                alt={`Image of ${hero.name}`}
                draggable={false}
            />
            <TextContainer>

                {/* Hero name and nav bar */}
                <TextHeader>
                    {hero.name}
                    <Nav>
                        <NavButton 
                            active={tab === Tab.Info}
                            onClick={() => setTab(Tab.Info)}
                        >Info</NavButton>
                        <Spacer>/</Spacer>
                        <NavButton 
                            active={tab === Tab.Story}
                            onClick={() => setTab(Tab.Story)}
                        >Story</NavButton>
                    </Nav>
                </TextHeader>

                {/* Display info or backstory depending on the active tab */
                    tab === Tab.Story 
                        ? <HeroStory 
                            story={hero.backStory}
                          />
                        : <HeroInfo 
                            description={hero.description}
                            stats={hero.attributes}
                            skills={hero.skills}
                          />
                }

            </TextContainer>
        </Card>
    );
}

//=====================================================================================//
