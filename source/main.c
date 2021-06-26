#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

C2D_TextBuf g_staticBuf;
C2D_Text g_staticText[4];

typedef struct ball_s {
	int x, y; /* position on the screen */
	int w,h; // ball width and height
	int dx, dy; /* movement vector */
} ball_t;

typedef struct paddle_s {
	int x,y;
	int w,h;
} paddle_t;

static ball_t ball;
static paddle_t paddle;
int score;

int checkCollision(ball_t a, paddle_t b) {

	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bottom_a, bottom_b;

	left_a = a.x;
	right_a = a.x + a.w;
	top_a = a.y;
	bottom_a = a.y + a.h;

	left_b = b.x;
	right_b = b.x + b.w;
	top_b = b.y;
	bottom_b = b.y + b.h;


	if (left_a > right_b) {
		return 0;
	}

	if (right_a < left_b) {
		return 0;
	}

	if (top_a > bottom_b) {
		return 0;
	}

	if (bottom_a < top_b) {
		return 0;
	}

	return 1;
}

void moveBall() {
	ball.x += ball.dx;
	ball.y += ball.dy;

	if (ball.x < 0 || ball.x > SCREEN_WIDTH) {
		ball.dx = -ball.dx;
	}

	if (ball.y < 0) {
		ball.dy = -ball.dy;
	}

	if (ball.y > SCREEN_HEIGHT) {
		ball.dy = -ball.dy;
	}

	int c = checkCollision(ball, paddle);

	if (c == 1) {
		ball.dy = -ball.dy;
		score += 1;
	}

	if ((ball.y > SCREEN_HEIGHT) && (c != 1)) {
		score -= 1;
	}
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_TOP, NULL);

	// Create screen
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Define colors
	u32 bgColor = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
	u32 fgColor = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);

	// Init
	time_t t;
	srand((unsigned) time(&t));
	printf("\x1b[2;3HPong");
	printf("\x1b[29;3Hproudly written in nano");
	paddle.x = SCREEN_WIDTH /2 - 25;
	paddle.y = SCREEN_HEIGHT - 15;
	paddle.w = 50;
	paddle.h = 10;
	int paddleChange = 0;
	ball.x = SCREEN_WIDTH /2;
	ball.y = SCREEN_HEIGHT /2;
	ball.dx = rand() % 4;
	ball.dy = rand() % 4;
	ball.dx += 1;
	ball.dy += 1;
	score = 0;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		printf("\x1b[4;3HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[5;3HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[6;3HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

		printf("\x1b[8;3HScore:  %6.2d", score);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, bgColor);
		C2D_SceneBegin(top);

		// Handle paddle
		if (paddleChange < 0) {
			if (paddleChange < -7) {
				paddleChange = -7;
			}
			paddle.x += paddleChange;
			if (paddle.x < 0) {
				paddle.x = 0;
			}
		} else if (paddleChange > 0) {
			if (paddleChange < 7) {
				paddleChange = 7;
			}
			paddle.x += paddleChange;
			if (paddle.x > SCREEN_WIDTH - 50) {
				paddle.x = SCREEN_WIDTH - 50;
			}
		}

		moveBall();

		C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor); // Clear screen before we re-draw everything.
		C2D_DrawRectSolid(paddle.x, paddle.y , 0, paddle.w, paddle.h, fgColor); // Draw paddle
		C2D_DrawRectSolid(ball.x, ball.y, 0, 10, 10, fgColor); // Draw ball

		// New frame
		C3D_FrameEnd(0);

		// Your code goes here
		u32 kHeld = hidKeysHeld();
		if (kHeld & KEY_START) {
			break; // break in order to return to hbmenu
		} else if (kHeld & KEY_CPAD_LEFT || kHeld & KEY_LEFT) {
			paddleChange -= 1;
		} else if (kHeld & KEY_CPAD_RIGHT || kHeld & KEY_RIGHT) {
			paddleChange += 1;
		} else {
			paddleChange = 0;
		}
	}

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
