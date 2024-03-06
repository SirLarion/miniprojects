import { useState } from 'react';
import styled from 'styled-components';

import { fullscreen } from './Layout';


const animationTime = 300

interface IStyledModal {
    active: {
        real: boolean;
        cached: boolean;
    };
    animating: boolean;
    opaque: boolean;
    z: number;
}
export const ModalContainer = styled.div<IStyledModal>`
    ${fullscreen}
    display: ${props => props.active.real || props.active.cached ? 'flex' : 'none'};
    opacity: ${props => (props.animating && props.active.real) || props.opaque ? 100 : 0};
    background-color: #00000066;
    z-index: ${props => props.z};
    position: absolute;
    top: 0;
    left: 0;
    transition: all ${animationTime}ms;
`;

interface IModal {
    active: boolean;
    z: number;
}

//===================================================================/
/*
 *
 */
const Modal: React.FC<IModal> = ({active, z, children}) => {
    const [animating, setAnimating] = useState(false);
    const [cacheActive, setCacheActive] = useState(false);
    
    if(active !== cacheActive) {
        setTimeout(() => setAnimating(true), 1);
        setTimeout(() => {
            setCacheActive(active);
            setAnimating(false);
        }, animationTime);
    }

    return ( 
        <ModalContainer
            active={{real: active, cached: cacheActive}}
            animating={animating}
            opaque={active && (active === cacheActive)}
            z={z}
        > 
            { children }
        </ModalContainer>
    ); 
}

//===================================================================/

export default Modal;
