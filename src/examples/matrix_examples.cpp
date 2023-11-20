
// wireframe rendering of points
// lighting (toon shading first?)
// load mesh from OBJ
// generate meshes

struct MyData {
    
};

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;
}

void MyGameUpdate() {
    Camera *cam = &Game->camera;

    vec2 size = V2(0.05f, 0.05f);
    vec3 position = V3(1, 0, 0);
    vec3 positionB = V3(1, 1, 0);

    mat3 mat = Identity3();
    mat.columns[0] = V3(0, 1, 0);
    mat.columns[1] = V3(1, 0, 0);

    DrawRect(position.xy, size, RGB(1, 1, 1));

    DrawRect((mat * position).xy, size, RGB(1, 0, 0));

    mat.columns[0] = V3(0, -1, 0);
    mat.columns[1] = V3(1, 0, 0);
    DrawRect((mat * position).xy, size, RGB(1, 0, 0));

    mat.columns[0] = V3(cosf(Time), sinf(Time), 0);
    mat.columns[1] = V3(-sinf(Time), cosf(Time), 0);
    DrawRect((mat * position).xy, size, RGB(1, 1, 0));

    mat.columns[0] = V3(2, 0, 0);
    mat.columns[1] = V3(0, 1, 0);
    DrawRect((mat * positionB).xy, size, RGB(0, 1, 0));

    mat3 matB = Identity3();

    matB.columns[0] = V3(cosf(Time), sinf(Time), 0);
    matB.columns[1] = V3(-sinf(Time), cosf(Time), 0);

    mat.columns[0] = V3(2, 0, 0);
    mat.columns[1] = V3(0, 2, 0);

    // Now the matrix is a combination of the rotation and the scale
    mat = matB * mat;
    DrawRect((mat * position).xy, size, RGB(0, 1, 1));

    // Show relative positioning

    DrawRect(V2(0), size, RGB(0.2f, 0.2f, 0.2f));
}
