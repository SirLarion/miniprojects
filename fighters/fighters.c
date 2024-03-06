/* #include "fighters.h" Header file contents included in .c file due to header
 * not being recognised in TIM */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_MSG_LEN 100
#define MAX_CMD_LEN 80
#define MAX_STR_LEN 30
#define ABS_MAX_HP 500
#define ABS_MAX_DMG 100
#define ABS_MAX_EXP 10000

//The categories of pseudorandomized messages gotten from getMSG
typedef enum {
  ADDCHAR,
  LOWDMG,
  MIDDMG,
  HIGHDMG,
  KNOCKOUT
} Msgtype;

/* Separate struct for the weapions characters have. Not strictly
 * necessary but leaves room for expansion. */
typedef struct {
  char name[MAX_STR_LEN];
  int maxdmg;
} Weapon;

//The attributes of a character in the fight
typedef struct {
  char name[MAX_STR_LEN];
  unsigned int maxhp;
  int hp;
  unsigned int exp;
  Weapon weapon;
} Character;

/* The latest command inputted by the user. Easy to process
 * as it gets carried as a value of the fight struct */
typedef struct {
  char str1[MAX_STR_LEN];
  char str2[MAX_STR_LEN];
  int int1;
  int int2;
  int init;
} Command;

/* The current state of the program. Only one "Fight" exists. Loading a new one
 * only changes the values of the existing one. */
typedef struct {
  Character* characters;
  unsigned int num;
  Command cmd;
} Fight;


void welcome(){
  char* title = malloc(600);
  strcpy(title, \
"||==============================================================================||\n\
||     dMMMMMP dMP .aMMMMP dMP dMP dMMMMMMP dMMMMMP dMMMMb  .dMMMb       .aMMMb ||\n\
||    dMP     amr dMP\"    dMP dMP    dMP   dMP     dMP.dMP dMP\" VP      dMP\"VMP ||\n\
||   dMMMP   dMP dMP MMP\"dMMMMMP    dMP   dMMMP   dMMMMK\"  VMMMb       dMP      ||\n\
||  dMP     dMP dMP.dMP dMP dMP    dMP   dMP     dMP\"AMF dP .dMP  amr dMP.aMP   ||\n\
|| dMP     dMP  VMMMP\" dMP dMP    dMP   dMMMMMP dMP dMP  VMMMP\"  dMP  VMMMP\"    ||\n\
||==============================================================================||\n");

  int i = 0;
  while(title[i]){
    printf("%c", title[i]);
    i++;
  }
  printf("\n\nA production by Miska Tammenpää\n\n");
  free(title);
}

/* Get a pseudorandomized message from category defined by Msgtype and save it into dst.
 * Used for increased variety in the outputs printed after the user inputs a command */
void getMSG(char* dst, Msgtype type){
  srand(time(0));
  int msgNo = rand() % 3;
  switch(type){
    //Messages related to addCharacter function
    case ADDCHAR:
      switch(msgNo){
        case 0:
          strcpy(dst, "A fighter has joined the battle!");
          break;
        case 1:
          strcpy(dst, "Someone's coming... A new fighter!?");
          break;
        case 2:
          strcpy(dst, "Hold on to your seats, there's a new fighter!");
          break;
        default:
          strcpy(dst, "A new warrior has entered the ring!");
          break;
      }
      break;
    //Messages related the attackCharacter where the attacker deals low damage
    case LOWDMG:
      switch(msgNo){
        case 0:
          strcpy(dst, "%s attacks but it's sloppy...");
          break;
        case 1:
          strcpy(dst, "%s lunges but their attack is weak.");
          break;
        case 2:
          strcpy(dst, "%s's attack grazes their opponent.");
          break;
        default:
          strcpy(dst, "%s attacks but it doesn't land very well.");
          break;
      }
      break;
    //Messages related the attackCharacter where the attacker deals average to high damage
    case MIDDMG:
      switch(msgNo){
        case 0:
          strcpy(dst, "Look at %s go, a nice shot!");
          break;
        case 1:
          strcpy(dst, "%s lines up their attack and lands a hit!");
          break;
        case 2:
          strcpy(dst, "Wow, %s lands a nice attack.");
          break;
        default:
          strcpy(dst, "Ohh, what a shot from %s!");
          break;
      }
      break;
    //Messages related the attackCharacter where the attacker deals maximum damage
    case HIGHDMG:
      switch(msgNo){
        case 0:
          strcpy(dst, "OH lord, look at that shot from %s!");
          break;
        case 1:
          strcpy(dst, "INCREDIBLE blow dealt by %s!");
          break;
        case 2:
          strcpy(dst, "%s lands a FIERCE blow!");
          break;
        default:
          strcpy(dst, "An amazing shot from %s! That takes some serious skill.");
          break;
      }
      break;
    //Messages related the attackCharacter where the defender gets knocked out
    case KNOCKOUT:
      switch(msgNo){
        case 0:
          strcpy(dst, "%s takes the cake on that one!\"\n");
          break;
        case 1:
          strcpy(dst, "%s shows no mercy!\"\n");
          break;
        case 2:
          strcpy(dst, "%s is showing everyone how it's done!\"\n");
          break;
        default:
          strcpy(dst, "A monstrous knockout from %s!\"\n");
          break;
      }
      break;
    default:
      strcpy(dst, "");
      break;
  }
}

//Helper method for checking if a character is between 0 and 9 in ASCII
int isNum(char c){
  return (c > 48 && c < 58);
}

//Helper method for turning a string into lower case
void stringLower(char* dst, const char* str){
  int i = 0;
  dst[i] = '\0';
  while(str[i]){
    dst[i] = tolower(str[i]);
    i++;
  }
  dst[i] = '\0';
}

//Calculates what level a character is on based on their amount of exp
int getLevel(int xp){
  int i = 0;
  int lim = 630*(i > 0 ? i : 1);
  while(xp > lim){
    xp -= lim;
    i++;
    lim = 630*i;
  }
  return i;
}

/* Calculates the damage dealt pseudorandomly as a function of the character's
 * max damage and the level they are on */
int calcDMG(int max, int lvl){
  srand(time(0));
  int rd = rand() % (max+1);
  if(lvl > 0){
    /* The range of possible values given by the function decreases as the
     * character's level increases */
    int lim = (lvl/6) * max;
    while(rd < lim){
      if((rand() % 6) < lvl){
        break;
      }
      rd = rand() % (max+1);
    }
  }
  return rd;
}

/* Gets the longest singular string currently existing in the fight
 * (a character's name or a weapon's name in practice)
 * Used for setting the size of the decorative bounding box in the printAll function */
int getMaxString(Fight* fight){
  int current = 6;
  int i = 0;
  while(i < fight->num){
    int newLenChar = strlen(fight->characters[i].name);
    int newLenWep  = strlen(fight->characters[i].weapon.name);
    current = newLenChar > current ? newLenChar : current;
    current = newLenWep  > current ? newLenWep  : current;
    i++;
  }
  return current;
}

/* initialises the fight struct by allocating memory to the list
 * of characters it contains and storing their amount into num */
void initFight(Fight* fight, unsigned int numCharacters){
  fight->characters = malloc((numCharacters+1)*sizeof(Character));
  fight->num = numCharacters;
}

/* A precursor to shutting down the program. Frees all the allocated
 * memory that is left */
void quitFight(Fight* fight, char* cmdPtr){
  printf("ANNOUNCER: \"Thank you, my beloved audience, for joining us today! Have a good night\"\n");
  free(fight->characters);
  free(fight);
  free(cmdPtr);
}

/* Tries to find a character with the given name. Returns a pointer
 * to the character if it is found or a NULL pointer if not. */
Character* findCharacter(Fight* fight, const char* name){
  unsigned int i = 0;
  char* name2 = malloc(strlen(name)+1);
  stringLower(name2, name); //Comparisons in lower case -> not case sensitive
  while(i < fight->num){
    Character* current = &(fight->characters[i]);
    char* name1 = malloc(strlen(current->name)+1);
    stringLower(name1, current->name);
    if(!strcmp(name1, name2)){
      free(name1);
      free(name2);
      return current;
    }
    free(name1);
    i++;
  }
  free(name2);
  return NULL;
}

/* Comparison function for qsort. Compares two characters based
 * on the amount of exp they have and whether they're dead or not. */
int cmpExp(const void* a, const void* b){
  int aEXP = (*(Character*)a).hp > 0 ? (*(Character*)a).exp : -1;
  int bEXP = (*(Character*)b).hp > 0 ? (*(Character*)b).exp : -1;
  //Characters' exps if their hp is positive, otherwise -1
  return (bEXP - aEXP);
}

/* Tries to add a new character with the given attributes into the fight.
 * If successful the character is stored inside the 'fight' struct's
 * 'characters' array */
void addCharacter(Fight* fight, const char* name, int maxhp, const char* wepName, int maxdmg){
  //Check if a character with the given name already exists
  if(!findCharacter(fight, name)){
    //Check if the given maxhp and maxdmg are valid. Otherwise print a message accordingly
    if(maxhp <= 0){
      printf("ANNOUNCER: \"A new warrior has entered the r.. wait no, they're already dead!\"\n");
    }
    else if(maxdmg <= 0){
      printf("ANNOUNCER: \"Sorry %s, you've been disqualified! We're here to DO damage!\"\n", name);
    }
    else if(maxhp >= ABS_MAX_HP || maxdmg >= ABS_MAX_DMG){
      printf("ANNOUNCER: \"Whoaa %s, that's a little overboard. Let's give the other fighters a chance shall we?\"\n", name);
    }
    else {
      fight->characters = realloc(fight->characters, ((fight->num)+1)*sizeof(Character));
      Character* newChar = &(fight->characters[fight->num]);
      fight->num++;

      //initialise weapon with given values
      Weapon wep;
      strcpy(wep.name, wepName);
      wep.maxdmg = maxdmg;

      //initialise new character into fight->characters to the end of the list
      strcpy(newChar->name, name);
      newChar->maxhp  = maxhp;
      newChar->hp     = maxhp;
      newChar->exp    = 0;
      newChar->weapon = wep;

      //Print a pseudorandom message (see getMSG) indicating the adding was successful
      char* msg = malloc(MAX_MSG_LEN);
      getMSG(msg, ADDCHAR);

      printf("ANNOUNCER: \"%s \nANNOUNCER: %s; HP: %d, dealing a maximum of %d damage with their %s!\"\n", msg, name, maxhp, maxdmg, wepName);
      free(msg);
    }
  }
  else {
    printf("ANNOUNCER: \"%s is already in the ring! We can't have another one!\"\n", name);
  }
}

/* Tries to make the character with name1 attack the character with name2.
 * If successful, changes the values of the relevant attributes in the
 * Character structs. */
void attackCharacter(Fight* fight, const char* name1, const char* name2){
  Character* offense = findCharacter(fight, name1);
  Character* defense = findCharacter(fight, name2);
  /* Check if either of the characters exists and whether they
   * are dead or not. */
  if(!offense && !defense){
    printf("ANNOUNCER: \"The crowd goes wild as %s attacks %s but dissappointment ensues with the realization that neither character exists!\"\n", name1, name2);
  }
  else if(!offense){
    printf("ANNOUNCER: \"%s gets ready to defend but is relieved when they realize that there's no %s in the ring.\"\n", defense->name, name1);
  }
  else if(!defense){
    printf("ANNOUNCER: \"%s lunges at %s but their attack flies right through because %s doesn't exist!\"\n", offense->name, name2, name2);
  }
  else if(offense->hp <= 0){
    printf("ANNOUNCER: \"%s is dead! This is just adding insult to injury...\"\n", offense->name);
  }
  else if(defense->hp <= 0){
    printf("ANNOUNCER: \"%s kicks %s's dead body! My god, what an insult!\"\n", offense->name, defense->name);
  }
  else if(!strcmp(offense->name, defense->name)){
    printf("ANNOUNCER: \"%s punches themself in the face! Quite a weak shot though...\"\n", offense->name);
  }
  else {
    //Simpler variables for ease of use
    char* n1 = malloc(30);
    char* n2 = malloc(30);
    strcpy(n1, offense->name);
    strcpy(n2, defense->name);
    int* hp = &(defense->hp);
    int lvl = getLevel(offense->exp);
    int max = offense->weapon.maxdmg;
    int dmg = calcDMG(max, lvl);

    //Check if the damage dealt is not zero
    if(dmg){
      /* Print a pseudorandom message (see getMSG) depending on the amount of
       * damage dealt */
      char* msg = malloc(MAX_MSG_LEN);
      if(dmg == max && (dmg/(*hp) > defense->maxhp/10)){
        getMSG(msg, HIGHDMG);
      }
      else if(dmg < max/3 && (dmg/(*hp) < defense->maxhp/10)){
        getMSG(msg, LOWDMG);
      }
      else {
        getMSG(msg, MIDDMG);
      }
      printf("ANNOUNCER: \"");
      printf(msg, n1);
      printf(" %s takes %d damage from %s's %s!\"\n", n2, dmg, n1, offense->weapon.name);
      int xpGain = (*hp * 10)/(max); //Amount of exp gained is gotten from function:
      *hp -= dmg;                    //(defender-hp*10)/attacker-maxdmg
      if(*hp <= 0){
        getMSG(msg, KNOCKOUT);
        printf("ANNOUNCER: \"AND %s GOES DOWN! ", n2);
        printf(msg, n1);
        xpGain += (defense->maxhp * 10); //Bonus xp if the defender is knocked out
      }
      printf("ANNOUNCER: \"%s gains %d exp.\n", n1, xpGain);
      offense->exp += xpGain;
      //Check if the attacker's level increased as a result of the attack
      if(getLevel(offense->exp) > lvl){
        int hpGain = (lvl+1)*10;
        offense->hp += hpGain;
        offense->maxhp += hpGain;
        printf("ANNOUNCER: \"%s has leveled up! %d HP gained!\"\n", n1, hpGain);
      }
      if(offense->exp > ABS_MAX_EXP){
        offense->exp = ABS_MAX_EXP -1;
      }
      free(msg);
    }
    else {
      printf("ANNOUNCER: \"%s attacks and.. OHH barely misses! %s is left without a scratch this time.\"\n", n1, n2);
    }
    free(n1);
    free(n2);
  }
}

//Prints the character with the given name if such a character exists.
void printCharacter(Fight* fight, const char* name){
  //Get a pointer to the character in question. NULL if doesn't exist
  Character* target = findCharacter(fight, name);
  int len = getMaxString(fight) +1;
  if(target){
    printf("|%*s %-3d %-3d %-4d %-*s|\n", len, target->name, target->hp, target->weapon.maxdmg, target->exp, len, target->weapon.name);
  }
  else {
    printf("ANNOUNCER: \"Looking around, I can't for the life of me find someone called %s\"\n", name);
  }
}

/* Prints all characters currently in the fight (dead or not). Decorated with a dynamically sized
 * bounding box.
 */
void printAll(Fight* fight){
  int len = getMaxString(fight) +1; //Scaling factor for the box
  char* deco = malloc(len+1); //Extra lines when maxString is longer
  int i = 0;
  for(; i < len; i++){
    deco[i] = '-';
  }
  deco[i] = '\0';
  //Sort the list of characters according to the given comparison function
  qsort(fight->characters, fight->num, sizeof(Character), cmpExp);
  printf("|%s--------------%s|\n", deco, deco);
  printf("|%*s %-3s %s %-4s %-*s|\n", len, "Name", "HP", "DMG", "EXP", len, "Weapon");
  printf("|%s--------------%s|\n", deco, deco);
  for(int i = 0; i < fight->num; i++){
    printCharacter(fight, fight->characters[i].name);
  }
  printf("|%s--------------%s|\n", deco, deco);
  free(deco);
}

//Print help message
void printHelp() {
  printf("ANNOUNCER: \"Here's a little guide to get you started in the ring!\"\n\n");
  printf("\
          Add character:      A {NAME} {HP} {WEAPON} {MAXDMG}\n\
          Attack character:   H {ATTACKERNAME} {DEFENDERNAME}\n\
          Write into file:    W {FILENAME}\n\
          Read from file:     R {FILENAME}\n\
          Print characters:   L\n\
          Print help:         ?\n\
          Close program:      Q\n");
}

//Save the state of the fight (the data of the characters) into a file with the given name
int saveFight(const Fight* fight, const char* filename){
    char* fname = malloc(strlen(filename)+4);
    strcpy(fname, filename);
    strcat(fname, ".fc");                //Add a file format identifier so only files saved by the
    FILE* f = fopen(fname, "w");        //program will be loaded
    //Return if unable to open file
    if(f==NULL){
      return 0;
    }
    //Helper pointer for ease of use
    Character* list = fight->characters;
    int i = 0;
    while(i < fight->num){
      fprintf(f, "%s %d %d %d %d %s\n", list[i].name, list[i].hp, list[i].weapon.maxdmg, list[i].maxhp, list[i].exp, list[i].weapon.name);
      i++;
    }
    fclose(f);
    free(fname);
    return 1;
  }

/* Load a fight from the file with the given name,
 * discard the current fight and replace it with the data from the file
 */
int loadFight(Fight* fight, const char* filename){
  char* fname = malloc(strlen(filename)+4);
  strcpy(fname, filename);
  strcat(fname, ".fc");
  FILE* f = fopen(fname, "r");
  if(f==NULL){
    return 0;
  }
  free(fight->characters);
  fight->characters = malloc(sizeof(Character));
  //Temp variable for storing a single row of the file
  char* buf = malloc(MAX_CMD_LEN+5); //Possibly longer than the longest command due to
  int i = 0;                         //exp being stored in the file
  while(fgets(buf, MAX_CMD_LEN+5, f)){
    sscanf(buf, "%s %d %d %d %d %s", fight->characters[i].name,   &(fight->characters[i].hp), &(fight->characters[i].weapon.maxdmg), \
                                   &(fight->characters[i].maxhp), &(fight->characters[i].exp),  fight->characters[i].weapon.name);
    fight->characters = realloc(fight->characters, (i+2)*sizeof(Character));
    i++;
    strcpy(buf, "\0");
  }
  //Number of characters is the number of rows in the file
  fight->num = i;
  fclose(f);
  free(buf);
  free(fname);
  return 1;
}

/* Reads through the command given as a parameter in string form and extracts relevant
 * parameters from it. The method then saves the parameters into the 'cmd' structure inside
 * the given 'fight' structure.
 */
void parseCommand(Fight* fight, char* cmd){
  //Temporary space for the parameters in the command
  char* strParam1 = malloc(MAX_STR_LEN);
  char* strParam2 = malloc(MAX_STR_LEN);
  strcpy(strParam1, "\0");
  strcpy(strParam2, "\0");
  int intParam1 = 0;
  int intParam2 = 0;

  /* Bit-level bus for detecting which values have been initialised (e.g. 1000 -> strParam1 has been initialised,
   * 0101 -> both int parameters have been initialised
   * 1111 -> all values have been initialised*/
  int init = 0x0;

  //Temp storage for a single word
  char* cmdWord = malloc(MAX_STR_LEN);
  strcpy(cmdWord, "\0");

  /* Keep going through the string until either the end of the string
   * has been reached or all parameters have been initialised */
  while(init != 0xF && cmd){
    //At this point the current character is always whitespace
    cmd++;
    if(isNum(cmd[0]) || cmd[0] == '-'){      //Check if current character is a digit (or sign),
      if((init & 0x4) != 0x4){               //save parameters as an integer if true
        intParam1 = strtol(cmd, &cmd, 10);
        init |= 0x4;
      }
      else if((init & 0x1) != 0x1) {
        intParam2 = strtol(cmd, &cmd, 10);
        init |= 0x1;
      }
    }
    else {                                   //Otherwise save as a string parameter
      sscanf(cmd, "%s", cmdWord);
      if((init & 0x8) != 0x8){
        strcpy(strParam1, cmdWord);
        init |= 0x8;
      }
      else if((init & 0x2) != 0x2) {
        strcpy(strParam2, cmdWord);
        init |= 0x2;
      }
    }
    //Jump to the next whitespace
    cmd = strchr(cmd, ' ');
  }
  strcpy(fight->cmd.str1, strParam1);        //Store the temp variables in 'fight->cmd'
  strcpy(fight->cmd.str2, strParam2);
  fight->cmd.int1 = intParam1;
  fight->cmd.int2 = intParam2;
  fight->cmd.init = init;
  free(cmdWord);
  free(strParam1);
  free(strParam2);
}

/* Check the identifier (first character) of a command and call the appropriate method
 * with the parameters currently stored in fight's command struct
 */
void processCommand(Fight* fight, char id){
  Command cmd = fight->cmd;

  switch(tolower(id)) {
    case ' ':
      break;

    case '0':
      //Initial empty command
      printf("ANNOUNCER: \"Welcome everyone to the battle-arena!\"\n");
      break;

    case 'a':
      //Jump to addCharacter if all parameters have been initialised
      if(cmd.init == 0xF){
        addCharacter(fight, cmd.str1, cmd.int1, cmd.str2, cmd.int2);
      }
      //Otherwise print error message
      else {
        printf("ANNOUNCER: \"Sorry, but we'll need some more accurate info than that to describe a fighter!\"\n");
        printf("(Syntax: A {NAME} {HP} {WEAPON} {MAXDMG})\n");
      }
      break;

    case 'h':
      //Jump to attackCharacter if at least both string parameters have been initialised
      if(cmd.init >= 0xA){
        attackCharacter(fight, cmd.str1, cmd.str2);
      }
      else {
        printf("ANNOUNCER: \"We gotta have some more details on who's attacking whom here!\"\n");
        printf("(Syntax: H {ATTACKERNAME} {DEFENDERNAME})\n");
      }
      break;

    case 'l':
      //Jump to printALl
      printAll(fight);
      break;

    case 'w':
      //Jump to saveFight if at least the first string parameter has been initialised
      if(cmd.init >= 0x8){
        if(saveFight(fight, cmd.str1)){
          printf("ANNOUNCER: \"Ladies and gentlemen, this fight has been such a pleasure to follow -\nANNOUNCER: - that we have decided to archive it as \'%s\' for later use!\"\n", cmd.str1);
        }
        else {
          printf("ANNOUNCER: \"It seems saving this fight into our archives has failed! What a terrible plunder...\"");
        }
      }
      break;

    case 'r':
      //Jump to saveFight if at least the first string parameter has been initialised
      if(cmd.init >= 0x8){
        if(loadFight(fight, cmd.str1)){
          printf("ANNOUNCER: \"Allllright, welcome back ladies and gentlemen! Here's a quick recap of where we left last time:\"\n");
          printAll(fight);
        }
        else {
          printf("ANNOUNCER: \"Unfortunately, after combing through our archives, a fight under the name \'%s\' couldn't be retrieved.\"\n", cmd.str1);
        }
      }
      break;

    case '?':
      //Print help message
      printHelp();
      break;

    default:
      //Every other identifier is disregarded
      printf("ANNOUNCER: \"I don't quite understand what you meant with that\"\n");
      break;
  }
}

int main() {
  //Print banner
  welcome();
  Fight* fight = malloc(sizeof(Fight));
  initFight(fight, 0); //Initialise fight with 0 characters
  char identifier = '0';
  char* cmd = malloc(MAX_CMD_LEN);
  char* cmdInit = cmd;
  strcpy(cmd, "0\0");

  //Keep asking for commands until the quit command
  while(tolower(identifier) != 'q'){
    parseCommand(fight, cmd);           //Store the contents of the command into fight->cmd
    processCommand(fight, identifier);  //Process the values of fight->cmd according to the identifier
    cmd = cmdInit;
    strcpy(cmd, "\0");
    printf(">");
    fgets(cmd, MAX_CMD_LEN, stdin);
    if(strlen(cmd) > 1){
      identifier = cmd[0];
    }
    else {
      identifier = ' '; //Whitespace if the command is empty.
    }
    cmd++;
  }
  //Free all memory and shutdown
  quitFight(fight, cmdInit);
  return 0;
}
