import seedrandom from 'seedrandom';

import { Card, Player } from '../types'
import { cardWords } from '../utils/locale';

// Shuffle the given array of Card elements randomly or with a
// seed if one is given
export const shuffleCards = (a: Card[], seed?: string) => {
    var j, x, i;
    var b = a.slice();
    const s = seed ? seed : `${Math.random()}`;
    seedrandom(s, { global: true });

    for (i = b.length - 1; i > 0; i--) {
        j = Math.floor(Math.random() * (i + 1));
        x = b[i];
        b[i] = b[j];
        b[j] = x;
    }
    return b;
}

// Count the amount of true values in an array of booleans 
// where each value corresponds to a card with a foreign species
// that has been removed from the board
export const countForeignRemaining = (a: boolean[]): number => {
    var count = 0;
    for(var i = 0; i < a.length; ++i){
        if(a[i])
            count++;
    }
    return a.length/2 - count;
}

// Filter the players with the most cards from the list of players
// given as a parameter
const getWinners = (players: Player[]): Player[] => {

    let winners: Player[] = [];
    const playerCards = players.map(p => p.cards.length);
    const max = Math.max(...playerCards);

    for(var i = 0; i < players.length; i++){
        if(playerCards[i] === max)
            winners.push(players[i]);
    }
    return winners;
}

export const getWinnerText = (players: Player[]): string[] => {
    const numPlayers = players.length;
    const winners = getWinners(players);
    const winnerNames = winners.map(w => w.name);
    const maxCards = winners[0].cards.length;

    let text = '';
    let subtext = '';

    if(winners.length === 1) {
        text = `${winners[0].name} voittaa!`;
        subtext = `${maxCards} ${maxCards === 1 ? cardWords.singular : cardWords.plural}`;
    }
    else if(winners.length > 1) {
        if(winners.length === numPlayers)
            text = 'Tasapeli!';
        else
            text = `${winnerNames.slice(0, -1).join(', ')} ja ${winnerNames[winners.length -1]} voittavat!`;

        subtext = `${maxCards} ${maxCards === 1 ? cardWords.singular : cardWords.plural}`;
    }
    return [text, subtext];
}
