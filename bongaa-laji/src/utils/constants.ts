
import { Session, Player, Card, SpeciesType } from '../types';
import { playerWords } from '../utils/locale';

export const cardH = 14;
export const cardW = 10;

export const minPlayers = 2;
export const maxPlayers = 4;
export const speciesAmount = 24;

const rowAmount = 3;
const colAmount = 8;

export const gridW = speciesAmount/rowAmount * cardW + speciesAmount/rowAmount -1;
export const gridH = speciesAmount/colAmount * cardH + speciesAmount/colAmount -1;

export const defaultCards: Card[] = (new Array(speciesAmount)).fill(0).map((n: number, i: number) => {
    const c: Card = {
        sIndex: (i % 12) + 1,
        sType: i < 12 ? SpeciesType.Foreign : SpeciesType.Native
    }
    return c;
});

export const defaultPlayers: Player[] = [
    {
        name: `${playerWords.singular} 1`,
        cards: []  
    },
    {
        name: `${playerWords.singular} 2`,
        cards: []  
    }
];

export const defaultSession: Session = {
    started: false,
    numPlayers: minPlayers,
    players: defaultPlayers
}


