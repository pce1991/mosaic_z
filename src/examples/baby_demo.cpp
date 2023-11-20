
void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
}

int32 modeIndex = 0;

vec4 black = V4(0, 0, 0, 1);
vec4 white = V4(1, 1, 1, 1);

// checker board that blinks
// line that grows
// falling rain drops
// spiral
// scan line fill

typedef void ModeUpdate();

r32 checkerboardTimer = 0;
bool checkerboardAlternate = 0;

void CheckerBoard() {

    r32 timeSince = Game->time - checkerboardTimer;

    if (timeSince > 5.0f) {
        checkerboardTimer = Game->time;
        checkerboardAlternate = !checkerboardAlternate;
    }
    
    int32 cursor = 0;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            vec4 color = black;
            if (checkerboardAlternate) {
                if (cursor % 2 == 0) {
                    color = white;
                }    
            }
            else {
                if (cursor % 2 == 0) {
                    color = black;
                }
                else {color = white;}
            }
            

            SetTileColor(x, y, color);
            cursor++;
        }
        
        cursor++;
    }
}


int32 cursor = 0;
int32 checkerboardCursor = 0;

void CheckerBoardShift() {
    r32 timeSince = Game->time - checkerboardTimer;

    if (timeSince > 4.0f) {
        cursor += 1;
        checkerboardTimer = Game->time;
    }
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            vec4 color = black;

            if (cursor % 2 == 0) {
                color = white;
            }    

            SetTileColor(x, y, color);
            cursor++;
        }
        
        cursor++;
    }
}

vec2 drops[20];

void FallingDrops() {
    if (Mosaic->gridWidth != 128 || Mosaic->gridHeight != 128) {
        SetMosaicGridSize(128, 128);

        for (int i = 0; i < 20; i++) {
            drops[i].x = RandiRange(0, 128);
            drops[i].y = RandfRange(-128, -1);
            drops[i].y = RandfRange(0, 128);
        }
    }

    for (int i = 0; i < 20; i++) {
        drops[i].y += 20 * DeltaTime;

        if (drops[i].y > 128) {
            drops[i].x = RandiRange(0, 128);
            drops[i].y = RandfRange(-128, -1);
        }
    }

    for (int i = 0; i < 20; i++) {
        SetBlockColor(drops[i], 8, 8, white);
    }
}


void Waterfall() {
    if (Mosaic->gridWidth != 80 || Mosaic->gridHeight != 80) {
        SetMosaicGridSize(80, 80);

        for (int i = 0; i < 10; i++) {
            drops[i].x = 8 * i;
            drops[i].y = -8 * i;
        }
    }

    for (int i = 0; i < 10; i++) {
        drops[i].y += 20 * DeltaTime;
    }

    if (drops[9].y > 80) {
        for (int i = 0; i < 10; i++) {
            drops[i].x = 8 * i;
            drops[i].y = -8 * (i + 1);
        }
    }

    for (int i = 0; i < 10; i++) {
        SetBlockColor(drops[i], 8, 8, white);
    }
}

void Mode0Update() {
    
}

ModeUpdate *modeUpdates[] = {
    &CheckerBoard,
    &CheckerBoardShift,
    &FallingDrops,
    &Waterfall,
    &Mode0Update,
};

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    modeUpdates[modeIndex]();

    int32 modeCount = ARRAY_LENGTH(ModeUpdate*, modeUpdates);

    if (InputPressed(Input, Input_LeftArrow)) {
        modeIndex--;
    }
    if (InputPressed(Input, Input_RightArrow)) {
        modeIndex++;
    }
    if (modeIndex < 0) {
        modeIndex = modeCount - 1;
    }
    else if (modeIndex >= modeCount){
        modeIndex = 0;
    }
}
