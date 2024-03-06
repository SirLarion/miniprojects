import React from 'react';
import styled from 'styled-components';

// Line elements
//=================================/
interface IVLine {
    height: string;
}
export const VLine = styled.div`
    width: 0;
    height: ${(p: IVLine) => p.height};
    border: 1px solid #001147;
`;

interface IHLine {
    width: string;
}
export const HLine = styled.div`
    height: 0;
    width: ${(p: IHLine) => p.width};
    border: 1px solid #001147;
`;
//=================================/


// Containers
//=================================/
interface IPadded {
    top?: string;
    right?: string;
    bottom?: string;
    left?: string;
}
export const PaddedContainer = styled.div`
    padding-top: ${(p: IPadded) => p.top};
    padding-right: ${(p: IPadded) => p.right};
    padding-bottom: ${(p: IPadded) => p.bottom};
    padding-left: ${(p: IPadded) => p.left};
`;

export const FlexContainer = styled.div`
    display: flex;
    align-items: center;
`;

interface IContainer {
    width?: string;
    height?: string;
}
export const Container = styled.div`
    width: ${(p: IContainer) => p.width};
    height: ${(p: IContainer) => p.height};
`;
//=================================/


// Disable selection
export const noSelect = '\
    -webkit-touch-callout:none; \
    -webkit-user-select:none; \
    -moz-user-select:none; \
    -ms-user-select:none; \
    user-select:none; \
';
