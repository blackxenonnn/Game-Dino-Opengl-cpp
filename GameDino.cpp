#include <GL/glut.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath> // Menggunakan cmath untuk konsistensi

// --- KONSTANTA ---
const int WIDTH = 800;
const int HEIGHT = 400;
const float GROUND_Y = 50.0f;
const float GRASS_HEIGHT = 10.0f;
const float DINO_START_X = 100.0f;
const float DINO_SIZE = 30.0f;
const float JUMP_POWER = 15.0f;
const float GRAVITY = 0.8f;
const float OBSTACLE_SPEED = 4.0f;

// --- Deklarasi Fungsi ---
void init();
void display();
void update(int value);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void resetGame();
void drawText(float x, float y, const char* text);
void drawGround();
void drawDino();
void drawCactus(float x, float y, float width, float height);
void drawCloud(float x, float y);
void drawBird(float x, float y);
void drawStartScreen();
void drawGameOverScreen();
void drawScore();
void drawSun();
void drawMountain();

// --- Struktur Data ---
struct Cactus {
    float x, y;
    float width, height;
    bool passed;
};

struct Cloud {
    float x, y;
};

struct Bird {
    float x, y;
    float speed;
};

// --- Variabel Game ---
float dinoY = GROUND_Y + GRASS_HEIGHT;
float dinoSpeedY = 0;
bool isJumping = false;
bool gameOver = true; // Mulai dengan game over (start screen)
bool gameStarted = false; // Untuk membedakan start screen dan game over

std::vector<Cactus> obstacles;
std::vector<Cloud> clouds;
std::vector<Bird> birds;
int score = 0;
int obstacleSpawnTimer = 0;

void init() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    srand(time(0));

    // Burung awal (posisi acak tetap dipertahankan)
    for (int i = 0; i < 3; i++) {
        Bird bird;
        bird.x = rand() % WIDTH;
        bird.y = 250 + rand() % 100;
        bird.speed = 1.0f + (rand() % 10) / 5.0f;
        birds.push_back(bird);
    }
}

// Fungsi untuk menggambar matahari
void drawSun() {
    glColor3f(1.0f, 0.9f, 0.0f); // Warna kuning cerah
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(720 + cos(angle) * 40, 340 + sin(angle) * 40);
    }
    glEnd();
}

// Fungsi untuk menggambar gunung
void drawMountain() {
    // Gunung besar di belakang - warna hijau agak gelap
    glColor3f(0.0f, 0.5f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, GROUND_Y + GRASS_HEIGHT);
    glVertex2f(250, 280);
    glVertex2f(500, GROUND_Y + GRASS_HEIGHT);
    glEnd();
    
    // Gunung kecil di depan - warna hijau lebih gelap
    glColor3f(0.0f, 0.4f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(400, GROUND_Y + GRASS_HEIGHT);
    glVertex2f(600, 250);
    glVertex2f(800, GROUND_Y + GRASS_HEIGHT);
    glEnd();
}


void drawGround() {
    // Tanah
    glColor3f(0.6f, 0.4f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, GROUND_Y);
    glVertex2f(0, GROUND_Y);
    glEnd();

    // Rumput
    glColor3f(0.0f, 1.0f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, GROUND_Y);
    glVertex2f(WIDTH, GROUND_Y);
    glVertex2f(WIDTH, GROUND_Y + GRASS_HEIGHT);
    glVertex2f(0, GROUND_Y + GRASS_HEIGHT);
    glEnd();
}

void drawDino() {
    glColor3f(1.0f, 0.2f, 0.0f); // Merah

    // Badan
    glBegin(GL_QUADS);
    glVertex2f(DINO_START_X, dinoY);
    glVertex2f(DINO_START_X + DINO_SIZE, dinoY);
    glVertex2f(DINO_START_X + DINO_SIZE, dinoY + DINO_SIZE);
    glVertex2f(DINO_START_X, dinoY + DINO_SIZE);
    glEnd();

    // Kepala
    float headSize = DINO_SIZE * 0.6f;
    glBegin(GL_QUADS);
    glVertex2f(DINO_START_X + DINO_SIZE * 0.7f, dinoY + DINO_SIZE);
    glVertex2f(DINO_START_X + DINO_SIZE * 0.7f + headSize, dinoY + DINO_SIZE);
    glVertex2f(DINO_START_X + DINO_SIZE * 0.7f + headSize, dinoY + DINO_SIZE + headSize * 0.7f);
    glVertex2f(DINO_START_X + DINO_SIZE * 0.7f, dinoY + DINO_SIZE + headSize * 0.7f);
    glEnd();

    // Ekor
    glBegin(GL_TRIANGLES);
    glVertex2f(DINO_START_X, dinoY + DINO_SIZE * 0.2f);
    glVertex2f(DINO_START_X - DINO_SIZE * 0.5f, dinoY + DINO_SIZE * 0.5f);
    glVertex2f(DINO_START_X, dinoY + DINO_SIZE * 0.8f);
    glEnd();
}

void drawCactus(float x, float y, float width, float height) {
    glColor3f(0.4f, 0.6f, 0.0f); // Hijau gelap

    // Batang utama
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Lengan kaktus
    float armWidth = width * 0.4f;
    float armHeight = height * 0.4f;
    glBegin(GL_QUADS);
    // Lengan kiri
    glVertex2f(x - armWidth, y + height * 0.5f);
    glVertex2f(x, y + height * 0.5f);
    glVertex2f(x, y + height * 0.5f + armHeight);
    glVertex2f(x - armWidth, y + height * 0.5f + armHeight);
    // Lengan kanan
    glVertex2f(x + width, y + height * 0.6f);
    glVertex2f(x + width + armWidth, y + height * 0.6f);
    glVertex2f(x + width + armWidth, y + height * 0.6f + armHeight);
    glVertex2f(x + width, y + height * 0.6f + armHeight);
    glEnd();
}

void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + cos(angle) * 25, y + sin(angle) * 20);
        glVertex2f(x + 30 + cos(angle) * 20, y + 10 + sin(angle) * 15);
    }
    glEnd();
}

//Fungsi untuk menggambar burung
void drawBird(float x, float y) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    // Sayap kiri
    glVertex2f(x, y);
    glVertex2f(x - 10, y + 5);
    // Sayap kanan
    glVertex2f(x, y);
    glVertex2f(x + 10, y + 5);
    glEnd();
}


void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawStartScreen() {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(WIDTH / 2 - 80, HEIGHT / 2 + 20, "DINO RUNNER 2D");
    drawText(WIDTH / 2 - 100, HEIGHT / 2 - 10, "Tekan SPASI untuk Mulai");
}

void drawGameOverScreen() {
    glColor3f(1.0f, 0.0f, 0.0f);
    drawText(WIDTH / 2 - 50, HEIGHT / 2 + 20, "GAME OVER!");

    glColor3f(1.0f, 1.0f, 1.0f);
    char finalScoreStr[50];
    snprintf(finalScoreStr, sizeof(finalScoreStr), "Skor Akhir: %d", score);
    drawText(WIDTH / 2 - 70, HEIGHT / 2 - 10, finalScoreStr);

    glColor3f(0.8f, 0.8f, 0.8f);
    drawText(WIDTH / 2 - 100, HEIGHT / 2 - 40, "Tekan 'R' untuk Ulang");
}

void drawScore() {
    glColor3f(0.1f, 0.1f, 0.1f);
    char scoreStr[50];
    snprintf(scoreStr, sizeof(scoreStr), "Skor: %d", score);
    drawText(WIDTH - 150, HEIGHT - 30, scoreStr);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Gambar elemen latar belakang (digambar pertama agar di lapisan paling belakang)
    drawSun();
    drawMountain();
    
    // Gambar elemen latar depan
    drawGround();
    for (size_t i = 0; i < clouds.size(); ++i) {
        drawCloud(clouds[i].x, clouds[i].y);
    }
    for (size_t i = 0; i < birds.size(); i++) {
        drawBird(birds[i].x, birds[i].y);
    }
    if (gameOver) {
        if (!gameStarted) {
            drawStartScreen();
        } else {
            drawGameOverScreen();
        }
    } else {
        // Gambar elemen game saat berjalan
        drawDino();
        for (size_t i = 0; i < obstacles.size(); ++i) {
            drawCactus(obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height);
        }
        drawScore();
    }
    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        // --- Logika Lompat ---
        if (isJumping) {
            dinoY += dinoSpeedY;
            dinoSpeedY -= GRAVITY;
            if (dinoY <= GROUND_Y + GRASS_HEIGHT) {
                dinoY = GROUND_Y + GRASS_HEIGHT;
                isJumping = false;
                dinoSpeedY = 0;
            }
        }
        // --- Gerakkan & Hapus Kaktus ---
        for (size_t i = 0; i < obstacles.size(); ) {
            obstacles[i].x -= OBSTACLE_SPEED;
            if (obstacles[i].x + obstacles[i].width < 0) {
                obstacles.erase(obstacles.begin() + i);
            } else {
                if (!obstacles[i].passed && obstacles[i].x + obstacles[i].width < DINO_START_X) {
                    obstacles[i].passed = true;
                    score += 10;
                }
                i++;
            }
        }
        // --- Deteksi Tabrakan ---
        for (size_t i = 0; i < obstacles.size(); ++i) {
            if (DINO_START_X < obstacles[i].x + obstacles[i].width &&
                DINO_START_X + DINO_SIZE > obstacles[i].x &&
                dinoY < obstacles[i].y + obstacles[i].height &&
                dinoY + DINO_SIZE > obstacles[i].y)
            {
                gameOver = true;
            }
        }
        // --- Buat Kaktus Baru ---
        obstacleSpawnTimer++;
        if (obstacleSpawnTimer > 100 + (rand() % 50) - (score / 100)) {
            obstacleSpawnTimer = 0;
            Cactus newCactus;
            newCactus.x = WIDTH;
            newCactus.y = GROUND_Y + GRASS_HEIGHT;
            newCactus.width = 20 + rand() % 10;
            newCactus.height = (rand() % 2 == 0) ? 30.0f : 50.0f;
            newCactus.passed = false;
            obstacles.push_back(newCactus);
        }
    }

    // Gerakkan Burung (selalu bergerak)
    for (size_t i = 0; i < birds.size(); i++) {
        birds[i].x += birds[i].speed;
        if (birds[i].x > WIDTH + 10) {
            birds[i].x = -10;
            birds[i].y = 250 + rand() % 100;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void resetGame() {
    gameOver = false;
    gameStarted = true;
    score = 0;
    obstacles.clear();
    birds.clear(); 
    // Buat ulang burung untuk game baru
    for (int i = 0; i < 3; i++) {
        Bird bird;
        bird.x = rand() % WIDTH;
        bird.y = 250 + rand() % 100;
        bird.speed = 1.0f + (rand() % 10) / 5.0f;
        birds.push_back(bird);
    }
    dinoY = GROUND_Y + GRASS_HEIGHT;
    dinoSpeedY = 0;
    isJumping = false;
    obstacleSpawnTimer = 0;
}


void keyboard(unsigned char key, int x, int y) {
    if (gameOver) {
        if (key == 'r' || key == 'R') {
            resetGame();
        } else if (key == ' ' && !gameStarted) {
            resetGame();
        }
    } else {
        if (key == ' ') {
            if (!isJumping) {
                isJumping = true;
                dinoSpeedY = JUMP_POWER;
            }
        }
    }

    if (key == 'q' || key == 'Q' || key == 27) {
        exit(0);
    }
}

void specialKeys(int key, int x, int y) {

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Dino Game - Pemandangan");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
