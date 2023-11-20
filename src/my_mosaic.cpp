


/* @TODO: 
   collision for goblins
   entrances and exits
   combat
   items
   NPC interactions
   Cleanly separate level definition from level state
   Entities that arent just 1 tiles

   Switch cooridnate system to be bottom left ugh

   Draw sprites centered on a position but that seems tricky when our sprites are even dimensions. Oh well as long as our center position is fractional that's fine. 
   
*/

int32 const TileWidth = 16;
int32 const TileHeight = 16;

int32 Columns = 256 / TileWidth;
int32 Rows = 256 / TileHeight;


enum Direction {
                East,
                North,
                West,
                South,
                Direction_Count,
};


struct ZSprite {
    int32 width;
    int32 height;
    vec4 *colors;
};


vec2 DirectionToVector(Direction dir) {
    switch (dir) {
        default : { assert(false); } break;
        case East : { return V2(1, 0); } break;
        case North : { return V2(0, -1); } break;
        case West : { return V2(-1, 0); } break;
        case South : { return V2(0, 1); } break;
    }
}

Direction VectorToDirection(vec2 v) {
    if (v.x > 0) {
        return East;
    }
    if (v.x < 0) {
        return West;
    }
    if (v.y < 0) {
        return North;
    }
    if (v.y > 0) {
        return South;
    }

    assert(false);
}

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

struct Arrow : Entity {
    vec2 velocity;
    Direction facingDir;
};

struct Rock : Entity {
    
};

struct Tree : Entity {
    
};

struct Grass : Entity {
    
};
    
struct CollisionEvent {
    Entity *a;
    Entity *b;
};

struct Physics {
    DynamicArray<CollisionEvent> events;
};

struct EntityManager {
    DynamicArray<Arrow> arrows;

    DynamicArray<Grass> grass;
    DynamicArray<Rock> rocks;
    DynamicArray<Tree> trees;
};

struct GameMem {
    MemoryArena arena;
    MemoryArena frameMem;

    EntityManager entityManager;

    Player player;

    Rock rock;

    ZSprite heroSprite;
    ZSprite arrowSprites[Direction_Count];

    ZSprite treeSprite;
    ZSprite grassSprite;
    ZSprite rockSprite;
    ZSprite poolSprite;
};

GameMem GM = {};


void SpawnRock(EntityManager *em, vec2 position) {
    Rock rock = {};
    rock.position = position;
    rock.min = V2(1, 8);
    rock.max = V2(15, 16);
    PushBack(&em->rocks, rock);
}

void SpawnTree(EntityManager *em, vec2 position) {
    Tree tree = {};
    tree.position = position;
    tree.min = V2(0, 0);
    tree.max = V2(32, 32);
    PushBack(&em->trees, tree);
}

void SpawnGrass(EntityManager *em, vec2 position) {
    Grass grass = {};
    grass.position = position;
    PushBack(&em->grass, grass);
}

void SpawnArrow(EntityManager *em, vec2 position, vec2 vel) {
    Arrow arrow = {};
    arrow.facingDir = VectorToDirection(vel);
    arrow.position = position;
    arrow.velocity = vel;

    if (arrow.facingDir == North ||
        arrow.facingDir == South) {
        arrow.min = V2(6, 2);
        arrow.max = V2(10, 15);
    }
    else {
        arrow.min = V2(2, 6);
        arrow.max = V2(15, 10);
    }

    PushBack(&em->arrows, arrow);
}


void MyMosaicInit() {
    SetMosaicGridSize(256, 256);

    AllocateMemoryArena(&GM.arena, Megabytes(64));
    AllocateMemoryArena(&GM.frameMem, Megabytes(16));

    GM.entityManager.arrows = MakeDynamicArray<Arrow>(&GM.arena, 32);
    GM.entityManager.trees = MakeDynamicArray<Tree>(&GM.arena, 64);
    GM.entityManager.grass = MakeDynamicArray<Grass>(&GM.arena, 256);
    GM.entityManager.rocks = MakeDynamicArray<Rock>(&GM.arena, 32);

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
    LoadZSpriteFromFile("data/sprites/arrow_east.png", &GM.arrowSprites[East], &GM.arena);
    LoadZSpriteFromFile("data/sprites/arrow_north.png", &GM.arrowSprites[North], &GM.arena);
    LoadZSpriteFromFile("data/sprites/arrow_west.png", &GM.arrowSprites[West], &GM.arena);
    LoadZSpriteFromFile("data/sprites/arrow_south.png", &GM.arrowSprites[South], &GM.arena);

    EntityManager *em = &GM.entityManager;
    
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
        SpawnRock(em, TilePositionToPixel(12, 9));
    }

    for (int y = 0; y < Rows; y++) {
        for (int x = 0; x < Columns; x++) {
            SpawnGrass(em, TilePositionToPixel(x, y));
        }
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 10; x++) {
            SpawnTree(em, TilePositionToPixel(x * 2, y * 2));
        }
    }
}

void DrawCollider_AABB(vec2 position, vec2 min, vec2 max, vec4 color) {
    vec2 dim = max - min;

    for (int y = 0; y < dim.y; y++) {
        for (int x = 0; x < dim.x; x++) {
            if ((y > 0 && y < dim.y - 1) && (x > 0 && x < dim.x - 1)) {
                continue;
            }
            
            SetTileColor(position + V2(x, y) + min, color);
        }
    }
}

void UpdatePlayer(Player *player) {
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

    if (moveDir.x > 0) {
        player->facingDir = East;
    }
    if (moveDir.x < 0) {
        player->facingDir = West;
    }

    if (moveDir.y > 0) {
        player->facingDir = South;
    }
    if (moveDir.y < 0) {
        player->facingDir = North;
    }

    moveDir = Normalize(moveDir);

    if (Abs(moveDir.x) > 0 || Abs(moveDir.y) > 0) {
        player->moveTimer += DeltaTime;
    }

    player->position = player->position + (moveDir * 70 * DeltaTime);

    if (InputPressed(Keyboard, Input_C)) {
        vec2 vel = DirectionToVector(player->facingDir) * 180;
        SpawnArrow(&GM.entityManager, player->position, vel);
    }
}

void UpdateArrows(EntityManager *em) {
    for (int i = 0; i < em->arrows.count; i++) {
        Arrow *arrow = &em->arrows[i];
        arrow->position = arrow->position + arrow->velocity * DeltaTime;
    }
}

void DetectCollisions() {
    EntityManager *em = &GM.entityManager;
    
    Player *player = &GM.player;
    Rock *rock = &GM.rock;

    vec2 playerCenterWorld = player->position + player->center;
    
    vec2 playerMinWorld = player->position + player->min;
    vec2 playerMaxWorld = player->position + player->max;

    for (int i = 0; i < em->rocks.count; i++) {
        Rock *rock = &em->rocks[i];

        vec2 rockMinWorld = rock->position + rock->min;
        vec2 rockMaxWorld = rock->position + rock->max;

        vec2 dir = V2(0);
        if (TestCircleAABB(playerCenterWorld, player->radius, rockMinWorld, rockMaxWorld, &dir)) {
            player->position = player->position + dir;
        }
    }

    for (int i = 0; i < em->trees.count; i++) {
        Tree *tree = &em->trees[i];

        vec2 treeMinWorld = tree->position + tree->min;
        vec2 treeMaxWorld = tree->position + tree->max;

        vec2 dir = V2(0);
        if (TestCircleAABB(playerCenterWorld, player->radius, treeMinWorld, treeMaxWorld, &dir)) {
            player->position = player->position + dir;
        }
    }
}


void MyMosaicUpdate() {
    //ClearTiles(V4(0.08f, 0.5f, 0.17f, 1.0f));

    EntityManager *em = &GM.entityManager;

    UpdatePlayer(&GM.player);

    UpdateArrows(&GM.entityManager);

    DetectCollisions();

    for (int i = 0; i < em->grass.count; i++) {
        Grass *grass = &em->grass[i];
        DrawSprite(grass->position, &GM.grassSprite);
    }

    DrawSprite(GM.player.position, &GM.heroSprite);

    // DrawSprite(TilePositionToPixel(12, 9), &GM.rockSprite);
    // DrawSprite(TilePositionToPixel(8, 12), &GM.rockSprite);
    // DrawSprite(TilePositionToPixel(9, 12), &GM.rockSprite);

    DrawSprite(TilePositionToPixel(3, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(4, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(5, 14), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(4, 13), &GM.poolSprite);
    DrawSprite(TilePositionToPixel(5, 13), &GM.poolSprite); 

    // for (int y = 0; y < 4; y++) {
    //     for (int x = 0; x < 10; x++) {
    //         DrawSprite(TilePositionToPixel(x * 2, y * 2), &GM.treeSprite);
    //     }
    // }

    for (int i = 0; i < em->trees.count; i++) {
        Tree *tree = &em->trees[i];
        DrawSprite(tree->position, &GM.treeSprite);
    }

    for (int i = 0; i < em->rocks.count; i++) {
        Rock *rock = &em->rocks[i];
        DrawSprite(rock->position, &GM.rockSprite);
    }
    
    for (int i = 0; i < em->arrows.count; i++) {
        Arrow *arrow = &em->arrows[i];
        DrawSprite(arrow->position, &GM.arrowSprites[arrow->facingDir]);
        //DrawCollider_AABB(arrow->position, arrow->min, arrow->max, V4(0.5f, 0.0f, 0.0f, 1.0f));
    }

    // Debug drawing
    // @TODO: let's have some calls to defer these render commands from gameplay code
    //DrawCollider_AABB(GM.rock.position, GM.rock.min, GM.rock.max, V4(0.5f, 0.0f, 0.0f, 1.0f));
}

