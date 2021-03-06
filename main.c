/******* *********************************************************************/
/*                                                                           */
/*                                		                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017      Fabrizio Caruso                                             */
/*                					                     */
/*              				                             */
/* EMail:        Fabrizio_Caruso@hotmail.com                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/
 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "settings.h"
#include "character.h"
#include "strategy.h"
#include "text.h"
#include "missile.h"
#include "input.h"
#include "level.h"
#include "enemy.h"
#include "invincible_enemy.h"
#include "powerups.h"

// Input input/output driver headers
#include "display_macros.h"
#include "input_macros.h"
#include "sleep_macros.h"
#include "sound_macros.h"


// #define DEBUG_ATARI
// #define DEBUG_CHARACTERS

unsigned int invincibleSlowDown = 30000;
unsigned short invincibleXCountDown = 100;
unsigned short invincibleYCountDown = 100;

unsigned int ghostSlowDown;
unsigned int powerUpCoolDown;
	
unsigned int gunInitialCoolDown;
unsigned int gunCoolDown;

unsigned int ghostLevelDecrease = 100;
unsigned int powerUpInitialCoolDown = 100; 

unsigned int ghostLevel = 1u;
unsigned long points = 0ul;

unsigned short playerDirection = 0; // 0: right, 1: down, 2: left, 3: up
unsigned short missileDirection = 0;
unsigned short playerFire = 0;
unsigned short guns = GUNS_NUMBER;

unsigned short lives;

unsigned short innerVerticalWallY; 
unsigned short innerVerticalWallX; 
unsigned short innerVerticalWallLength;

// Level
// The level affects:
// 1. powerUpCoolDown (how long before a new powerUp is spawned)
// 2. ghostSlowDown (how much the power up slows the enemies down)
// 3. invincibleXCountDown (time needed to activate the invincible ghost)
// 4. invincibleYCountDown
// 5. invincibleSlowDown (how much the invincible ghost is slowed-down)
// 6. invincibleLoopTrigger (how long before the invincible ghost appears)
unsigned short level = 1;

unsigned int invincibleLoopTrigger = 1000;
unsigned short invincibleGhostCountTrigger = 2;

unsigned short ghostCount = GHOSTS_NUMBER;

unsigned char XSize;
unsigned char YSize;

unsigned int loop;

extern Image PLAYER_IMAGE;
extern Image GHOST_IMAGE;
extern Image DEAD_GHOST_IMAGE;
extern Image INVINCIBLE_GHOST_IMAGE;
extern Image BOMB_IMAGE;
extern Image POWERUP_IMAGE;
extern Image MISSILE_IMAGE;
extern Image GUN_IMAGE;



Character invincibleGhost;
Character player; 
Character powerUp;
Character gun;
Character missile;
Character* ghosts[GHOSTS_NUMBER];
Character* bombs[BOMBS_NUMBER];

Character leftEnemyMissile;
Character rightEnemyMissile;

char strategyArray[GHOSTS_NUMBER];
	
void initializeCharacters(void)
{
	unsigned char i;
	for(i=0;i<GHOSTS_NUMBER;++i)
	{	
		ghosts[i] = (Character *) malloc(sizeof(Character));
	}
	for(i=0;i<BOMBS_NUMBER;++i)
	{
		bombs[i] = (Character *) malloc(sizeof(Character));
	}
}

void handle_missile()
{
	// Check if player has fired the gun
	if(playerFire && missile._status==0 && guns>0)
	{
		SHOOT_SOUND();
		--guns;
		missileDirection = playerDirection;
		missile._status = setMissileInitialPosition(&missile, &player, missileDirection);
		playerFire = 0;
		DRAW_MISSILE(missile._x,missile._y,missile._imagePtr);					
		checkMissileVsGhosts(&missile);
		if(areCharctersAtSamePosition(&missile, &invincibleGhost))
			{
				die(&missile);
				DELETE_MISSILE(missile._x,missile._y,missile._imagePtr);
				restoreMissile(&missile);
				DRAW_INVINCIBLE_GHOST(invincibleGhost._x, invincibleGhost._y, invincibleGhost._imagePtr);
			}		
	}
	
	// Move missile if fired
	if(missile._status==1)
	{
		moveMissile(&missile, missileDirection);
		// TODO: Inefficient
		checkMissileVsGhosts(&missile);
		
		if(areCharctersAtSamePosition(&missile, &invincibleGhost))
		{
			PING_SOUND();
			die(&missile);
			DELETE_MISSILE(missile._x,missile._y,missile._imagePtr);
			restoreMissile(&missile);
			DRAW_INVINCIBLE_GHOST(invincibleGhost._x, invincibleGhost._y, invincibleGhost._imagePtr);
		}
	}
}


void handle_gun_item()
{
	// Manage gun 
	if(gun._status==1)
	{
		if(powerUpReached(&player, &gun))
		{
			ZAP_SOUND();
			DELETE_GUN(gun._x,gun._y,gun._imagePtr);
			DRAW_PLAYER(player._x, player._y, player._imagePtr);
			guns = GUNS_NUMBER;
			points+=GUN_BONUS;
			gun._status = 0;	
			gunCoolDown = gunInitialCoolDown;
		}
		else
		{
			DRAW_GUN(gun._x, gun._y, gun._imagePtr);
		}
	}		
	else if (gunCoolDown == 0)
	{	
		gun._status = 1;
		do
		{
			relocateCharacter(&gun, bombs,4);
		} while(nearInnerWall(&gun));
		DRAW_GUN(gun._x, gun._y, gun._imagePtr);
	}
	else
	{
		--gunCoolDown;
	}				
}


void handle_powerup_item()
{
	// Manage powerUp
	if(powerUp._status == 1)
	{
		if(powerUpReached(&player, &powerUp))
		{
			ZAP_SOUND();
			die(&powerUp);
			DELETE_POWERUP(powerUp._x,powerUp._y,powerUp._imagePtr);
			DRAW_PLAYER(player._x, player._y, player._imagePtr);
			decreaseGhostLevel(); 
			points+=POWER_UP_BONUS;
			//powerUp._status = 0;	
			powerUpCoolDown = powerUpInitialCoolDown;
		}
		else
		{
			DRAW_POWERUP(powerUp._x,powerUp._y,powerUp._imagePtr);
		}		
	}
	else if (powerUpCoolDown == 0)
	{
		powerUp._status = 1;
		do
		{
			relocateCharacter(&powerUp, bombs,4);
		} while(nearInnerWall(&powerUp));
		DRAW_POWERUP(powerUp._x,powerUp._y,powerUp._imagePtr);
	}
	else
	{
		--powerUpCoolDown;
	}
}


void handle_invincible_ghost(void)
{
	// Manage invincible ghost
	if(!invincibleGhost._status && 
	  ((invincibleXCountDown==0)||(invincibleYCountDown==0)) || 
	   (loop>=invincibleLoopTrigger) || (ghostCount<=invincibleGhostCountTrigger))
	{
		invincibleGhost._status = 1;
		DRAW_INVINCIBLE_GHOST(invincibleGhost._x, invincibleGhost._y, invincibleGhost._imagePtr);
	}
	else
	{
		--invincibleXCountDown;
		--invincibleYCountDown;
	}
	if(invincibleGhost._status)
	{ 	
		invincibleSlowDown = computeInvincibleSlowDown();

		if(rand()>invincibleSlowDown)
		{
			TOCK_SOUND();
			DELETE_INVINCIBLE_GHOST(invincibleGhost._x,invincibleGhost._y,invincibleGhost.imagePtr);
			moveTowardCharacter(&invincibleGhost, &player, 4);
			DRAW_INVINCIBLE_GHOST(invincibleGhost._x, invincibleGhost._y, invincibleGhost._imagePtr);
		}
		if(areCharctersAtSamePosition(&invincibleGhost, &player))
		{
			EXPLOSION_SOUND();
			die(&player);
			//DELETE_PLAYER(player._x,player._y,player._imagePtr);
			printDefeatMessage();
			sleep(1);
		}
	}
}


void handle_player_vs_outer_wall(void)
{
	// Check collision player vs outer wall
	if(wallReached(&player))
	{
		EXPLOSION_SOUND();
		die(&player);
		//DELETE_PLAYER(player._x,player._y,player._imagePtr);
		DRAW_BROKEN_WALL(player._x,player._y);
		printDefeatMessage();
		sleep(1);
	}
}


void handle_player_vs_inner_wall(void)
{
	// Check collistion player vs inner wall
	if(innerWallReached(&player))
	{
		EXPLOSION_SOUND();
		die(&player);
		//DELETE_PLAYER(player._x,player._y,player._imagePtr);
		DRAW_BROKEN_WALL(player._x,player._y);		
		printDefeatMessage();
		sleep(1);
	}
}

void handle_player_vs_bombs_and_ghosts(void)
{
	// Check collision player vs ghosts and player vs bombs
	if(playerReached(&player) ||
	   playerReachedBombs(&player))
	{
		EXPLOSION_SOUND();
		die(&player);
		//DELETE_PLAYER(player._x,player._y,player._imagePtr);
		printDefeatMessage();
		sleep(1);
	}	
}
			
void initialScreen(void)
{
	// Set Screen Colors
	setScreenColors();			
	CLEAR_SCREEN();					
	printStartMessage();

}
		
int main(void)
{		
	INIT_INPUT();
	
	// Ask for the screen size 
	GET_SCREEN_SIZE(&XSize, &YSize);
	
	initializeCharacters();
	
	while(1)
	{
		INIT_IMAGES();
		INIT_GRAPHICS();
		
		CLEAR_SCREEN();
		
		#ifdef DEBUG_CHARACTERS
			printf("ghost %c\n", GHOST_IMAGE._imageData);
			printf("invincible %c\n", INVINCIBLE_GHOST_IMAGE._imageData);
			printf("bomb %c\n", BOMB_IMAGE._imageData);
			printf("player %c\n", PLAYER_IMAGE._imageData);
			printf("powerup %c\n", POWERUP_IMAGE._imageData);
			printf("gun %c\n", GUN_IMAGE._imageData);
			printf("missile %c\n", MISSILE_IMAGE._imageData);
		#endif
		
		initialScreen();
		WAIT_PRESS()
		CLEAR_SCREEN();
		deleteCenteredMessage();

		points = 0ul;
		level = INITIAL_LEVEL; 	
		lives = LIVES_NUMBER;
		do // Level (Re-)Start
		{ 
			computeStrategy();
			loop = 0;
			ghostLevel = 0u;
			ghostCount = GHOSTS_NUMBER;
			guns = 0;
			gun._status = 0;
			
			computePowerUp(&ghostLevelDecrease, &powerUpInitialCoolDown);
			gunCoolDown = computeGunInitialCoolDown();
			
			computeInvincibleGhostParameters();

			ghostSlowDown = computeGhostSlowDown();
			
			// Clear the screen, put cursor in upper left corner
			CLEAR_SCREEN();

			printLevel();
			sleep(1);
			
			// Wait for the user to press a key 
			printPressKeyToStart();
			WAIT_PRESS();

			deleteCenteredMessage();
			
			// Draw a border around the screen 
			drawBorders();
			
			// Initialize characters
			updateInnerWallVerticalData();	
			
			fillLevelWithCharacters();	
			
			displayStatsTitles();
			
			rightEnemyMissile._x = XSize-4; rightEnemyMissile._y = 4;
			leftEnemyMissile._x = 4; leftEnemyMissile._y = YSize-4;
			
			while(player._status && ghostCount>0) // while alive && there are still ghosts
			{
				++loop;
				
				ghostSlowDown = computeGhostSlowDown();
				
				drawInnerVerticalWall();

				if(level==5 || level == 10 || level == 15 || level >= 18)
				{
					DELETE_MISSILE(leftEnemyMissile._x,leftEnemyMissile._y,leftEnemyMissile._imagePtr);
					if(leftEnemyMissile._x==XSize-2)
						leftEnemyMissile._x=2;
					else
						++leftEnemyMissile;
					DRAW_MISSILE(leftEnemyMissile._x,leftEnemyMissile._y,leftEnemyMissile._imagePtr);
					if(areCharctersAtSamePosition(&leftEnemyMissile,&player))
					{
						die(&player);
						printDefeatMessage();
						sleep(1);
					}
					
					DELETE_MISSILE(rightEnemyMissile._x,rightEnemyMissile._y,rightEnemyMissile._imagePtr);
					if(rightEnemyMissile._x==2)
						rightEnemyMissile._x=XSize-2;
					else
						--rightEnemyMissile;
					DRAW_MISSILE(rightEnemyMissile._x,rightEnemyMissile._y,rightEnemyMissile._imagePtr);				
					if(areCharctersAtSamePosition(&rightEnemyMissile,&player))
					{
						die(&player);
						printDefeatMessage();
						sleep(1);
					}
				}
				
				MOVE_PLAYER();
				
				handle_missile();
			
				// Chase the player
				chasePlayer(ghostSlowDown);
				
				// TODO: this should detect collisions of ghosts that have just moved
				if(missile._status)
				{
					checkMissileVsGhosts(&missile);
				}
				
				handle_player_vs_bombs_and_ghosts();
				
				handle_player_vs_inner_wall();
			
				// Check collisions bombs vs ghosts
				checkBombsVsGhosts();
				
				// Check collisions ghosts vs ghosts
			    checkGhostsVsGhosts();
				
				handle_gun_item();
				
				handle_powerup_item();
					
				handle_player_vs_outer_wall();
				
				DRAW_BOMBS();
				
				// Display ghosts
				displayGhosts();

				// Display stats and their values
				// TODO: This should be handled elsewhere
				#if defined(__ATMOS__)
				#else
					displayStatsTitles();
				#endif
				displayStats();
				
				handle_invincible_ghost();
				
				// Add points to score
				points+=LOOP_POINTS;
				
				// Increase ghost speed
				++ghostLevel;
			}; // end inner while [while (player._alive && ghostCount>0), i.e., exit on death or end of level]

			if(player._status) // if level finished
			{
				sleep(1);
				printVictoryMessage();
				sleep(1);
				CLEAR_SCREEN();
				printLevelBonus();
								
				sleep(1);
				
				points+= LEVEL_BONUS*level;
				++level;
				
				if((level==6) || (level==11) || (level==16))
				{	
					PING_SOUND();
					CLEAR_SCREEN();
					sleep(1);
					printExtraLife();
					sleep(1);
					++lives;
				}
				updateInnerWallVerticalData();
			}
			else // if dead
			{
				CLEAR_SCREEN();
				--lives;
				if(lives>0)
				{
					player._status = 1;
				}
			}
		} while (player._status && (level<(FINAL_LEVEL+1))); // lives left and not completed game game 
			
	if(level==FINAL_LEVEL+1) // if completed game
	{
		gameCompleted();
		sleep(1);
	}
	// GAME OVER	
	printGameOver();
	sleep(1);
	finalScore();
	sleep(2);
	CLEAR_SCREEN();
	} // while(1) -> restart from the beginning

	return EXIT_SUCCESS;
}
