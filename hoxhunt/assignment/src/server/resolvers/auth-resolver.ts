import { Resolver, Query, ObjectType, Field, Arg } from 'type-graphql';
import { Service } from 'typedi';
import { AuthService } from '../services/auth-service';

@ObjectType()
class AuthToken {
	@Field()
	jwt: string;
}

@Service()
@Resolver((of) => AuthToken)
export class AuthTokenResolver {
	constructor() {}

	@Query((returns) => AuthToken)
	authenticate(@Arg('userId') userId: string): AuthToken {
		return { jwt: AuthService().generateJwtForUserId(userId) };
	}
}
