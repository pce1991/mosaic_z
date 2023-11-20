


/* @TODO: 
   collision for goblins
   entrances and exits
   combat
   items
   NPC interactions
   Cleanly separate level definition from level state
   Entities that arent just 1 tiles

   Switch cooridnate system to be bottom left ugh

   
*/

int32 const TileWidth = 16;
int32 const TileHeight = 16;

struct ZSprite {
    int32 width;
    int32 height;
    vec4 *colors;
};


vec2 TilePositionToPixel(int32 x, int32 y) {
    return V2(x * TileWidth, y * TileHeight);
}


void DrawSprite(vec2 pos, ZSprite *sprite) {
    int32 index = 0;
    for (int y = 0; y < sprite->height; y++) {
        for (int x = 0; x < sprite->width; x++) {
            if (sprite->colors[index].a == 0) {
                index++;
                continue;
            }
            SetTileColor(pos.x + x, pos.y + y, sprite->colors[index]);
            index++;
        }
    }
}

void AllocateZSprite(MemoryArena *arena, ZSprite *sprite, int32 width, int32 height) {
    sprite->width = width;
    sprite->height = height;

    sprite->colors = PushArray(arena, vec4, sprite->width * sprite->height);
}

void LoadZSpriteFromFile(char *path, ZSprite *sprite, MemoryArena *arena) {
    int32 x, y, components;
    uint8 *data = stbi_load(path, &x, &y, &components, 0);
    // assuming 4 color channels

    if (data == NULL) {
        assert(false);
    }

    AllocateZSprite(arena, sprite, x, y);
    
    int32 colorIndex = 0;
    int32 dataLength = x * y * components;

    switch (components) {
        default : {assert(false); } break;
        case 1 : {
            for (int i = 0; i < dataLength; i++) {
                sprite->colors[colorIndex++] = V4(data[i] / 255.0f,
                                                  data[i] / 255.0f,
                                                  data[i] / 255.0f,
                                                  data[i] / 255.0f);
            }
        } break;

        case 4 : {
            for (int i = 0; i < dataLength; i += 4) {
                sprite->colors[colorIndex++] = V4(data[i] / 255.0f,
                                                  data[i + 1] / 255.0f,
                                                  data[i + 2] / 255.0f,
                                                  data[i + 3] / 255.0f);
            }
        }
    }


    free(data);
}

enum Direction {
                East,
                North,
                West,
                South,
};

struct Level {
    
};

struct LevelState {
    
};

enum EntityType {
                 EntityType_Entity,
                 EntityType_Player,
                 EntityType_Rock,
};

struct Entity {
    vec2 position;

    vec2 min;
    vec2 max;

    vec2 center;
    real32 radius;
};

struct Player : Entity {
    int32 levelIndex;
    
    int8 health;
    Direction facingDir;

    real32 moveTimer;
};

struct Rock : Entity {
    
};

    
struct CollisionEvent {
    Entity *a;
    Entity *b;
};

struct Physics {
    DynamicArray<CollisionEvent> events;
};

struct GameMem {
    MemoryArena arena;
    MemoryArena frameMem;

    Player player;

    Rock rock;

    ZSprite heroSprite;

    ZSprite treeSprite;
    ZSprite grassSprite;
    ZSprite rockSprite;
    ZSprite poolSprite;
};

GameMem GM = {};

void MyMosaicInit() {
    SetMosaicGridSize(256, 256);

    AllocateMemoryArena(&GM.arena, Megabytes(64));
    AllocateMemoryArena(&GM.frameMem, Megabytes(16));

#if 0
    {
        ZSprite *sprite = &GM.sprite;
        *sprite = {};
        sprite->width = 8;
        sprite->height = 8;

        sprite->colors = PushArray(&GM.arena, vec4, sprite->width * sprite->height);

        for (int y = 0; y < sprite->height; y++) {
            for (int x = 0; x < sprite->width; x++) {
                sprite->colors[x + (y * sprite->height)] = V4(0.1f, 0.7f, 0.3f, 1.0f);
            }
        }
    }
#endif

    LoadZSpriteFromFile("data/sprites/tree_big_2.png", &GM.treeSprite, &GM.arena);
    LoadZSpriteFromFile("data/sprites/grass_2.png", &GM.grassSprite, &GM.arena);
    LoadZSpriteFromFile("data/sprites/rock_1.png", &GM.rockSprite, &GM.arena);
    LoadZSpriteFromFile("data/sprites/pool_1.png", &GM.poolSprite, &GM.arena);

    LoadZSpriteFromFile("data/sprites/hero_down.png", &GM.heroSprite, &GM.arena);


    {
        Player *player = &GM.player;
        player->position = V2(100, 200);
        player->min = V2(0);
        player->max = V2(16, 16);

        player->center = V2(8, 8);
        player->radius = 8;
    }

    {
        Rock *rock = &GM.rock;
        rock->position = TilePositionToPixel(12, 9);
        // rock->min = V2(0, 6);
        // rock->max = V2(14, 16);

        rock->min = V2(1, 8);
        rock->max = V2(15, 16);
    }
}

void DrawCollider_AABB(vec2 position, vec2 min, vec2 max, vec4 color) {
    vec2 dim = max - min;

    for (int y = 0; y < dim.y; y++) {
        for (int x = 0; x < dim.x; x++) {
            SetTileColor(position + V2(x, y) + min, color);
        }
    }
}

void PlayerUpdate(Player *player) {
    vec2 moveDir = V2(0);

    if (InputHeld(Keyboard, Input_LeftArrow)) {
        moveDir.x = -1;
    }
    else if (InputHeld(Keyboard, Input_RightArrow)) {
        moveDir.x = 1;
    }
    if (InputHeld(Keyboard, Input_UpArrow)) {
        moveDir.y = -1;
    }
    else if (InputHeld(Keyboard, Input_DownArrow)) {
        moveDir.y = 1;
    }

    moveDir = Normalize(moveDir);

    if (Abs(moveDir.x) > 0 || Abs(moveDir.y) > 0) {
        player->moveTimer += DeltaTime;
    }

    player->position = player->position + (moveDir * 70 * DeltaTime);
}


void DetectCollisions() {
    Player *player = &GM.player;
    Rock *rock = &GM.rock;

    vec2 playerCenterWorld = player->position + player->center;
    
    vec2 playerMinWorld = player->position + player->min;
    vec2 playerMaxWorld = player->position + player->max;

    vec2 rockMinWorld = rock->position + rock->min;
    vec2 rockMaxWorld = rock->position + rock->max;

    vec2 dir = V2(0);
    // if (TestAABBAABB(playerMinWorld, playerMaxWorld, rockMinWorld, rockMaxWorld, &dir)) {
    //     player->position = player->position + dir;
    // }

    if (TestCircleAABB(playerCenterWorld, player->radius, rockMinWorld, rockMaxWorld, &dir)) {
        TestCircleAABB(playerCenterWorld, player->radius, rockMinWorld, rockMaxWorld, &dir);
        player->position = player->position + dir;
    }
}


void MyMosaicUpdate() {
    //ClearTiles(V4(0.08f, 0.5f, 0.17f, 1.0f));

    int32 Columns = 256 / 16;
    int32 Rows = 256 / 16;

    PlayerUpdate(&GM.player);

    DetectCollisions();

    for (int y = 0; y < Rows; y++) {
        for (int x = 0; x < Columns; x++) {
            DrawSprite(TilePositionToPixel(x, y), &GM.grassSprite);        
        }
    }

    DrawSprite(GM.player.position, &GM.heroSprite);

    // DrawSprite(TilePositionToPixel(12, 9), &GM.rockSprite);
    // DrawSprite(TilePositionToPixel(8, 12), &GM.rockSprite);
    // DrawSprite(TilePositionToPixel(9, 12), &GM.rockSprite);

    DrawSprite(GM.rock.position, &GM.rockSprite);

    DrawSprite(TilePositionToPixel(3, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(4, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(5, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(4, 13), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(5, 13), &GM.poolSprite); 

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 10; x++) {
            DrawSprite(TilePositionToPixel(x * 2, y * 2), &GM.treeSprite);
        }
    }

    DrawCollider_AABB(GM.rock.position, GM.rock.min, GM.rock.max, V4(0.5f, 0.0f, 0.0f, 1.0f)); 
}

