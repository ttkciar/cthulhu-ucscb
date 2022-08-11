/*	%Z% %M% %I% %E% %Q%	*/

#ifdef PROTO
#include "proto.h"
#endif

#ifndef NOCURSES
#include <curses.h>
#include <fcntl.h>
#include <sys/types.h>
#else
#include "doscrs.h"
#endif

#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdlib.h>

#ifdef SYSV
#include <string.h>
#include <unistd.h> 
#else
#include <strings.h>
#endif

#define	HIGH_SCORE	"/usr/games/lib/cth.score"
#define	DATAFILE	"/usr/games/lib/.datafile"
#define	SAVE_LIST	"/usr/games/lib/.graves"

/*	Cthulhu.c
	Jason Abbott	(jasona@sco.com)
	1990 (August?)

	This game is possible due to the playtesters of UCSC 'b'
	and all their helpful coments. Yes, it is a little bit of
	a rip off of Chaosium's "Call of Cthulhu", but only to the
	extent of using sanity as well as hit points, and in the
	fact that I "barrowed" some of H.P.'s imagination, which
	they own the game rights to. Oh well...

	There is a man page/doc/help/README file that should go with 
	this game, if you don't have a copy, just mail me at one of 
	the two above addresses and I'll send one by Nightgaunt Express.

	SYSV(xenix)	compile with -DTERMCAP -Ox -xenix -lcurses
	SYSV(unix)	compile with -DTERMCAP -Ox -lcurses
	BSD(unix)	compile with -lcurses -ltermcap
*/

#define VERSION		".6.5 "
/*	4.7	Added Mythos spell
	4.8	Fixed Corrupt spell
		Fixed adding to many attributes to subtract from attributes
		Made Regeneration more expensive for monsters
		Made level 0 cities easier to start in
	4.9	Fixed error in corruption levels (everything being set < 200)
		Changed "finatics" to "fanatics" (geez...)
	4.10	Spelling Errors
		Cause of death for Hospitals and Asylums
		Streamlined Screen Usage
		Scrolling statements
	5.0	Adaptation for larger size screens
		Fixed some long lines
		Fixed Escape spell
		Added Adaptable starting stats
		Added losing items
		Added thieving monsters
		Added starving and eating
		Not effected by Disruption if you have a ranged ('n') weapon
		New spells (Finding, Food Crtn, Invis, Sacrifice, Isolation)
	5.1	Ichor Demon Susceptable to light, not summoner
		Spend your last ten on food
		Tell you when Invisibility does or doesn't work
	5.2	Can't eat when not on earth
		set group for save-game files (chmod 660)
		Anonymous high score name (-n flag)
	5.3	All names 10 characters or less
		Disruption and the Excel spell cause hunger
		Vampirism feeds you
		Toned down Isolation and Invisibility
	5.4	Removed a modulo by zero error in useobject
		Make it a little harder for monsters to find you
		Vampire spell cures hunger
		Cleansing spell does damage relative to intelligence
		Fixed save game function (corruption being incr. too much)
	5.5	Gave user choice over SACRIFICE benefits
		Protection and immunity help versus disease
		Iron Will Tattoo gives you more power
		Intelligence Tattoo gives you more sanity
	5.6	touched up corruption spell
		twidled with the recursive finding function
		added longevity spell
		diseases can run out of steam
	5.7	Elder Signs
		make longevity spell more powerful
	6.0	fixed neverending Elder signs
		spelling corrections (thanks to entropy@ucscb)
		wrote man page
		cumbersome save-game guards installed
	6.1	re-wrote fight script to be more coheirent
		fixed elder signs (if you had one still working, they all did)
		installed universal/relative corruption rating
		altered discover display
		added three more names to annonymous
		added the '^' key to study for just one day
	6.2	Fixed lack of printw statements for stolen objects and diseases
		Added COUNT_DOWN for time optimization
		Function to Macro speed enhancements (WAIT, STATE, EARN_MONEY)
		Reworked WAIT around fight so you can see disease/theft/etc...
		fixed teleportation and dimensional travel
	6.3	Broke up cthulhu.c into many little .c files
		Added command line args [SDI]
		Changed initialization of Str/Dex/Int from 8+n to n
		Fixed teleport spell bug (return to list #, not city #)
		Added fifteen monsters from the Dreamlands & my imagination
		Added monster powers Death_Aura and Webs
		Changed SCO to SYSV
		Updated man page
		Recognize user name via get_username()
	6.4	Print out ask() three lines from bottom, not four
		Remove debug statement from use of D-travel book
		Added the composelist() routine
		Print out names of identified objects
		Adjusted where ask() places questions
	6.5	Use getlogin() on non-SYSV machines
		Fix killing off the 12 headmasters == winning the game
		Hospitals and Longevity tell you what diseases you have
		(maybe) cure charged objects from dissappearing
		Identify spell works as identify command as per 6.4
		Prints out propper message if scared to death by monster
		Turn off elder sign if stolen by imps (et all)
		Player must hit a key to acknowledge some statements
*/

#define SAVEGAME	"/cthave"
#define ENV_STR		"CTHULHU_STR"
#define ENV_DEX		"CTHULHU_DEX"
#define ENV_INT		"CTHULHU_INT"

#define	PIPE	"a pipe"
#define	AXE	"an axe"
#define SIGN	"an elder sign"

#define NCITIES		47
#define NSPELLS		28
#define NOBJECTS	59
#define NMONSTERS	98
#define NABILITIES	7
#define NDIMS		12
#define NPLAGUE		10
#define NCORRUPTION	9
#define NUMBERSIGNS	7
#define GROWTH		100
#define MAX_ON_HS	100
#define MAX_SAVERS	100

#define AARMOR		0
#define AMENTALHEALTH	1
#define AHOLY		2
#define AWILL		3
#define ASEEINVIS	4
#define ALEARN		5
#define AREGEN		6

#define RETURN		1
#define DISCOVER	2
#define HEALING		3
#define IDENTIFY	4
#define REGENERATION	5
#define SANCTUARY	6
#define SOLITUDE	7
#define STEALTH		8
#define TELEPORT	9
#define BERSERKER	10
#define BLAST		11
#define CLEANSING	12
#define CORRUPT		13
#define ESCAPE		14
#define EXCEL		15
#define HOLY_AURA	16
#define IMMUNITY	17
#define JUDGEMENT	18
#define PROTECTION	19
#define VAMPIRE		20
#define WITHER		21
#define MYTHOS		22
#define FINDING		23
#define FOOD		24
#define	INVISIBILITY	25
#define SACRIFICE	26
#define	ISOLATION	27
#define LONGEVITY	28

#define HANDS		-1
#define GROUP		953

#ifdef SYSV
#define POWER(m,x)	(strchr(mon[m].abilities, x) != NULL)
#else
#define memcpy(x,y,z)	bcopy(y,x,z)
#define POWER(m,x)	(index(mon[m].abilities, x) != NULL)
#endif

#define ABS(x)		((x>0)?(x):-(x))
#define GIFT(m,x)	(obj[m].abilities == x)
#define NAME(i)		((obj[i].have==1)?obj[i].generic:obj[i].name)
#define COUNT_DOWN(i,c) for (i=c; --i >= 0; )
#define STATE(str)	move(st_pos,0); addstr(str); move(++st_pos,0); \
				if (st_pos == LINES) { WAIT(); }
#define WAIT()		ask("-> more <-"); while (getchar() != ' ')
#define EARN_MONEY()	if (rand() % 20 < player.intel) player.money += ((rand() % player.str) + 3)
#define MOD_2(n)	(n & 0x1)
#define	MOD_4(n)	(n & 0x3)
#define MOD_8(n)	(n & 0x7)
#define MOD_16(n)	(n & 0x17)
#define ITOLISTICON(j)	(((j) < 10) ? '0' + (j) : \
			 ((j) < 36) ? 'a' + (j) - 10 : 'A' + (j) - 36)

struct plague_table {
	unsigned caught : 1;
	char *name;
} ;

struct spell_table {
	char *name;
	unsigned sanlos : 5;
	unsigned ability : 5;
	unsigned cbt : 1;
	unsigned know : 1;
} ;

struct monster_table {
	char *name;
	unsigned san : 7;
	unsigned dice : 7;
	unsigned dam : 7;
	unsigned body : 10;
	unsigned dex : 6;
	int level;
	char *abilities;
};

struct object_table {
	char *name;
	char *generic;
	char abilities;
	unsigned dice : 3;
	unsigned dam : 4;
	int worth;
	int charges, lastused;
	unsigned sanlos : 6;
	unsigned have : 2;
} ;

struct city_table {
	char name[30];
	int lat;
	unsigned lon : 9;
	unsigned mon : 6;
	unsigned tres : 6;
	int level;
} ;

struct player_table {
	int sanity, topsan, body, topbody, money, score, total_corruption;
	int city, dex, str, intel, objects, spells, time;
	int weapon, stealth, longevity, drunk, food, feddisease;
	unsigned int usingsign:1;
	int ability[NABILITIES];
	char name[20];
} ;

struct fight_status {
	int lastcbt;
	char rage, immune, protected, clensing;
	char escape, excel, invisible, holyaura, webs;
} ;

extern struct player_table player;
extern struct plague_table plague[NPLAGUE]; 
extern struct city_table cty[NCITIES];
extern struct object_table obj[NOBJECTS];
extern struct monster_table mon[NMONSTERS];
extern struct spell_table spell[NSPELLS];

extern struct fight_status ft_stat;

extern char death[80], tmp[80];
extern char *dlev[NCORRUPTION];
extern char *headmaster[NDIMS];

extern unsigned int goal;		/* bitmap of headmasters' killed */

extern int pc, highlist, st_pos, mpd;
extern int cpc[NCITIES], tpc[NCITIES], cp[NCITIES];
extern int spl[NSPELLS];
extern int object[NOBJECTS];
extern int errno;

extern void Quit(), buserr(), segerr();
extern int sign_index[NUMBERSIGNS];
