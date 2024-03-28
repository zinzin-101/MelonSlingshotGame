#include "game.h"


// -------------------------------------------
// Level variable, static - visible only in this file
// -------------------------------------------

static sf::Sprite	*sSpriteArray;							// Store all unique sprite in your game
static sf::Texture	*sTexArray;								// Corresponding texture of the sprite
static GameObj		sGameObjInstArray[GAME_OBJ_INST_MAX];	// Store all game object instance
static int			sNumGameObj;
static int			sNumSprite;
static int			sNumTex;

static GameObj*		sPlayer;										// Pointer to the Player game object instance

static glm::vec3 prevMousePos;
static bool isMouseDown;
static int spawnMode;

// View
sf::View view;


// -------------------------------------------
// Game object instant functions
// -------------------------------------------

GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,bool mortal, 
	int life, bool anim, int numFrame, int currFrame, int frameSizeX)
{
	// loop through all object instance array to find the free slot
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;
		if (pInst->flag == FLAG_INACTIVE) {

			pInst->type = type;
			pInst->flag = FLAG_ACTIVE;
			pInst->position = pos;
			pInst->velocity = vel;
			pInst->scale = scale;
			pInst->orientation = orient;
			pInst->modelMatrix = glm::mat4(1.0f);
			pInst->mortal = mortal;
			pInst->lifespan = -1;
			pInst->anim = anim;
			pInst->numFrame = numFrame;
			pInst->currFrame = currFrame;
			pInst->frameSizeX = frameSizeX;
			pInst->currOffset = currFrame * frameSizeX;
			pInst->target = -1;
			pInst->targetPos = glm::vec3();
			pInst->enablePhysics = true;
			pInst->health = -1;

			if (life != 0) {
				pInst->lifespan = life;
			}

			if (pInst->type == TYPE_BOX) {
				pInst->health = 3;
			}

			sNumGameObj++;
			return pInst;
		}
	}

	// Cannot find empty slot => return 0
	return NULL;
}

void gameObjInstDestroy(GameObj& pInst)
{
	// Lazy deletion, not really delete the object, just set it as inactive
	if (pInst.flag == FLAG_INACTIVE)
		return;

	sNumGameObj--;
	pInst.flag = FLAG_INACTIVE;
}




void GameInit() {
	srand(time(NULL));

	// clear the Sprite array
	sNumSprite = 0;

	// clear the Texture array
	sNumTex = 0;

	// clear the game object instance array
	memset(sGameObjInstArray, 0, sizeof(GameObj) * GAME_OBJ_INST_MAX);
	sNumGameObj = 0;

	// Set the ship object instance to NULL
	sPlayer = NULL;

	// --------------------------------------------------------------------------
	// Create all of the unique sprites/textures and put them in SpriteArray/TexArray
	//		- The order of sprite should follow enum GAMEOBJ_TYPE 
	/// --------------------------------------------------------------------------


	// Create Sprite/texture
	sSpriteArray = new sf::Sprite[MESH_MAX];
	sTexArray = new sf::Texture[TEXTURE_MAX];
	
	sTexArray[sNumTex].loadFromFile("asset\\summer-background.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\object.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\square.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	// Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	gameObjInstCreate(TYPE_BACKGROUND, glm::vec3(window.getSize().x/2, window.getSize().y/2, 0.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(window.getSize().x / 2160.0, window.getSize().y / 1215.0, 1.0f),
		0, false, 0, false, 0, 0, 0);

	/*for (int i = 0; i < 10; i++) {
		int rand_s = rand();
		gameObjInstCreate(TYPE_OBJECT, glm::vec3(rand() % window.getSize().x, rand() % (window.getSize().y - 300), 0.0f),
			glm::vec3(50.0 * ((2 * (float)rand() / (float)(RAND_MAX)) - 1), 0.0f, 0.0f),
			glm::vec3(100.0f / 1000, 100.0f / 1000, 1.0f), 0.0f, false, 0, false, 0, 0, 0);
	}*/


	for (int i = 0; i < 30; i++) {
		int rand_s = rand();
		gameObjInstCreate(TYPE_BOX, glm::vec3((window.getSize().x / 3.0) + (125 * (i % 5)), (window.getSize().y / 2.0) + (125 * (i / 5)), 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(100.0f / 1000, 100.0f / 1000, 1.0f), 0.0f, false, 0, false, 0, 0, 0);
	}


	// set view
	//view.setCenter(window.getSize().x/2, window.getSize().y/2);
	view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view.setCenter(window.getSize().x / 2.0, window.getSize().y / 2.0);

	view.setRotation(0.0f);

	std::cout << "game init" << std::endl;
}



void GameUpdate(double dt, long frame, int &state) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------

	static bool isSelected = false;
	static int objIndex = -1;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//isSelected = false;
		//objIndex = -1;
		
		sf::Vector2i position = sf::Mouse::getPosition(window);

		GameObj* pInst;

		switch (isSelected) {
			case true:
				pInst = sGameObjInstArray + objIndex;
				pInst->enablePhysics = false;
				pInst->position = glm::vec3(position.x, position.y, 0);

				if (glm::length(pInst->position - glm::vec3(position.x, position.y, 0)) > 200.0) {
					pInst->enablePhysics = true;
					isSelected = false;
					objIndex = -1;
				}
				break;

			case false:
				if (objIndex == -1) {
					for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
						pInst = sGameObjInstArray + i;
						if (pInst->type == TYPE_OBJECT) {
							pInst->enablePhysics = true;
							if (glm::length(pInst->position - glm::vec3(position.x, position.y, 0)) < DEFAULT_RADIUS) {
								isSelected = true;
								objIndex = i;
								break;
							}
						}
					}
				}
				break;
		}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		GameObj* pInst;
		sf::Vector2i position = sf::Mouse::getPosition(window);
		if (objIndex != -1) {
			pInst = sGameObjInstArray + objIndex;
			pInst->velocity = (glm::vec3(position.x, position.y, 0.0) - prevMousePos) * (0.005f / (float)dt);
		}

		objIndex = -1;
		isSelected = false;

		for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
			pInst = sGameObjInstArray + i;
			if (pInst->type == TYPE_OBJECT) {
				pInst->enablePhysics = true;
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !isMouseDown){
		isMouseDown = true;
		sf::Vector2i position = sf::Mouse::getPosition(window);
		//std::cout << "x: " << position.x << ", y: " << position.y << std::endl;

		switch (spawnMode){
			case SPAWN_BALL:
				gameObjInstCreate(TYPE_OBJECT, glm::vec3(position.x, position.y, 0.0f),
					glm::vec3(0, 0.0f, 0.0f),
					glm::vec3(100.0f / 1000, 100.0f / 1000, 1.0f), 0.0f, false, 60, false, 0, 0, 0);
				break;

			case SPAWN_BOX:
				gameObjInstCreate(TYPE_BOX, glm::vec3(position.x, position.y, 0.0f),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(100.0f / 1000, 100.0f / 1000, 1.0f), 0.0f, false, 0, false, 0, 0, 0);
				break;
		}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		isMouseDown = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
		spawnMode = SPAWN_BALL;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
		spawnMode = SPAWN_BOX;
	}

	//zoom-UO
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
		view.zoom(0.99f); 
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		view.zoom(1 / 0.99f);
	}

	sf::Vector2i tempMousePos = sf::Mouse::getPosition(window);
	prevMousePos = glm::vec3(tempMousePos.x, tempMousePos.y, 0);
	//-----------------------------------------
	// Decrease object lifespan
	//-----------------------------------------

	if (frame % 3 == 0) {
		
		//++
		for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
			GameObj* pInst = sGameObjInstArray + i;
			
			if (pInst->type == TYPE_OBJECT) {
				if (glm::length(pInst->velocity) < 0.5 && pInst->enablePhysics) {
					pInst->lifespan--;
				}
				else {
					pInst->lifespan = 60;
				}
			}
			else if (pInst->type == TYPE_BOX) {
				pInst->lifespan--;
				if (pInst->lifespan <= 0) {
					pInst->enablePhysics = true;
				}
			}
			else {
				if (pInst->lifespan > 0) {
					pInst->lifespan--;
				}

				if (pInst->lifespan == 0) {
					gameObjInstDestroy(*pInst);
				}
			}
		}

	}
	
	//---------------------------------------------------------
	// Update all game obj position using velocity 
	//---------------------------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if (pInst->type == TYPE_OBJECT) {
			if (pInst->enablePhysics) {
				if (pInst->position.y < window.getSize().y) {
					pInst->velocity.y = pInst->velocity.y + (DEFAULT_GRAVITY * dt);
				}
				else {
					pInst->position.y = window.getSize().y;

					if (pInst->velocity.y > 0.0) {
						pInst->velocity = glm::vec3(pInst->velocity.x, -glm::abs(pInst->velocity.y) * 0.75, 0);
					}
					else if (pInst->velocity.y < 0.0) {
						pInst->velocity = glm::vec3(pInst->velocity.x, glm::abs(pInst->velocity.y) * 0.75, 0);
					}
				}

				if (pInst->position.x < 0.0) {
					pInst->velocity = glm::vec3(glm::abs(pInst->velocity.x) * 0.75, pInst->velocity.y, 0);
				}

				if (pInst->position.x > window.getSize().x) {
					pInst->velocity = glm::vec3(-glm::abs(pInst->velocity.x) * 0.75, pInst->velocity.y, 0);
				}

			}
			else {
				pInst->velocity = glm::vec3(0, 0, 0);
			}

			pInst->position += pInst->velocity;
		}

		if (pInst->type == TYPE_BOX) {
			if (pInst->enablePhysics) {
				if (pInst->position.y < window.getSize().y) {
					pInst->velocity.y = pInst->velocity.y + (DEFAULT_GRAVITY * dt);
				}
				else {
					pInst->position.y = window.getSize().y;
				}
			}
			else {
				pInst->velocity = glm::vec3(0, 0, 0);
			}

			pInst->position += pInst->velocity;
		}
	}

	//----------------------------------------------------
	// Update animation for animated object every n frame
	//----------------------------------------------------

	if (frame % 3 == 0) {

		//++
		for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
			GameObj* pInst = sGameObjInstArray + i;

			if (!pInst->anim) {
				continue;
			}

			if (pInst->currFrame <= pInst->numFrame) {
				pInst->currOffset += pInst->frameSizeX;
			}
			else {
				pInst->currOffset = 0;
			}
		}

	}

	//-----------------------------------------
	// Update some game obj behavior
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;
	}

	//-----------------------------------------
	// Check for collsion, O(n^2)
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst1 = sGameObjInstArray + i;

		// skip inactive object
		if (pInst1->flag == FLAG_INACTIVE)
			continue;

		if (pInst1->type == TYPE_OBJECT) {
			for (int j = 0; j < GAME_OBJ_INST_MAX; j++) {
				GameObj* pInst2 = sGameObjInstArray + j;

				if (i == j) {
					continue;
				}

				if (pInst2->flag == FLAG_INACTIVE) {
					continue;
				}

				/*if (pInst2->type == TYPE_OBJECT) {
					float distance = glm::length(pInst1->position - pInst2->position);
					if (distance <= 50.0) {
						float tempX = pInst1->velocity.x;
						float tempY = pInst1->velocity.y;
						pInst1->velocity.x = -pInst2->velocity.x * 0.9;
						pInst1->velocity.y = -pInst2->velocity.y;
						pInst2->velocity.x = -tempX * 0.9;
						pInst2->velocity.y = -tempY;
					}*/
			}
		
			if (glm::length(pInst1->velocity) < 0.5 && pInst1->lifespan <= 0) {
				gameObjInstDestroy(*pInst1);
			}
		}

		if (pInst1->type == TYPE_BOX) {
			for (int j = 0; j < GAME_OBJ_INST_MAX; j++) {
				GameObj* pInst2 = sGameObjInstArray + j;

				if (i == j) {
					continue;
				}

				if (pInst2->flag == FLAG_INACTIVE) {
					continue;
				}

				if (pInst2->type == TYPE_OBJECT) {
					int collisionType;
					if (checkCollisionSquareCircle(pInst1->position, pInst2->position, 40, collisionType) && pInst2->enablePhysics) {
						if (collisionType == COL_TOP || collisionType == COL_BOTTOM) {
							//std::cout << "top" << std::endl;
							pInst2->velocity.y = -pInst2->velocity.y;
						}
						else if (collisionType == COL_SIDE) {
							//std::cout << "side" << std::endl;
							pInst2->velocity.x = -pInst2->velocity.x;
						}
						pInst1->health--;

						if (pInst1->health <= 0) {
							gameObjInstDestroy(*pInst1);
						}
					}
				}

				if (pInst2->type == TYPE_BOX) {
					int collisionType;
					if (checkCollisionSquareSquare(pInst1->position, pInst2->position, 60, collisionType)) {
						//std::cout << "collided" << std::endl;
						if (collisionType == COL_BOTTOM) {
							pInst1->enablePhysics = false;
							pInst1->lifespan = 3;
						}
					}
				}
			}
		}
	}


	double fps = 1.0 / dt;
	//printf("Level1: Update @> %f fps, frame>%ld\n", fps, frame);
	//printf("Score> %i\n", sScore);
	//printf("num obj> %i\n", sNumGameObj);
}


void GameDraw(double dt) {

	window.setView(view);

	// clear framebuffer
	window.clear(sf::Color::Black);

	// draw all game object instance in the sGameObjInstArray
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if (pInst->numFrame > 0) {
			int w = sSpriteArray[pInst->type].getTextureRect().width;
			int h = sSpriteArray[pInst->type].getTextureRect().height;
			sSpriteArray[pInst->type].setTextureRect(sf::IntRect(pInst->currOffset, 0, w,h));
		}

		sSpriteArray[pInst->type].setPosition(sf::Vector2f(pInst->position.x, pInst->position.y));
		sSpriteArray[pInst->type].setScale(sf::Vector2f(pInst->scale.x, pInst->scale.y));
		sSpriteArray[pInst->type].setRotation(glm::degrees(pInst->orientation));
		
		window.draw(sSpriteArray[pInst->type]);
		
		
	}


	// swap framebuffer
	window.display();


	}

void GameClean() {


	// call gameObjInstDestroy for all object instances in the sGameObjInstArray
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		gameObjInstDestroy(sGameObjInstArray[i]);
	}

	delete[] sTexArray;
	delete[] sSpriteArray;

	std::cout << "game clean" << std::endl;
	

}

bool checkCollisionSquareCircle(glm::vec3 square, glm::vec3 circle, float halfWidth, int &collisionType) {
	float diffX = circle.x - square.x;
	float diffY = circle.y - square.y;

		// Calculate the minimum distance to separate along X and Y
	float minXDist = DEFAULT_RADIUS + halfWidth;
	float minYDist = DEFAULT_RADIUS + halfWidth;

		// Calculate the depth of collision for both the X and Y axis
	float depthX = diffX > 0 ? minXDist - diffX : -minXDist - diffX;
	float depthY = diffY > 0 ? minYDist - diffY : -minYDist - diffY;

		// Now that you have the depth, you can pick the smaller depth and move
		// along that axis.
	if (depthX != 0 && depthY != 0) {
		if (abs(depthX) < abs(depthY)) {
			// Collision along the X axis. React accordingly
			if (depthX > 0) {
			// Left side collision
				collisionType = COL_SIDE;
			}
			else {
				// Right side collision
				collisionType = COL_SIDE;
			}
		}
		else {
			// Collision along the Y axis.
			if (depthY > 0) {
				// Top side collision
				collisionType = COL_TOP;
			}
			else {
				// Bottom side collision
				collisionType = COL_BOTTOM;
			}
		}
	}
	else {
		collisionType = COL_TOP;
	}

	float circleDistanceX = abs(circle.x - square.x);
	float circleDistanceY = abs(circle.y - square.y);

	if (circleDistanceX > (halfWidth + DEFAULT_RADIUS)) {
		collisionType = COL_NONE;
		return false;
	}
	if (circleDistanceY > (halfWidth + DEFAULT_RADIUS)) {
		collisionType = COL_NONE;
		return false;
	}

	if (circleDistanceX <= halfWidth || circleDistanceY <= halfWidth) {
		return true;
	}

	float cornerDistance_sq = pow((circleDistanceX - halfWidth), 2) +
		pow((circleDistanceY - halfWidth), 2);

	return (cornerDistance_sq <= (pow(DEFAULT_RADIUS, 2)));
}

bool checkCollisionSquareSquare(glm::vec3 s1, glm::vec3 s2, float halfWidth, int& collisionType) {
	if (s1.x - halfWidth > s2.x + halfWidth) {
		return false;
	}

	if (s2.x - halfWidth > s1.x + halfWidth) {
		return false;
	}

	if (s1.y + halfWidth < s2.y - halfWidth) {
		return false;
	}

	if (s2.y + halfWidth < s1.y - halfWidth) {
		return false;
	}

	if (s1.y < s2.y) {
		collisionType = COL_BOTTOM;
	}
	else {
		collisionType = COL_TOP;
	}

	return true;
}

float dotAngle(glm::vec3 v1, glm::vec3 v2) {
	return acosf(glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2)));
}