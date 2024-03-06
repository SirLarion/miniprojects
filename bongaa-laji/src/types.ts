export enum MenuState {
    Initial,
    PlayerSelection
}

export enum SpeciesType {
    Foreign = "foreign",
    Native = "native"
}

export type WordSet = {
    singular: string;
    plural: string;
}

export type Card = {
    sIndex: number;
    sType: SpeciesType;
}

export type CardState = {
    selected: boolean;
    revealed: boolean;
    removed: boolean;
}

export type Player = {
    name: string;
    cards: Card[];
}

export type Session = {
    started: boolean;
    numPlayers: number;
    players: Player[];
}
