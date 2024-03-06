import styled from 'styled-components'

const CapImage = styled.img`
    width: 32vw;
    &:hover {
        width: 40vw;
    }
    transition: width 0.2s;
`;

const TCap = ({handleClick}) => {
    return (
        <CapImage
            src="/t-kari.png"
            onClick={handleClick}
        />  
    );
}

export default TCap;
