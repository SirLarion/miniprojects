import { Entity, PrimaryGeneratedColumn, Column } from 'typeorm';
import { ObjectType, ID, Field } from 'type-graphql';
import { TElement } from '../types/element';

@Entity()
@ObjectType()
export class Attribute {
	@Field((type) => ID)
	@PrimaryGeneratedColumn()
	id!: number;

	@Field()
	@Column()
	strength: number;

	@Field()
	@Column()
	intelligence: number;

	@Field()
	@Column()
	stamina: number;

	@Field()
	@Column()
	agility: number;

	@Field()
	@Column()
	speed: number;

	@Field()
	@Column()
	resistance: TElement;

	@Field()
	@Column()
	weakness: TElement;

	@Field()
	@Column()
	healthpoints: number;

	@Field()
	@Column()
	mana: number;
}
