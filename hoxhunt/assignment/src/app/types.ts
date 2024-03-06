export type Stats = {
    agility: number;
    healthpoints: number;
    intelligence: number;
    mana: number;
    resistance: string;
    speed: number;
    stamina: number;
    strength: number;
    weakness: string;
}

export type Skill = {
    name: string;
    element: string;
    damage: number;
}

export type HeroType = {
    name: string;
    imgUrl: string;
    description: string;
    backStory: string;
    attributes: Stats;
    skills: Skill[];
}
