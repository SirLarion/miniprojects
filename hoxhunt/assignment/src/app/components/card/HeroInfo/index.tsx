import React from 'react';
import styled from 'styled-components'

import { HeroAttribute } from '../HeroAttribute';
import { Paragraph } from '../../Typography';
import { Stats, Skill } from '../../../types';

import { 
    VLine, HLine, 
    Container,
    PaddedContainer, 
    FlexContainer,
    noSelect
} from '../../Layout';


// Top level containers
//=================================/
const DescriptionContainer = styled(Container)`
    font-size: 2.05vw;
    line-height: 4.1vw;
    @media (min-width: 780px) {
        font-size: 16px;
        line-height: 32px;
    }

`;

const StatsContainer = styled.div`
    height: 54%;
    font-family: "Oswald";
    letter-spacing: 1.2px;
    ${noSelect}
    font-size: 2.9vw;
    @media (min-width: 780px) {
        font-size: 22px;
    }
`;

const PaddedFlex = styled(FlexContainer)`
    height: 65%;
    padding: 15px 0 15px 0;
`;
//=================================/

// Attributes
//=================================/
const Attributes = styled.div`
    width: 60%;
    padding-right: 15px;
    margin-bottom: auto;
`;

const AttributeRow = styled.div`
    width: 100%;
    height: initial;
    display: flex;
    justify-content: space-around;
    margin-bottom: auto;
`;
//=================================/

// Skills
//=================================/
const SkillsBody = styled.div`
    height: calc(100% - 37px);
    display: flex;
    flex-direction: column;
    padding-left: 22px;
    margin-bottom: auto;
    overflow: scroll;
`;

const SkillsHeader = styled.div`
    line-height: 24px;
    letter-spacing: 1.15px;
    padding-bottom: 13px;
`;

const SkillContainer = styled(FlexContainer)`
    font-size: 0.74em;
    padding-bottom: 20px;
`;
const Damage = styled(FlexContainer)`margin-left: 10px;`;
//=================================/


interface IHeroInfo {
    description: string;
    stats: Stats;
    skills: Skill[];
}

//=====================================================================================//
/*
 * Container for hero description, attributes and skills. Displays attributes and skills
 * in a neat 'grid' with icons showing attribute and damage types.
 */
export const HeroInfo: React.FC<IHeroInfo> = ({description, stats, skills}) => {
    return (
        <Container width="100%" height="100%">

            {/* Hero description, top half */}
            <DescriptionContainer height="35%">
                <PaddedContainer top="25px">
                    <Paragraph>{description}</Paragraph>
                </PaddedContainer>
            </DescriptionContainer>

            {/* Bottom half, attributes and skills */}
            <StatsContainer>
                <HLine width="100%" />
                <PaddedFlex>

                    <Attributes>
                        {/* "HSM" aka. Health, stamina and mana */}
                        <AttributeRow>
                            <HeroAttribute noOverlay name="health" value={stats.healthpoints}/>
                            <HeroAttribute noOverlay name="stamina" value={stats.stamina}/>
                            <HeroAttribute noOverlay name="mana"  value={stats.mana}/>
                        </AttributeRow>

                        {/* Other attributes */}
                        <AttributeRow>
                            <HeroAttribute name="strength" value={stats.strength}/>
                            <HeroAttribute name="intelligence" value={stats.intelligence}/>
                            <HeroAttribute name="agility" value={stats.agility}/>
                            <HeroAttribute name="speed" value={stats.speed}/>
                        </AttributeRow>
                    </Attributes>

                    <VLine height="100%" />

                    {/* Skills */}
                    <Container width="40%" height="100%">
                        <PaddedContainer left="22px">
                            <SkillsHeader>Skills</SkillsHeader>
                        </PaddedContainer>
                        <SkillsBody>
                            { skills.map(s => (
                                <SkillContainer key={s.name}>
                                    {`${s.name}:`}
                                    <Damage>
                                        {s.damage}
                                        <HeroAttribute name={s.element.toLowerCase()} small />
                                    </Damage>
                                </SkillContainer>
                            ))}
                        </SkillsBody>
                    </Container>

                </PaddedFlex>

                <HLine width="100%" />
                
                {/* Weakness and resistance */}
                <FlexContainer>
                    <FlexContainer>
                        Weakness:
                        <HeroAttribute name={stats.weakness.toLowerCase()} />
                    </FlexContainer>

                    <FlexContainer>
                        Resistance: 
                        <HeroAttribute name={stats.resistance.toLowerCase()} />
                    </FlexContainer>
                </FlexContainer>

            </StatsContainer>
        </Container>
    );
}
//=====================================================================================//
// HeroInfo END
