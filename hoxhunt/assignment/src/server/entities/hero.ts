import { Entity, PrimaryGeneratedColumn, Column, ManyToMany, JoinTable, JoinColumn, OneToOne } from 'typeorm';
import { ObjectType, ID, Field } from 'type-graphql';
import { Lazy } from '../helpers';
import { Skill } from '../entities/skill';
import { Attribute } from './attribute';

@Entity()
@ObjectType()
export class Hero {
	@Field((type) => ID)
	@PrimaryGeneratedColumn()
	id!: number;

	@Field()
	@Column()
	name: string;

	@Field()
	@Column()
	imgUrl: string;

	@Field()
	@Column()
	description: string;

	@Field()
	@Column()
	backStory: string;

	@Field((type) => [Skill])
	@ManyToMany((type) => Skill, { lazy: true, cascade: ['insert'] })
	@JoinTable()
	skills: Lazy<Skill[]>;

	@Field((type) => Attribute)
	@OneToOne((type) => Attribute, { lazy: true, cascade: ['insert'] })
	@JoinColumn()
	attributes: Lazy<Attribute>;
}
