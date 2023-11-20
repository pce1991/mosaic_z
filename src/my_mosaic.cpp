


/* @TODO: 
   collision for goblins
   entrances and exits
   combat
   items
   NPC interactions
   Cleanly separate level definition from level state
   Entities that arent just 1 tiles
*/

int32 const TileWidth = 16;
int32 const TileHeight = 16;

struct ZSprite {
    int32 width;
    int32 height;
    vec4 *colors;
};

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

struct GameMem {
    MemoryArena arena;
    MemoryArena frameMem;

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
    LoadZSpriteFromFile("data/sprites/grass_1.png", &GM.grassSprite, &GM.arena);
    LoadZSpriteFromFile("data/sprites/rock_1.png", &GM.rockSprite, &GM.arena);
    LoadZSpriteFromFile("data/sprites/pool_1.png", &GM.poolSprite, &GM.arena);

    LoadZSpriteFromFile("data/sprites/hero_down.png", &GM.heroSprite, &GM.arena);
    
}

vec2 TilePositionToPixel(int32 x, int32 y) {
    return V2(x * TileWidth, y * TileHeight);
}

void MyMosaicUpdate() {
    //ClearTiles(V4(0.08f, 0.5f, 0.17f, 1.0f));

    int32 Columns = 256 / 16;
    int32 Rows = 256 / 16;

    for (int y = 0; y < Rows; y++) {
        for (int x = 0; x < Columns; x++) {
            DrawSprite(TilePositionToPixel(x, y), &GM.grassSprite);        
        }
    }

    DrawSprite(V2((sinf(Time * 0.1f) * 100) + 100, 140), &GM.heroSprite);        

    DrawSprite(TilePositionToPixel(12, 9), &GM.rockSprite);
    DrawSprite(TilePositionToPixel(8, 12), &GM.rockSprite);
    DrawSprite(TilePositionToPixel(9, 12), &GM.rockSprite);

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

    
    
}

