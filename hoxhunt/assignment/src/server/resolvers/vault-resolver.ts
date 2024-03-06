import { Resolver, Query } from 'type-graphql';

import { Vault } from '../entities/vault';
import { InjectRepository } from 'typeorm-typedi-extensions';
import { Repository } from 'typeorm';
import { Service } from 'typedi';

@Service()
@Resolver(of => Vault)
export class VaultResolver {
	constructor(@InjectRepository(Vault) private readonly vaultRepository: Repository<Vault>) {}

	@Query((returns) => Vault)
	vault(): Promise<Vault> {
		return this.vaultRepository.findOne();
	}
}
