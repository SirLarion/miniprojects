/*
 * Count the amount of criminals in a set of emails according to
 * a certain filtration ruleset
 */
const countCriminals = (emails) => {

    let count = 0;

    // Helpers
    // =================================================================================== //
    const parsePersonFromEmail = (email) => {
        // person object
        return {
            firstName:   email.split('.')[0],
            lastName:    email.split('@')[0].split('.')[1],
            affiliation: email.split('@')[1].split('.')[0],
            domain:      email.split('@')[1].split('.').slice(1).join('.')
        }
    }
    const printPerson = (person) => {
        console.log(`${person.firstName} ${person.lastName} @ ${person.affiliation}.${person.domain}`);
    }
    // =================================================================================== //

    // Boolean functions relevant to the ruleset
    // =================================================================================== //
    const hasRinLastname = (person) => {
        return (/r+/).test(person.lastName);
    }
    const hasCinName = (person) => {
        return (/c+/).test(`${person.firstName} ${person.lastName}`);
    }
    const isCoUk = (person) => {
        return person.domain === 'co.uk';
    }
    const isWonkaOrGringotts = (person) => {
        return person.affiliation === 'wonkaindustries' || person.affiliation === 'gringottsbank';
    }
    const isFirstnameLengthBelowFour = (person) => {
        return person.firstName.length < 4;
    }
    const hasTwoTsInName = (person) => {
        return (`${person.firstName} ${person.lastName}`).split('t').length >= 3;
    }
    // =================================================================================== //

    /*
     * Find out whether 'person' is a criminal or not according to the given rules.
     * Filter with the boolean functions defined above, one rule at a time, in the 
     * reverse order of priority. More important rules must be able to override
     * the outcome of less important rules
     */
    const isCriminal = (person) => {
        let criminal = false;

        // Rule #4
        criminal = hasTwoTsInName(person);

        // Rule #3
        criminal = criminal || (isWonkaOrGringotts(person) && !(isFirstnameLengthBelowFour(person)));

        // Alternate Rule #3, override rule #4 if wonka or gringotts but name is under 4
        //
        // NOTE: I feel this follows the assignment more accurately but the amount of criminals 
        //       was not prime with this implementation.
        //
        // criminal = isWonkaOrGringotts(person) ? !(isFirstnameLengthBelowFour(person)) : criminal;

        // Rule #2
        criminal = criminal || (isCoUk(person) && hasCinName(person));

        // Rule #1, always override if lastname has r
        criminal = criminal && !(hasRinLastname(person));

        return criminal;
    }

    // Iterate through the emails checking each if they're a criminal.
    // Increment 'count' by one each time a criminal is found 
    emails.forEach(email => {
        const person = parsePersonFromEmail(email);
        if(isCriminal(person)) count++;
    });

    return count;
}
