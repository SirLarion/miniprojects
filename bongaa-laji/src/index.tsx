import React from 'react';
import ReactDOM from 'react-dom';
import { DefaultTheme, ThemeProvider, createGlobalStyle } from 'styled-components';

import BongaaApp from './BongaaApp';

const GlobalStyle = createGlobalStyle`
    body {
        margin: 0;
        font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Oxygen',
            'Ubuntu', 'Cantarell', 'Fira Sans', 'Droid Sans', 'Helvetica Neue',
            sans-serif;
        -webkit-font-smoothing: antialiased;
        -moz-osx-font-smoothing: grayscale;
        font-size: 14px;
        font-variant: tabular-nums;
        line-height: 1.5715;
        font-feature-settings: 'tnum', "tnum";

    }
    *, ::before, ::after {
        box-sizing: border-box;
    }
`;

const theme: DefaultTheme = {
    colors: {
        main: '#84C37D',
        mainDark: '#75B36E',
        secondary: '#FEFEF2',
        secondaryDark: '#F3F3DA'
    },
    fonts: {
        main: 'Staatliches',
        secondary: 'Lilita One'
    }
}

ReactDOM.render(
    <React.StrictMode>
        <GlobalStyle />
        <ThemeProvider theme={theme}>
            <BongaaApp />
        </ThemeProvider>
    </React.StrictMode>,
    document.getElementById('root')
);
