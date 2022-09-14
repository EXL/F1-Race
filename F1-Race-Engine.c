/*
 * Useful commands:
 *
 *   $ gcc F1-Race-Engine.c -o F1-Race -lSDL2
 *   $ rm Resources.h ; find assets/ -type f -exec xxd -i {} >> Resources.h \;
 *
 *   $ ffmpeg -i GAME_F1RACE_STATUS_SCORE.gif GAME_F1RACE_STATUS_SCORE.bmp
 *   $ convert GAME_F1RACE_STATUS_SCORE.gif GAME_F1RACE_STATUS_SCORE.bmp
 *
 *   $ find -name "*.gif" -exec sh -c 'ffmpeg -i "$1" `basename $1 .gif`.bmp' sh {} \;
 */

#include "Resources.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define F1RACE_PLAYER_CAR_IMAGE_SIZE_X             (15)
#define F1RACE_PLAYER_CAR_IMAGE_SIZE_Y             (20)
#define F1RACE_PLAYER_CAR_CARSH_IMAGE_SIZE_X       (15)
#define F1RACE_PLAYER_CAR_CARSH_IMAGE_SIZE_Y       (25)
#define F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_X         (23)
#define F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_Y         (27)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_IMAGE_SIZE_X  (7)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_IMAGE_SIZE_Y  (15)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_0_SHIFT       (1)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_1_SHIFT       (7)
#define F1RACE_OPPOSITE_CAR_TYPE_COUNT             (7)
#define F1RACE_PLAYER_CAR_FLY_FRAME_COUNT          (10)
#define F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X         (17)
#define F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_Y         (35)
#define F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X         (12)
#define F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_Y         (18)
#define F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X         (15)
#define F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_Y         (20)
#define F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X         (12)
#define F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_Y         (18)
#define F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X         (17)
#define F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_Y         (27)
#define F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X         (13)
#define F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_Y         (21)
#define F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_X         (13)
#define F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_Y         (22)
#define F1RACE_OPPOSITE_CAR_COUNT                  (8)
#define F1RACE_OPPOSITE_CAR_DEFAULT_APPEAR_RATE    (2)
#define F1RACE_MAX_FLY_COUNT                       (9)
#define F1RACE_TIMER_ELAPSE                        (100)
#define F1RACE_PLAYER_CAR_SHIFT                    (5)
#define F1RACE_PLAYER_CAR_FLY_SHIFT                (2)
#define F1RACE_DISPLAY_START_X                     (3)
#define F1RACE_DISPLAY_START_Y                     (3)
#define F1RACE_DISPLAY_END_X                       (124)
#define F1RACE_DISPLAY_END_Y                       (124)
#define F1RACE_ROAD_WIDTH                          (23)
#define F1RACE_SEPARATOR_WIDTH                     (3)
#define F1RACE_GRASS_WIDTH                         (7)
#define F1RACE_STATUS_WIDTH                        (32)
#define F1RACE_SEPARATOR_HEIGHT_SPACE              (3)
#define F1RACE_SEPARATOR_RATIO                     (6)
#define F1RACE_SEPARATOR_HEIGHT                    (F1RACE_SEPARATOR_HEIGHT_SPACE*F1RACE_SEPARATOR_RATIO)
#define F1RACE_STATUS_NUMBER_WIDTH                 (4)
#define F1RACE_STATUS_NUBBER_HEIGHT                (7)
#define F1RACE_GRASS_0_START_X                     (F1RACE_DISPLAY_START_X)
#define F1RACE_GRASS_0_END_X                       (F1RACE_GRASS_0_START_X + F1RACE_GRASS_WIDTH)-1
#define F1RACE_ROAD_0_START_X                      (F1RACE_GRASS_0_START_X + F1RACE_GRASS_WIDTH)
#define F1RACE_ROAD_0_END_X                        (F1RACE_ROAD_0_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_SEPARATOR_0_START_X                 (F1RACE_ROAD_0_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_SEPARATOR_0_END_X                   (F1RACE_SEPARATOR_0_START_X + F1RACE_SEPARATOR_WIDTH)-1
#define F1RACE_ROAD_1_START_X                      (F1RACE_SEPARATOR_0_START_X + F1RACE_SEPARATOR_WIDTH)
#define F1RACE_ROAD_1_END_X                        (F1RACE_ROAD_1_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_SEPARATOR_1_START_X                 (F1RACE_ROAD_1_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_SEPARATOR_1_END_X                   (F1RACE_SEPARATOR_1_START_X + F1RACE_SEPARATOR_WIDTH)-1
#define F1RACE_ROAD_2_START_X                      (F1RACE_SEPARATOR_1_START_X + F1RACE_SEPARATOR_WIDTH)
#define F1RACE_ROAD_2_END_X                        (F1RACE_ROAD_2_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_GRASS_1_START_X                     (F1RACE_ROAD_2_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_GRASS_1_END_X                       (F1RACE_GRASS_1_START_X + F1RACE_GRASS_WIDTH)-1
#define F1RACE_STATUS_START_X                      (F1RACE_GRASS_1_START_X + F1RACE_GRASS_WIDTH)
#define F1RACE_STATUS_END_X                        (F1RACE_STATUS_START_X + F1RACE_STATUS_WIDTH)

#define F1RACE_RELEASE_ALL_KEY                       \
{                                                    \
	f1race_key_up_pressed      = SDL_FALSE;          \
	f1race_key_down_pressed    = SDL_FALSE;          \
	f1race_key_left_pressed    = SDL_FALSE;          \
	f1race_key_right_pressed   = SDL_FALSE;          \
	if(f1race_is_crashing == SDL_TRUE)               \
		return;                                      \
}                                                    \

#define F1RACE_GET_NUMBER_IMAGE(value, image, len)   \
{                                                    \
   switch(value) {                                   \
   case 0:                                           \
      image = assets_GAME_F1RACE_NUMBER_0_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_0_bmp_len;     \
      break;                                         \
   case 1:                                           \
      image = assets_GAME_F1RACE_NUMBER_1_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_1_bmp_len;     \
      break;                                         \
   case 2:                                           \
      image = assets_GAME_F1RACE_NUMBER_2_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_2_bmp_len;     \
      break;                                         \
   case 3:                                           \
      image = assets_GAME_F1RACE_NUMBER_3_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_3_bmp_len;     \
      break;                                         \
   case 4:                                           \
      image = assets_GAME_F1RACE_NUMBER_4_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_4_bmp_len;     \
      break;                                         \
   case 5:                                           \
      image = assets_GAME_F1RACE_NUMBER_5_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_5_bmp_len;     \
      break;                                         \
   case 6:                                           \
      image = assets_GAME_F1RACE_NUMBER_6_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_6_bmp_len;     \
      break;                                         \
   case 7:                                           \
      image = assets_GAME_F1RACE_NUMBER_7_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_7_bmp_len;     \
      break;                                         \
   case 8:                                           \
      image = assets_GAME_F1RACE_NUMBER_8_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_8_bmp_len;     \
      break;                                         \
   case 9:                                           \
      image = assets_GAME_F1RACE_NUMBER_9_bmp;       \
      len = assets_GAME_F1RACE_NUMBER_9_bmp_len;     \
      break;                                         \
   }                                                 \
}

typedef struct {
	int16_t pos_x;
	int16_t pos_y;
	int16_t dx;
	int16_t dy;
	void * image;
	void * image_fly;
	void * image_head_light;
} F1RACE_CAR_STRUCT;

typedef struct {
	int16_t dx;
	int16_t dy;
	int16_t speed;
	int16_t dx_from_road;
	void * image;
	uint32_t length;
} F1RACE_OPPOSITE_CAR_TYPE_STRUCT;

typedef struct {
	int16_t dx;
	int16_t dy;
	int16_t speed;
	int16_t dx_from_road;
	void * image;
	uint32_t length;
	int16_t pos_x;
	int16_t pos_y;
	uint8_t road_id;
	SDL_bool is_empty;
	SDL_bool is_add_score;
} F1RACE_OPPOSITE_CAR_STRUCT;

static SDL_bool exit_main_loop = SDL_FALSE;
static SDL_Renderer* render = NULL;

static SDL_bool f1race_is_new_game = SDL_TRUE;
static SDL_bool f1race_is_crashing = SDL_FALSE;
static int16_t f1race_separator_0_block_start_y;
static int16_t f1race_separator_1_block_start_y;

static int16_t f1race_last_car_road;
static SDL_bool f1race_player_is_car_fly;
static int16_t f1race_player_car_fly_duration;
static int16_t f1race_score;
static int16_t f1race_level;
static int16_t f1race_pass;
static int16_t f1race_fly_count;
static int16_t f1race_fly_charger_count;

static SDL_bool f1race_key_up_pressed = SDL_FALSE;
static SDL_bool f1race_key_down_pressed = SDL_FALSE;
static SDL_bool f1race_key_right_pressed = SDL_FALSE;
static SDL_bool f1race_key_left_pressed = SDL_FALSE;

static F1RACE_CAR_STRUCT f1race_player_car;
static F1RACE_OPPOSITE_CAR_TYPE_STRUCT f1race_opposite_car_type[F1RACE_OPPOSITE_CAR_TYPE_COUNT];
static F1RACE_OPPOSITE_CAR_STRUCT f1race_opposite_car[F1RACE_OPPOSITE_CAR_COUNT];

static void F1Race_DrawBitmap(void *memory, uint32_t length, int32_t x, int32_t y) {
	SDL_RWops* bitmap_rwops = SDL_RWFromConstMem(memory, length);
	SDL_Surface* bitmap = SDL_LoadBMP_RW(bitmap_rwops, SDL_FALSE);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, bitmap);

	SDL_Rect rectangle;
	rectangle.x = x;
	rectangle.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h);
	SDL_RenderCopy(render, texture, NULL, &rectangle);

	SDL_FreeSurface(bitmap);
	SDL_DestroyTexture(texture);
}

static void F1Race_Render_Separator(void) {
	int16_t start_y, end_y;

	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 250, 250, 250, 0);
	rectangle.x = F1RACE_SEPARATOR_0_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_SEPARATOR_0_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	SDL_SetRenderDrawColor(render, 250, 250, 250, 0);
	rectangle.x = F1RACE_SEPARATOR_1_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_SEPARATOR_1_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	start_y = f1race_separator_0_block_start_y;
	end_y = start_y + F1RACE_SEPARATOR_HEIGHT_SPACE;
	while (SDL_TRUE) {
		SDL_SetRenderDrawColor(render, 150, 150, 150, 0);
		rectangle.x = F1RACE_SEPARATOR_0_START_X;
		rectangle.y = start_y;
		rectangle.w = F1RACE_SEPARATOR_0_END_X + 1 - rectangle.x;
		rectangle.h = end_y - rectangle.y;
		SDL_RenderFillRect(render, &rectangle);

		start_y += F1RACE_SEPARATOR_HEIGHT;
		end_y = start_y + F1RACE_SEPARATOR_HEIGHT_SPACE;
		if (start_y > F1RACE_DISPLAY_END_Y)
			break;
		if (end_y > F1RACE_DISPLAY_END_Y)
			end_y = F1RACE_DISPLAY_END_Y;
	}
	f1race_separator_0_block_start_y += F1RACE_SEPARATOR_HEIGHT_SPACE;
	if (f1race_separator_0_block_start_y >=
		(F1RACE_DISPLAY_START_Y + F1RACE_SEPARATOR_HEIGHT_SPACE * F1RACE_SEPARATOR_RATIO))
		f1race_separator_0_block_start_y = F1RACE_DISPLAY_START_Y;

	start_y = f1race_separator_1_block_start_y;
	end_y = start_y + F1RACE_SEPARATOR_HEIGHT_SPACE;
	while (SDL_TRUE) {
		SDL_SetRenderDrawColor(render, 150, 150, 150, 0);
		rectangle.x = F1RACE_SEPARATOR_1_START_X;
		rectangle.y = start_y;
		rectangle.w = F1RACE_SEPARATOR_1_END_X + 1 - rectangle.x;
		rectangle.h = end_y - rectangle.y;
		SDL_RenderFillRect(render, &rectangle);

		start_y += F1RACE_SEPARATOR_HEIGHT;
		end_y = start_y + F1RACE_SEPARATOR_HEIGHT_SPACE;
		if (start_y > F1RACE_DISPLAY_END_Y)
			break;
		if (end_y > F1RACE_DISPLAY_END_Y)
			end_y = F1RACE_DISPLAY_END_Y;
	}
	f1race_separator_1_block_start_y += F1RACE_SEPARATOR_HEIGHT_SPACE;
	if (f1race_separator_1_block_start_y >=
		(F1RACE_DISPLAY_START_Y + F1RACE_SEPARATOR_HEIGHT_SPACE * F1RACE_SEPARATOR_RATIO))
		f1race_separator_1_block_start_y = F1RACE_DISPLAY_START_Y;
}

static void F1Race_Render_Road(void) {
	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 150, 150, 150, 0);
	rectangle.x = F1RACE_ROAD_0_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_ROAD_2_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);
}

static void F1Race_Render_Status(void) {
	int16_t x_pos;
	int16_t y_pos;
	int16_t value;
	int16_t remain;
	int16_t score;
	int16_t index;

	void * image_id;
	uint32_t length;

	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = F1RACE_STATUS_START_X + 4;
	rectangle.y = F1RACE_DISPLAY_START_Y + 52;
	rectangle.w = F1RACE_STATUS_START_X + 29 + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_START_Y + 58 - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	x_pos = F1RACE_STATUS_START_X + 25;
	y_pos = F1RACE_DISPLAY_START_Y + 52;

	score = f1race_score;
	score = 1235;
	value = score % 10;
	remain = score / 10;

	while (SDL_TRUE) {
		F1RACE_GET_NUMBER_IMAGE(value, image_id, length);
		F1Race_DrawBitmap(image_id, length, x_pos, y_pos);

		x_pos -= 5;
		if (remain > 0) {
			value = remain % 10;
			remain = remain / 10;
		}
		else
			break;
	}

	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = F1RACE_STATUS_START_X + 4;
	rectangle.y = F1RACE_DISPLAY_START_Y + 74;
	rectangle.w = F1RACE_STATUS_START_X + 29 + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_START_Y + 80 - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	x_pos = F1RACE_STATUS_START_X + 16;
	y_pos = F1RACE_DISPLAY_START_Y + 74;

	F1RACE_GET_NUMBER_IMAGE(f1race_level, image_id, length);
	F1Race_DrawBitmap(image_id, length, x_pos, y_pos);

	x_pos = F1RACE_STATUS_START_X + 4;
	y_pos = F1RACE_DISPLAY_START_Y + 102;
	for (index = 0; index < 5; index++) {
		if (index < f1race_fly_charger_count)
			SDL_SetRenderDrawColor(render, 255, 0, 0, 0);
		else
			SDL_SetRenderDrawColor(render, 100, 100, 100, 0);
		rectangle.x = x_pos + index * 4;
		rectangle.y = y_pos - 2 - index;
		rectangle.w = x_pos + 2 + index * 4 + 1 - rectangle.x;
		rectangle.h = y_pos - rectangle.y;
		SDL_RenderFillRect(render, &rectangle);
	}

	F1RACE_GET_NUMBER_IMAGE(f1race_fly_count, image_id, length);
	x_pos = F1RACE_STATUS_START_X + 25;
	y_pos = F1RACE_DISPLAY_START_Y + 96;
	F1Race_DrawBitmap(image_id, length, x_pos, y_pos);
}

static void F1Race_Render_Player_Car(void) {
	int16_t dx;
	int16_t dy;

	void * image;
	uint32_t length;

	if (f1race_player_is_car_fly == SDL_FALSE)
		F1Race_DrawBitmap(assets_GAME_F1RACE_PLAYER_CAR_bmp, assets_GAME_F1RACE_PLAYER_CAR_bmp_len,
			f1race_player_car.pos_x, f1race_player_car.pos_y);
	else {
		dx = (F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_X - F1RACE_PLAYER_CAR_IMAGE_SIZE_X) / 2;
		dy = (F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_Y - F1RACE_PLAYER_CAR_IMAGE_SIZE_Y) / 2;
		dx = f1race_player_car.pos_x - dx;
		dy = f1race_player_car.pos_y - dy;
		switch (f1race_player_car_fly_duration) {
			case 0:
			case 1:
				image = assets_GAME_F1RACE_PLAYER_CAR_FLY_UP_bmp;
				length = assets_GAME_F1RACE_PLAYER_CAR_FLY_UP_bmp_len;
				break;
			case (F1RACE_PLAYER_CAR_FLY_FRAME_COUNT - 1):
			case (F1RACE_PLAYER_CAR_FLY_FRAME_COUNT - 2):
				image = assets_GAME_F1RACE_PLAYER_CAR_FLY_DOWN_bmp;
				length = assets_GAME_F1RACE_PLAYER_CAR_FLY_DOWN_bmp_len;
				break;
			default:
				image = assets_GAME_F1RACE_PLAYER_CAR_FLY_bmp;
				length = assets_GAME_F1RACE_PLAYER_CAR_FLY_bmp_len;
				break;
		}
		F1Race_DrawBitmap(image, length, dx, dy);
	}
}

static void F1Race_Render_Opposite_Car(void) {
	int16_t index;
	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		if (f1race_opposite_car[index].is_empty == SDL_FALSE)
			F1Race_DrawBitmap(f1race_opposite_car[index].image, f1race_opposite_car[index].length,
				f1race_opposite_car[index].pos_x, f1race_opposite_car[index].pos_y);
	}
}


static void F1Race_Render(void) {
//	gui_set_clip(F1RACE_STATUS_START_X, F1RACE_DISPLAY_START_Y, F1RACE_STATUS_END_X, F1RACE_DISPLAY_END_Y);

	F1Race_Render_Status();

//	gui_set_clip(F1RACE_ROAD_0_START_X, F1RACE_DISPLAY_START_Y, F1RACE_ROAD_2_END_X, F1RACE_DISPLAY_END_Y);

	F1Race_Render_Road();
	F1Race_Render_Separator();
	F1Race_Render_Opposite_Car();
	F1Race_Render_Player_Car();
}

static void F1Race_Render_Background(void) {
	SDL_Rect rectangle;

	SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
	SDL_RenderClear(render);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = F1RACE_DISPLAY_START_X - 1;
	rectangle.y = F1RACE_DISPLAY_START_Y - 1;
	rectangle.w = F1RACE_DISPLAY_END_X + 2 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y + 1 - rectangle.y;
	SDL_RenderDrawRect(render, &rectangle);

	SDL_SetRenderDrawColor(render, 130, 230, 100, 0);
	rectangle.x = F1RACE_GRASS_0_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_GRASS_0_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	SDL_SetRenderDrawColor(render, 100, 180, 100, 0);
	SDL_RenderDrawLine(render, F1RACE_GRASS_0_END_X - 1,
		F1RACE_DISPLAY_START_Y, F1RACE_GRASS_0_END_X - 1, F1RACE_DISPLAY_END_Y - 1);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	SDL_RenderDrawLine(render, F1RACE_GRASS_0_END_X,
		F1RACE_DISPLAY_START_Y, F1RACE_GRASS_0_END_X, F1RACE_DISPLAY_END_Y);

	SDL_SetRenderDrawColor(render, 130, 230, 100, 0);
	rectangle.x = F1RACE_GRASS_1_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_GRASS_1_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	SDL_SetRenderDrawColor(render, 100, 180, 100, 0);
	SDL_RenderDrawLine(render, F1RACE_GRASS_1_START_X + 1,
		F1RACE_DISPLAY_START_Y, F1RACE_GRASS_1_START_X + 1, F1RACE_DISPLAY_END_Y - 1);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	SDL_RenderDrawLine(render, F1RACE_GRASS_1_START_X,
		F1RACE_DISPLAY_START_Y, F1RACE_GRASS_1_START_X, F1RACE_DISPLAY_END_Y);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = F1RACE_STATUS_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_STATUS_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	F1Race_DrawBitmap(assets_GAME_F1RACE_LOGO_bmp, assets_GAME_F1RACE_LOGO_bmp_len,
		F1RACE_STATUS_START_X, F1RACE_DISPLAY_START_Y);
	F1Race_DrawBitmap(assets_GAME_F1RACE_STATUS_SCORE_bmp, assets_GAME_F1RACE_STATUS_SCORE_bmp_len,
		F1RACE_STATUS_START_X + 5, F1RACE_DISPLAY_START_Y + 42);
	F1Race_DrawBitmap(assets_GAME_F1RACE_STATUS_BOX_bmp, assets_GAME_F1RACE_STATUS_BOX_bmp_len,
		F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 50);
	F1Race_DrawBitmap(assets_GAME_F1RACE_STATUS_LEVEL_bmp, assets_GAME_F1RACE_STATUS_LEVEL_bmp_len,
		F1RACE_STATUS_START_X + 6, F1RACE_DISPLAY_START_Y + 64);
	F1Race_DrawBitmap(assets_GAME_F1RACE_STATUS_BOX_bmp, assets_GAME_F1RACE_STATUS_BOX_bmp_len,
		F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 72);
	F1Race_DrawBitmap(assets_GAME_F1RACE_STATUS_FLY_bmp, assets_GAME_F1RACE_STATUS_FLY_bmp_len,
		F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 89);
}

static void F1Race_Init(void) {
	int index;
	f1race_key_up_pressed = SDL_FALSE;
	f1race_key_down_pressed = SDL_FALSE;
	f1race_key_right_pressed = SDL_FALSE;
	f1race_key_left_pressed = SDL_FALSE;

	f1race_separator_0_block_start_y = F1RACE_DISPLAY_START_Y;
	f1race_separator_1_block_start_y = F1RACE_DISPLAY_START_Y + F1RACE_SEPARATOR_HEIGHT_SPACE * 3;
	f1race_player_car.pos_x = ((F1RACE_ROAD_1_START_X + F1RACE_ROAD_1_END_X - F1RACE_PLAYER_CAR_IMAGE_SIZE_X) / 2);
	f1race_player_car.dx = F1RACE_PLAYER_CAR_IMAGE_SIZE_X;
	f1race_player_car.pos_y = F1RACE_DISPLAY_END_Y - F1RACE_PLAYER_CAR_IMAGE_SIZE_Y - 1;
	f1race_player_car.dy = F1RACE_PLAYER_CAR_IMAGE_SIZE_Y;
	f1race_player_car.image = assets_GAME_F1RACE_PLAYER_CAR_bmp;
	f1race_player_car.image_fly = assets_GAME_F1RACE_PLAYER_CAR_FLY_bmp;
	f1race_player_car.image_head_light = assets_GAME_F1RACE_PLAYER_CAR_HEAD_LIGHT_bmp;

	f1race_opposite_car_type[0].dx = F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X;
	f1race_opposite_car_type[0].dy = F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_Y;
	f1race_opposite_car_type[0].image = assets_GAME_F1RACE_OPPOSITE_CAR_0_bmp;
	f1race_opposite_car_type[0].length = assets_GAME_F1RACE_OPPOSITE_CAR_0_bmp_len;
	f1race_opposite_car_type[0].speed = 3;
	f1race_opposite_car_type[0].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[1].dx = F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X;
	f1race_opposite_car_type[1].dy = F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_Y;
	f1race_opposite_car_type[1].image = assets_GAME_F1RACE_OPPOSITE_CAR_1_bmp;
	f1race_opposite_car_type[1].length = assets_GAME_F1RACE_OPPOSITE_CAR_1_bmp_len;
	f1race_opposite_car_type[1].speed = 4;
	f1race_opposite_car_type[1].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[2].dx = F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X;
	f1race_opposite_car_type[2].dy = F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_Y;
	f1race_opposite_car_type[2].image = assets_GAME_F1RACE_OPPOSITE_CAR_2_bmp;
	f1race_opposite_car_type[2].length = assets_GAME_F1RACE_OPPOSITE_CAR_2_bmp_len;
	f1race_opposite_car_type[2].speed = 6;
	f1race_opposite_car_type[2].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[3].dx = F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X;
	f1race_opposite_car_type[3].dy = F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_Y;
	f1race_opposite_car_type[3].image = assets_GAME_F1RACE_OPPOSITE_CAR_3_bmp;
	f1race_opposite_car_type[3].length = assets_GAME_F1RACE_OPPOSITE_CAR_3_bmp_len;
	f1race_opposite_car_type[3].speed = 3;
	f1race_opposite_car_type[3].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[4].dx = F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X;
	f1race_opposite_car_type[4].dy = F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_Y;
	f1race_opposite_car_type[4].image = assets_GAME_F1RACE_OPPOSITE_CAR_4_bmp;
	f1race_opposite_car_type[4].length = assets_GAME_F1RACE_OPPOSITE_CAR_4_bmp_len;
	f1race_opposite_car_type[4].speed = 3;
	f1race_opposite_car_type[4].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[5].dx = F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X;
	f1race_opposite_car_type[5].dy = F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_Y;
	f1race_opposite_car_type[5].image = assets_GAME_F1RACE_OPPOSITE_CAR_5_bmp;
	f1race_opposite_car_type[5].length = assets_GAME_F1RACE_OPPOSITE_CAR_5_bmp_len;
	f1race_opposite_car_type[5].speed = 5;
	f1race_opposite_car_type[5].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[6].dx = F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_X;
	f1race_opposite_car_type[6].dy = F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_Y;
	f1race_opposite_car_type[6].image = assets_GAME_F1RACE_OPPOSITE_CAR_6_bmp;
	f1race_opposite_car_type[6].length = assets_GAME_F1RACE_OPPOSITE_CAR_6_bmp_len;
	f1race_opposite_car_type[6].speed = 3;
	f1race_opposite_car_type[6].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_X) / 2;

	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		f1race_opposite_car[index].is_empty = SDL_TRUE;
		f1race_opposite_car[index].is_add_score = SDL_FALSE;
	}

	f1race_is_crashing = SDL_FALSE;
	f1race_last_car_road = 0;
	f1race_player_is_car_fly = SDL_FALSE;
	f1race_score = 0;
	f1race_level = 1;
	f1race_pass = 0;
	f1race_fly_count = 1;
	f1race_fly_charger_count = 0;
}

static void F1Race_Main(void) {
	if (f1race_is_new_game != SDL_FALSE) {
		F1Race_Init();
		f1race_is_new_game = SDL_FALSE;
	}

	/* Render First Frame */
	F1Race_Render_Background();
	F1Race_Render();

	/*
	GFX_OPEN_BACKGROUND_SOUND(F1RaceBackGround, F1RACEBACKGROUND, background_midi);
	GFX_PLAY_BACKGROUND_SOUND(background_midi);

	GFX_OPEN_DUMMY_BACKGROUND_SOUND();
	GFX_PLAY_DUMMY_BACKGROUND_SOUND();
	*/

	// gui_set_clip(0, 0, UI_device_width - 1, UI_device_height - 1);
	// gui_start_timer(F1RACE_TIMER_ELAPSE, F1Race_Cyclic_Timer);
	// gui_BLT_double_buffer(0, 0, UI_device_width - 1, UI_device_height - 1);

	if (f1race_is_crashing == SDL_TRUE) {
		// GFX_PLAY_VIBRATION();
	}
}

static void F1Race_Key_Left_Pressed(void) {
	F1RACE_RELEASE_ALL_KEY;
	f1race_key_left_pressed = SDL_TRUE;
}

static void F1Race_Key_Left_Released(void) {
	f1race_key_left_pressed = SDL_FALSE;
}

static void F1Race_Key_Right_Pressed(void) {
	F1RACE_RELEASE_ALL_KEY;
	f1race_key_right_pressed = SDL_TRUE;
}

static void F1Race_Key_Right_Released(void) {
	f1race_key_right_pressed = SDL_FALSE;
}

static void F1Race_Key_Up_Pressed(void) {
	F1RACE_RELEASE_ALL_KEY;
	f1race_key_up_pressed = SDL_TRUE;
}

static void F1Race_Key_Up_Released(void) {
	f1race_key_up_pressed = SDL_FALSE;
}

static void F1Race_Key_Down_Pressed(void) {
	F1RACE_RELEASE_ALL_KEY;
	f1race_key_down_pressed = SDL_TRUE;
}

static void F1Race_Key_Down_Released(void) {
	f1race_key_down_pressed = SDL_FALSE;
}

static void F1Race_Keyboard_Key_Handler(int32_t vkey_code, int32_t key_state) {
	switch (vkey_code) {
		case SDLK_LEFT:
		case SDLK_KP_4:
			(key_state) ? F1Race_Key_Left_Pressed() : F1Race_Key_Left_Released();
			break;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			(key_state) ? F1Race_Key_Right_Pressed() : F1Race_Key_Right_Released();
			break;
		case SDLK_UP:
		case SDLK_KP_2:
			(key_state) ? F1Race_Key_Up_Pressed() : F1Race_Key_Up_Released();
			break;
		case SDLK_DOWN:
		case SDLK_KP_8:
			(key_state) ? F1Race_Key_Down_Pressed() : F1Race_Key_Down_Released();
			break;

	}
}

int main(SDL_UNUSED int argc, SDL_UNUSED char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("F1 Race",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 150, 150, SDL_WINDOW_SHOWN); /* TODO: Name and SIZE */
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (render == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	F1Race_Main();

	while (!exit_main_loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					exit_main_loop = SDL_TRUE;
					break;
				case SDL_KEYDOWN:
					F1Race_Keyboard_Key_Handler(event.key.keysym.sym, SDL_TRUE);
					break;
				case SDL_KEYUP:
					F1Race_Keyboard_Key_Handler(event.key.keysym.sym, SDL_FALSE);
					break;
			}
		}

		F1Race_Render();

		SDL_RenderPresent(render);
		SDL_Delay(100); // TODO: Delay?
	}

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
