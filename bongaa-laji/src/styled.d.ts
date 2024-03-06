import 'styled-components';

declare module 'styled-components' {
    export interface DefaultTheme {
        colors: {
            main: string;
            mainDark: string;
            secondary: string;
            secondaryDark: string;
        };
        fonts: {
            main: string;
            secondary: string;
        };
    }
}
