#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define N 12
#define K 10
#define W 1100
#define H 760
#define CS 52
#define OX 40
#define OY 70

typedef struct { float x, y, vx, vy, t; Color c; }P;

int g[N][N], tn, cx, cy, st, ps, md, df, cur, res, mi, si, qt, fl, wc, pk;
int mv[2], wn[2], ls[2], hp[2], dd[K], wr[16], wl[16];
float ti, et, sh, mt;
char fn[64], ms[96];
P pt[512];
int dx[4] = { 0,1,1,1 }, dy[4] = { 1,0,1,-1 };
const char* mn[K] = { "Zombie","Skeleton","Creeper","Spider","Enderman","Witch","Blaze","Ghast","Wither","Ender Dragon" };
Color mc[K] = { {0,150,60,255},{200,200,200,255},{70,180,70,255},{80,55,45,255},{25,25,35,255},{115,75,145,255},{235,175,30,255},{235,235,235,255},{55,55,55,255},{25,12,35,255} };
Color me[K] = { {30,30,120,255},{10,10,10,255},{10,10,10,255},{230,30,30,255},{225,60,255,255},{255,255,255,255},{20,20,20,255},{70,70,70,255},{235,235,235,255},{200,80,255,255} };

int in(int r, int c) { return r >= 0 && c >= 0 && r < N && c < N; }

int wk(int r, int c, int x, int y, int v, int* e) {
    int l = 0;
    r += x; c += y;
    while (in(r, c) && g[r][c] == v) { l++; r += x; c += y; }
    *e = !in(r, c) ? 2 : (g[r][c] == 0 ? 0 : 1);
    return l;
}

int ck(int r, int c) {
    int v = g[r][c];
    for (int d = 0; d < 4; d++) {
        int e1, e2;
        int a = wk(r, c, dx[d], dy[d], v, &e1);
        int b = wk(r, c, -dx[d], -dy[d], v, &e2);
        if (a + b + 1 >= 5 && !(e1 == 1 && e2 == 1)) {
            wc = 0;
            for (int k = -b; k <= a; k++) { wr[wc] = r + dx[d] * k; wl[wc] = c + dy[d] * k; wc++; }
            return 1;
        }
    }
    return 0;
}

int bl(int v) {
    int m = 0, e;
    for (int i = 0; i < N; i++)for (int j = 0; j < N; j++)if (g[i][j] == v)
        for (int d = 0; d < 4; d++) {
            int a = 1 + wk(i, j, dx[d], dy[d], v, &e);
            if (a > m)m = a;
        }
    return m;
}

int ev(int r, int c, int v) {
    int s = 0;
    for (int d = 0; d < 4; d++) {
        int e1, e2;
        int l = 1 + wk(r, c, dx[d], dy[d], v, &e1) + wk(r, c, -dx[d], -dy[d], v, &e2);
        int o = (e1 == 0) + (e2 == 0);
        if (l >= 5)s += 100000;
        else if (l == 4)s += (o == 2 ? 10000 : (o == 1 ? 1000 : 0));
        else if (l == 3)s += (o == 2 ? 1000 : (o == 1 ? 100 : 0));
        else if (l == 2)s += (o == 2 ? 100 : (o == 1 ? 10 : 0));
        else s += o;
    }
    return s;
}

void sp(float x, float y, Color c, int n) {
    for (int i = 0; i < n; i++) {
        P* p = &pt[pk++ & 511];
        p->x = x; p->y = y;
        p->vx = GetRandomValue(-250, 250);
        p->vy = GetRandomValue(-400, -50);
        p->t = 0.6f + GetRandomValue(0, 60) / 100.0f;
        p->c = c;
    }
}

void pl(int r, int c) {
    if (g[r][c])return;
    int v = tn + 1, o = tn ^ 1, b;
    g[r][c] = v; mv[tn]++;
    sp(OX + c * CS + CS / 2, OY + r * CS + CS / 2, v == 1 ? SKYBLUE : mc[cur], 16);
    if (ck(r, c)) {
        hp[o] = 0; res = v; wn[tn]++; ls[o]++;
        if (v == 1)dd[cur] = 1;
        for (int k = 0; k < wc; k++)sp(OX + wl[k] * CS + CS / 2, OY + wr[k] * CS + CS / 2, GOLD, 24);
        st = 2; et = 0; sh = 1;
    }
    else if (mv[0] + mv[1] == N * N) {
        res = 3; st = 2; et = 0;
    }
    else {
        b = bl(v);
        if (b > 4)b = 4;
        if (10 - 2 * b < hp[o])hp[o] = 10 - 2 * b;
        tn = o; ti = 0.6f; sh = 0.25f;
    }
}

void ai() {
    int nz[3] = { 4000,600,0 };
    int b = -1000000000, br = 0, bc = 0;
    for (int r = 0; r < N; r++)for (int c = 0; c < N; c++)if (!g[r][c]) {
        int s = ev(r, c, 2) * 11 / 10 + ev(r, c, 1);
        int q = nz[df] * (K - cur) / K;
        s -= abs(r - N / 2) + abs(c - N / 2);
        if (q > 0)s += rand() % q;
        if (s > b) { b = s; br = r; bc = c; }
    }
    cx = br; cy = bc;
    pl(br, bc);
}

void rs() {
    memset(g, 0, sizeof g);
    memset(mv, 0, sizeof mv);
    tn = 0; cx = cy = N / 2; hp[0] = hp[1] = 10; res = 0; ti = 0.6f; wc = 0;
}

void nw() {
    cur = 0;
    memset(dd, 0, sizeof dd);
    memset(wn, 0, sizeof wn);
    memset(ls, 0, sizeof ls);
    rs(); st = 1;
}

void mg(const char* s) { snprintf(ms, sizeof ms, "%s", s); mt = 3; }

void op(int s, int p) {
    ps = p; st = s;
    strcpy(fn, "caro.sav"); fl = 8;
    while (GetCharPressed());
}

int sv() {
    FILE* f = fopen(fn, "w");
    if (!f)return 0;
    fprintf(f, "%d %d %d %d %d %d\n", md, df, cur, tn, hp[0], hp[1]);
    fprintf(f, "%d %d %d %d %d %d\n", mv[0], mv[1], wn[0], wn[1], ls[0], ls[1]);
    for (int i = 0; i < K; i++)fprintf(f, "%d ", dd[i]);
    fprintf(f, "\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)fprintf(f, "%d ", g[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    return 1;
}

int ld() {
    FILE* f = fopen(fn, "r");
    if (!f)return 0;
    int n = 12 + K + N * N, a[256], i, p = 0;
    for (i = 0; i < n; i++)if (fscanf(f, "%d", &a[i]) != 1)break;
    fclose(f);
    if (i < n)return 0;
    if (a[0] < 0 || a[0]>1 || a[1] < 0 || a[1]>2 || a[2] < 0 || a[2] >= K || a[3] < 0 || a[3]>1)return 0;
    for (i = 12 + K; i < n; i++)if (a[i] < 0 || a[i]>2)return 0;
    md = a[p++]; df = a[p++]; cur = a[p++]; tn = a[p++]; hp[0] = a[p++]; hp[1] = a[p++];
    mv[0] = a[p++]; mv[1] = a[p++]; wn[0] = a[p++]; wn[1] = a[p++]; ls[0] = a[p++]; ls[1] = a[p++];
    for (i = 0; i < K; i++)dd[i] = a[p++];
    for (i = 0; i < N; i++)for (int j = 0; j < N; j++)g[i][j] = a[p++];
    cx = cy = N / 2; res = 0; ti = 0.6f; wc = 0; st = 1;
    return 1;
}

int kp(int k) { return IsKeyPressed(k) || IsKeyPressedRepeat(k); }

void upd(float dt) {
    if (mt > 0)mt -= dt;
    if (sh > 0)sh -= dt * 1.5f;
    if (sh < 0)sh = 0;
    for (int i = 0; i < 512; i++)if (pt[i].t > 0) {
        P* p = &pt[i];
        p->x += p->vx * dt; p->y += p->vy * dt; p->vy += 900 * dt; p->t -= dt;
    }
    if (st == 0) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))mi = (mi + 3) % 4;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))mi = (mi + 1) % 4;
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (mi == 0)nw();
            else if (mi == 1)op(5, 0);
            else if (mi == 2) { st = 3; si = 0; }
            else qt = 1;
        }
    }
    else if (st == 3) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))si = (si + 2) % 3;
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))si = (si + 1) % 3;
        int d = (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) - (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT));
        if (si == 0 && d)md ^= 1;
        if (si == 1 && d)df = (df + d + 3) % 3;
        if ((si == 2 && IsKeyPressed(KEY_ENTER)) || IsKeyPressed(KEY_ESCAPE))st = 0;
    }
    else if (st == 4 || st == 5) {
        int k;
        while ((k = GetCharPressed()) > 0)if (fl < 30 && k>32 && k < 127 && k != '/' && k != '\\') { fn[fl++] = k; fn[fl] = 0; }
        if (kp(KEY_BACKSPACE) && fl > 0)fn[--fl] = 0;
        if (IsKeyPressed(KEY_ESCAPE))st = ps;
        else if (IsKeyPressed(KEY_ENTER) && fl > 0) {
            if (st == 4) { mg(sv() ? "Game saved" : "Cannot save"); st = ps; }
            else if (ld())mg("Game loaded");
            else { mg("Cannot load"); st = ps; }
        }
    }
    else if (st == 1) {
        if (IsKeyPressed(KEY_ESCAPE))st = 0;
        else if (IsKeyPressed(KEY_L))op(4, 1);
        else if (IsKeyPressed(KEY_T))op(5, 1);
        else if (md == 0 && tn == 1) {
            ti -= dt;
            if (ti <= 0)ai();
        }
        else {
            if (kp(KEY_W) && cx > 0)cx--;
            if (kp(KEY_S) && cx < N - 1)cx++;
            if (kp(KEY_A) && cy > 0)cy--;
            if (kp(KEY_D) && cy < N - 1)cy++;
            if (IsKeyPressed(KEY_ENTER)) {
                if (g[cx][cy])mg("That cell is taken");
                else pl(cx, cy);
            }
        }
    }
    else if (st == 2) {
        et += dt;
        if (et < 0.8f) { while (GetKeyPressed()); }
        else {
            int k = GetKeyPressed();
            if (k == KEY_Y) {
                if (res == 1) {
                    if (cur == K - 1)st = 6;
                    else { cur++; rs(); st = 1; }
                }
                else { rs(); st = 1; }
            }
            else if (k)st = 0;
        }
    }
    else if (st == 6) {
        if (IsKeyPressed(KEY_ENTER))st = 0;
    }
}

void rc(float x, float y, float w, float h, Color c) { DrawRectangleRec((Rectangle) { x, y, w, h }, c); }

void tx(const char* s, int x, int y, int z, Color c) {
    DrawText(s, x + 2, y + 2, z, (Color) { 0, 0, 0, 160 });
    DrawText(s, x, y, z, c);
}

void ct(const char* s, int y, int z, Color c) { tx(s, (W - MeasureText(s, z)) / 2, y, z, c); }

void bg() {
    for (int i = 0; i < W / 32 + 1; i++)for (int j = 0; j < H / 32 + 1; j++) {
        unsigned h = ((unsigned)i * 73856093u ^ (unsigned)j * 19349663u) >> 4 & 7u;
        int v = (int)h;
        DrawRectangle(i * 32, j * 32, 32, 32, (Color) { 95 + v * 4, 68 + v * 3, 44 + v * 2, 255 });
    }
}

void fc(int x, int y, int s, int i) {
    float u = s / 8.0f;
    rc(x, y, s, s, mc[i]);
    rc(x + u, y + 2 * u, 2 * u, 2 * u, me[i]);
    rc(x + 5 * u, y + 2 * u, 2 * u, 2 * u, me[i]);
    if (i == 2) {
        rc(x + 3 * u, y + 4 * u, 2 * u, u, BLACK);
        rc(x + 2 * u, y + 5 * u, 4 * u, 2 * u, BLACK);
        rc(x + 3 * u, y + 6 * u, 2 * u, u, mc[i]);
    }
    else rc(x + 2 * u, y + 5 * u, 4 * u, u, (Color) { 0, 0, 0, 150 });
}

void xs(float x, float y, float s) {
    float m = s * 0.18f;
    DrawLineEx((Vector2) { x + m, y + m }, (Vector2) { x + s - m, y + s - m }, s * 0.2f, (Color) { 20, 50, 80, 255 });
    DrawLineEx((Vector2) { x + m, y + s - m }, (Vector2) { x + s - m, y + m }, s * 0.2f, (Color) { 20, 50, 80, 255 });
    DrawLineEx((Vector2) { x + m, y + m }, (Vector2) { x + s - m, y + s - m }, s * 0.1f, (Color) { 90, 235, 235, 255 });
    DrawLineEx((Vector2) { x + m, y + s - m }, (Vector2) { x + s - m, y + m }, s * 0.1f, (Color) { 90, 235, 235, 255 });
}

void hr(float x, float y, int f) {
    static const char* p[6] = { ".11.11.","1111111","1111111",".11111.","..111..","...1..." };
    for (int r = 0; r < 6; r++)for (int c = 0; c < 7; c++)if (p[r][c] == '1') {
        Color k = (Color){ 50,10,10,255 };
        if (f >= 2 || (f == 1 && c < 4))k = (Color){ 235,35,45,255 };
        rc(x + c * 4, y + r * 4, 4, 4, k);
    }
}

void bd() {
    rc(OX - 8, OY - 8, N * CS + 16, N * CS + 16, (Color) { 40, 40, 40, 255 });
    for (int r = 0; r < N; r++)for (int c = 0; c < N; c++) {
        Color k = ((r + c) & 1) ? (Color) { 106, 170, 64, 255 } : (Color) { 94, 156, 56, 255 };
        rc(OX + c * CS, OY + r * CS, CS, CS, k);
        DrawRectangleLines(OX + c * CS, OY + r * CS, CS, CS, (Color) { 60, 100, 40, 255 });
        if (g[r][c] == 1)xs(OX + c * CS + 4, OY + r * CS + 4, CS - 8);
        else if (g[r][c] == 2)fc(OX + c * CS + 6, OY + r * CS + 6, CS - 12, cur);
    }
    if (st == 2 && res < 3)for (int k = 0; k < wc; k++) {
        unsigned char a = (unsigned char)(120 + 120 * sin(GetTime() * 10));
        rc(OX + wl[k] * CS, OY + wr[k] * CS, CS, CS, (Color) { 255, 215, 0, (unsigned char)(a / 3) });
        DrawRectangleLinesEx((Rectangle) { OX + wl[k] * CS, OY + wr[k] * CS, CS, CS }, 4, (Color) { 255, 215, 0, a });
    }
    if (st == 1 && !(md == 0 && tn == 1))DrawRectangleLinesEx((Rectangle) { OX + cy * CS, OY + cx * CS, CS, CS }, 4, YELLOW);
}

void hd() {
    char b[128];
    int x = 694;
    const char* q[2] = { "Steve",md ? "Player 2" : mn[cur] };
    snprintf(b, sizeof b, "MINECRAFT CARO  -  Battle %d/%d: %s", cur + 1, K, mn[cur]);
    tx(b, OX, 22, 28, WHITE);
    rc(x, OY - 8, 376, N * CS + 16, (Color) { 0, 0, 0, 150 });
    for (int p = 0; p < 2; p++) {
        int y = OY + p * 140;
        rc(x + 10, y, 356, 124, (Color) { 60, 60, 60, 200 });
        if (st == 1 && tn == p)DrawRectangleLinesEx((Rectangle) { x + 10, y, 356, 124 }, 3, YELLOW);
        if (p == 0) {
            rc(x + 22, y + 12, 44, 44, (Color) { 80, 60, 40, 255 });
            xs(x + 22, y + 12, 44);
        }
        else fc(x + 22, y + 12, 44, cur);
        tx(q[p], x + 80, y + 10, 26, WHITE);
        for (int h = 0; h < 5; h++)hr(x + 80 + h * 34, y + 44, hp[p] - 2 * h);
        snprintf(b, sizeof b, "Moves %d   Wins %d   Losses %d", mv[p], wn[p], ls[p]);
        tx(b, x + 22, y + 88, 20, LIGHTGRAY);
    }
    int nd = 0;
    for (int i = 0; i < K; i++)nd += dd[i];
    snprintf(b, sizeof b, "Mobs defeated: %d/%d", nd, K);
    tx(b, x + 22, 356, 22, WHITE);
    for (int i = 0; i < K; i++) {
        int a = x + 22 + (i % 5) * 68, c = 390 + (i / 5) * 50;
        fc(a, c, 40, i);
        if (!dd[i])rc(a, c, 40, 40, (Color) { 0, 0, 0, 150 });
        if (i == cur)DrawRectangleLinesEx((Rectangle) { a - 3, c - 3, 46, 46 }, 3, YELLOW);
    }
    tx("W A S D  : move", x + 22, 500, 20, WHITE);
    tx("Enter    : place block", x + 22, 526, 20, WHITE);
    tx("L / T    : save / load", x + 22, 552, 20, WHITE);
    tx("Esc      : main menu", x + 22, 578, 20, WHITE);
    tx("Five in a row wins", x + 22, 620, 20, LIGHTGRAY);
    tx("(not blocked on both ends)", x + 22, 644, 20, LIGHTGRAY);
    if (st == 1) {
        snprintf(b, sizeof b, "Turn: %s (%s)", q[tn], tn ? "O" : "X");
        tx(b, OX, 704, 22, WHITE);
    }
}

void pa() {
    for (int i = 0; i < 512; i++)if (pt[i].t > 0) {
        Color c = pt[i].c;
        c.a = (unsigned char)(255 * fminf(1, pt[i].t * 2));
        rc(pt[i].x, pt[i].y, 7, 7, c);
    }
}

void ov() {
    char b[128];
    const char* q[2] = { "Steve",md ? "Player 2" : mn[cur] };
    rc(0, H / 2 - 110, W, 220, (Color) { 0, 0, 0, 200 });
    if (res == 3)ct("Hai ben hoa nhau!", H / 2 - 80, 56, LIGHTGRAY);
    else {
        snprintf(b, sizeof b, "%s defeated %s!", q[res - 1], q[2 - res]);
        ct(b, H / 2 - 80, 52, res == 1 ? GOLD : RED);
    }
    if (et > 0.8f)ct("Press Y to continue - any other key for menu", H / 2 + 20, 28, WHITE);
}

void bt(const char* s, float y, int on) {
    float x = (W - 420) / 2.0f;
    rc(x, y, 420, 64, on ? (Color) { 160, 160, 160, 255 } : (Color) { 115, 115, 115, 255 });
    DrawRectangleLinesEx((Rectangle) { x, y, 420, 64 }, 4, on ? YELLOW : (Color) { 40, 40, 40, 255 });
    ct(s, y + 16, 32, WHITE);
}

void dm() {
    const char* it[4] = { "New Game","Load Game","Settings","Exit" };
    ct("MINECRAFT CARO", 70, 72, (Color) { 90, 235, 90, 255 });
    ct("Defeat every mob with five in a row", 160, 26, LIGHTGRAY);
    for (int i = 0; i < 4; i++)bt(it[i], 260 + i * 84, i == mi);
    for (int i = 0; i < K; i++)fc((W - 588) / 2 + i * 60, 640, 48, i);
}

void ds() {
    char b[3][64];
    ct("SETTINGS", 90, 56, WHITE);
    snprintf(b[0], 64, "Mode: %s", md ? "2 Players" : "Player vs Mob");
    snprintf(b[1], 64, "Difficulty: %s", df == 0 ? "Easy" : (df == 1 ? "Normal" : "Hard"));
    snprintf(b[2], 64, "Back");
    for (int i = 0; i < 3; i++)bt(b[i], 240 + i * 84, i == si);
    ct("W/S select    A/D change    Enter confirm", 560, 22, LIGHTGRAY);
}

void di() {
    char b[80];
    ct(st == 4 ? "SAVE GAME" : "LOAD GAME", 120, 56, WHITE);
    ct("Enter file name:", 260, 30, LIGHTGRAY);
    rc(250, 320, 600, 64, (Color) { 20, 20, 20, 230 });
    DrawRectangleLinesEx((Rectangle) { 250, 320, 600, 64 }, 4, GRAY);
    snprintf(b, sizeof b, "%s%s", fn, ((int)(GetTime() * 2) & 1) ? "_" : "");
    tx(b, 266, 336, 32, WHITE);
    ct("Enter: confirm    Esc: cancel", 430, 22, LIGHTGRAY);
}

void dv() {
    char b[96];
    ct("VICTORY!", 110, 84, GOLD);
    ct("You defeated every mob in the Overworld, the Nether and the End", 230, 28, WHITE);
    for (int i = 0; i < K; i++)fc((W - 588) / 2 + i * 60, 330, 48, i);
    snprintf(b, sizeof b, "Steve wins %d  -  losses %d", wn[0], ls[0]);
    ct(b, 440, 28, LIGHTGRAY);
    ct("Press Enter to return to the menu", 560, 26, YELLOW);
}

void drw() {
    bg();
    if (st == 0)dm();
    else if (st == 3)ds();
    else if (st == 4 || st == 5)di();
    else if (st == 6)dv();
    else {
        Camera2D cm = { {GetRandomValue(-100,100) / 100.0f * sh * 14,GetRandomValue(-100,100) / 100.0f * sh * 14},{0,0},0,1 };
        BeginMode2D(cm);
        bd();
        hd();
        pa();
        EndMode2D();
        if (st == 2)ov();
    }
    if (mt > 0)tx(ms, OX, 730, 20, YELLOW);
}

int main() {
    srand((unsigned)time(0));
    InitWindow(W, H, "Minecraft Caro");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    df = 1;
    rs();
    while (!qt && !WindowShouldClose()) {
        upd(GetFrameTime());
        BeginDrawing();
        ClearBackground(BLACK);
        drw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}