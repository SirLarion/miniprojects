import * as jwt from 'jsonwebtoken';

const JWT_SIGNING_SECRET = 'this_secret_should_come_from_hidden_env_files_and_never_committed_to_repo';

const generateJwtForUserId = (userId: string) => jwt.sign({ userId }, JWT_SIGNING_SECRET);

export const AuthService = () => ({
	generateJwtForUserId,
});
