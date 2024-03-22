#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstring>
#include <math.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>


extern sf::RenderWindow window;

// -------------------------------------------
// Defines
// -------------------------------------------
#define PI							3.1415926
#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			1024			// The total number of different game object instances
#define PLAYER_INITIAL_NUM			20				// initial number of ship lives
#define NUM_ENEMY					12
#define SHIP_ACC_FWD				-250.0f			// ship forward acceleration (in m/s^2)
#define SHIP_ACC_BWD				300.0f			// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(1.75 * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 2.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				-625.0f
#define MISSILE_SPEED               -475.0f
#define ENEMY_SPEED					175.0f	
#define MAX_SHIP_VELOCITY			275.0f
#define BASE_SHIP_VELOCITY          150.0f
#define MIN_SHIP_VELOCITY			75.0f
#define ENEMY_TURN_SPEED			PI
#define DEFAULT_RELOAD_DELAY		0.75f
#define DEFAULT_MISSILE_COUNT		2
#define DEFAULT_CHAFF_FLARE_COUNT	5
#define MAX_BOSS_HEALTH				30
#define MIN_X						-1500
#define MAX_X						3200
#define MIN_Y						-900
#define MAX_Y						1700


enum GAMEOBJ_TYPE
{
	// list of game object types
	TYPE_BACKGROUND = 0,
	TYPE_BULLET,
	TYPE_MISSILE,
	TYPE_SHIP,
	TYPE_ENEMY,
	TYPE_EXPLOSION,
	TYPE_ENEMY_BULLET,
	TYPE_ENEMY_MISSILE,
	TYPE_MISSILE_CONTRAIL,
	TYPE_CHAFF_FLARE,
	TYPE_BOSS,
	TYPE_BOSS_FLARE
};

#define FLAG_INACTIVE		0
#define FLAG_ACTIVE			1

// -------------------------------------------
// Structure definitions
// -------------------------------------------

struct GameObj
{
	int				type;				// enum type
	int				flag;				// 0 - inactive, 1 - active
	glm::vec3		position;			// usually we will use only x and y
	glm::vec3		velocity;			// usually we will use only x and y
	glm::vec3		scale;				// usually we will use only x and y
	float			orientation;		// 0 radians is 3 o'clock, PI/2 radian is 12 o'clock
	glm::mat4		modelMatrix;
	int				target;				// only for Missile game obj, index to sGameObjInstArray
	//
	bool			mortal;
	int				lifespan;			// in frame unit
	bool			anim;				// has animation?
	int				numFrame;			// #frame in texture animation
	int				currFrame;
	int				frameSizeX;			// frame size, assume single row sprite sheet
	int				currOffset;
	glm::vec3		targetPos;

	int				health;
};



// functions to create/destroy a game object instance
static GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,
								bool mortal, int life, bool anim, int numFrame, int currFrame, int offsetX);
static void		gameObjInstDestroy(GameObj& pInst);

float dotAngle(glm::vec3 v1, glm::vec3 v2);

void GameInit();
void GameUpdate(double dt, long frame, int &state);
void GameDraw(double dt);
void GameClean();
