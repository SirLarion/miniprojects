import React from 'react';
import styled from 'styled-components'
import { OverlayTrigger } from 'react-bootstrap';

import { capitalized } from '../../../utils/tools';
import { FlexContainer } from '../../Layout';

interface SizeProps {
    isSmall: boolean;
}

// The two variants of attributes
//=================================/
const Attribute = styled(FlexContainer)`
    font-size: 0.95em;
    justify-content: center;
    ${(p: SizeProps) => !p.isSmall ? 'margin: 0.45em' : ''}
`;

const HSMattr = styled(FlexContainer)`
    line-height: 24px;
    flex-direction: column;
`;
//=================================/



const Icon = styled.img`
    ${(p: SizeProps) => p.isSmall 
        ? 'width: 23px; margin-left: 4px'
        : 'width: 26px; margin: 4px;'
    }
`;

const Tooltip = styled.div`
    font-family: "Oswald";
    letter-spacing: 1.2px;
    background: #E0E0E0AA;
    border-radius: 3px;
    padding: 4px 7px 4px 7px;
`;



interface IHeroAttrProps {
    name: string;
    value?: number;
    noOverlay?: boolean;
    small?: boolean;
}

//=====================================================================================//
/*
 * Container for a single attribute of a hero. For health, stamina and mana, it displays
 * the name of the attribute, its value and a corresponding icon. The icon is looked for 
 * in the public folder with the name '{name}.png'
 * 
 * For others it displays only the icon and the value with the name popping up as a 
 * tooltip when hovered over. Uses React Bootstrap overlays to achieve this
 *
 * Can also be used for damage types in the same way. 
 */
export const HeroAttribute: React.FC<IHeroAttrProps> = ({name, value, noOverlay, small}) => {
    // Check if noOverlay flag was set. If it was, display the name of
    // the attribute as well as the icon and value
    if(noOverlay) {
        return (
            <HSMattr>
                { capitalized(name) }
                <Attribute isSmall={small}>
                    <Icon 
                        isSmall={small}
                        src={`/public/${name}.png`} 
                        alt={`${name} icon`} 
                    />
                    {value}
                </Attribute>
            </HSMattr>
        );
    }
    else {
        return (
            <OverlayTrigger
                placement="top"
                delay={{ show: 100, hide: 100 }}
                overlay={<Tooltip>{capitalized(name)}</Tooltip>}
            >
                <Attribute isSmall={small}>
                    <Icon 
                        isSmall={small}
                        src={`/public/${name}.png`} 
                        alt={`${name} icon`} 
                    />
                    {value}
                </Attribute>
            </OverlayTrigger>
        );
    }
}
//=====================================================================================//
// HeroAttribute END
