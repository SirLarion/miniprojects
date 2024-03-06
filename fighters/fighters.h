#ifndef FIGHTERS_H
#define FIGHTERS_H

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


#endif
