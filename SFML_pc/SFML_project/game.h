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
#define DEFAULT_GRAVITY				9.81
#define DEFAULT_RADIUS				25.0f
#define SLINGSHOT_POW				25.0f
#define SLINGSHOT_RADIUS			200.0f


enum GAMEOBJ_TYPE
{
	// list of game object types
	TYPE_BACKGROUND = 0,
	TYPE_OBJECT,
	TYPE_BOX,
	TYPE_SLING,
	TYPE_ENEMY
};

enum COLLSION_SIDE {
	COL_TOP,
	COL_SIDE,
	COL_BOTTOM,
	COL_NONE
};

enum SPAWN_MODE {
	SPAWN_BALL,
	SPAWN_BOX,
	SPAWN_SLING,
	SPAWN_ENEMY
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
	bool			enablePhysics;

	int				health;

	glm::vec3		nextVel;
};



// functions to create/destroy a game object instance
static GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,
								bool mortal, int life, bool anim, int numFrame, int currFrame, int offsetX);
static void		gameObjInstDestroy(GameObj& pInst);

bool checkCollisionSquareCircle(glm::vec3 square, glm::vec3 circle, float halfWidth, int &collisionType);
bool checkCollisionSquareSquare(glm::vec3 s1, glm::vec3 s2, float halfWidth, int &collisionType);
float dotAngle(glm::vec3 v1, glm::vec3 v2);

void GameInit();
void GameUpdate(double dt, long frame, int &state);
void GameDraw(double dt);
void GameClean();
