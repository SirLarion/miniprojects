import styled from 'styled-components';

// Disable selection
export const noSelect = '\
    -webkit-touch-callout:none; \
    -webkit-user-select:none; \
    -moz-user-select:none; \
    -ms-user-select:none; \
    user-select:none; \
';

// Full width and height
export const fullscreen = 'width: 100vw; height: 100vh;';


// Line elements
//=================================/
interface IVLine {
    height: string;
    color: string;
}
export const VLine = styled.div`
    width: 0;
    height: ${(p: IVLine) => p.height};
    border: 1px solid ${(p: IVLine) => p.color};
`;
//-----------------//

interface IHLine {
    width: string;
    color: string;
}
export const HLine = styled.div`
    height: 0;
    width: ${(p: IHLine) => p.width};
    border: 1px solid ${(p: IHLine) => p.color};
`;
//-----------------//


// Containers
//=================================/
export const FlexContainer = styled.div`
    display: flex;
`;
//-----------------//


// Functionals
//=================================/
export const Button = styled(FlexContainer)<{ isFramed?: boolean }>`
    ${props => props.isFramed 
        ? (`
            background-color: ${props.theme.colors.main} !important;
            &:hover {
                background-color: ${props.theme.colors.mainDark} !important;
                border: 5px solid ${props.theme.colors.secondaryDark} !important;
            }
            border: 5px solid ${props.theme.colors.secondary} !important;
        `)
        : ''     
    }
    background-color: ${props => props.theme.colors.secondary};
    &:hover {
        background-color: ${props => props.theme.colors.secondaryDark};
    }
    border: none;
    margin: 1rem;
    padding: 0.66rem 1.5rem 0.66rem 1.5rem;
    border-radius: 0.66rem;
    align-items: center;
    justify-content: center;
    box-shadow: 0px 2px 1px rgba(0, 0, 0, 0.20);
`;
//-----------------//
