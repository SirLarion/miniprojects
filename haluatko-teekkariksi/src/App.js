import { useState } from 'react'
import styled from 'styled-components'

import TCap from './TCap'
import Init from './Init'

const Start = styled.div`
    width: 100vw;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    background-image: url('/haluatko_bg.png');
`;

function App() {
    const [capClicked, setClicked] = useState(false);
    if(!capClicked) {
        return (
            <Start>
                <TCap handleClick={() => setClicked(true)}/>
            </Start>
        );
    }
    else {
        return <Init />;
    }
}

export default App;
