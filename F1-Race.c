/*
 * About:
 *   Port the "F1 Race" game from MTK OS to SDL2 library and Web using Emscripten.
 *
 * Author:
 *   nehochupechatat, OldPhonePreservation, EXL
 *
 * License:
 *   MIT
 *
 * History:
 *   19-Sep-2022: Implemented screen resizing on Phantom Horror request.
 *   16-Sep-2022: Implemented "Game Over" screen.
 *   15-Sep-2022: Added new MIDIs, switching, and mute.
 *   15-Sep-2022: Added Windows support and icons.
 *   14-Sep-2022: Implemented Emscripten support.
 *   14-Sep-2022: Resized texture to x2.
 *   14-Sep-2022: Implemented audio support.
 *   13-Sep-2022: Created initial draft/demo version.
 *
 * Compile commands:
 *   $ clear && clear && gcc F1-Race.c -o F1-Race -lSDL2 -lSDL2_mixer && strip -s F1-Race && ./F1-Race
 *   $ emcc --use-preload-plugins --preload-file assets F1-Race.c -s USE_SDL=2 -s USE_SDL_MIXER=2 -o F1-Race.html
 *
 * Create header file with resources:
 *   $ rm Resources.h ; find assets/ -type f -exec xxd -i {} >> Resources.h \;
 *
 * Convert GIFs to BMPs using ImageMagick and FFmpeg utilities:
 *   $ find -name "*.gif" -exec sh -c 'ffmpeg -i "$1" `basename $1 .gif`.bmp' sh {} \;
 *   $ find -name "*.gif" -exec sh -c 'convert "$1" `basename $1 .gif`.bmp' sh {} \;
 *
 * Convert MIDIs to WAVs and MP3s:
 *   $ timidity *.mid -Ow
 *   $ ffmpeg -i *.wav -acodec pcm_s16le -ar 11025 -ac 1 *_low.wav
 *   $ ffmpeg -i *.wav -ar 44100 -ac 1 -b:a 64k *.mp3
 *   $ ffmpeg -i *.wav -c:a libvorbis -ar 44100 -ac 1 -b:a 64k *.ogg
 */

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define WINDOW_WIDTH                                   (256)
#define WINDOW_HEIGHT                                  (256)
#define TEXTURE_WIDTH                                  (128)
#define TEXTURE_HEIGHT                                 (128)

#define F1RACE_PLAYER_CAR_IMAGE_SIZE_X                 (15)
#define F1RACE_PLAYER_CAR_IMAGE_SIZE_Y                 (20)
#define F1RACE_PLAYER_CAR_CARSH_IMAGE_SIZE_X           (15)
#define F1RACE_PLAYER_CAR_CARSH_IMAGE_SIZE_Y           (25)
#define F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_X             (23)
#define F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_Y             (27)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_IMAGE_SIZE_X      (7)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_IMAGE_SIZE_Y      (15)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_0_SHIFT           (1)
#define F1RACE_PLAYER_CAR_HEAD_LIGHT_1_SHIFT           (7)
#define F1RACE_OPPOSITE_CAR_TYPE_COUNT                 (7)
#define F1RACE_PLAYER_CAR_FLY_FRAME_COUNT              (10)
#define F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X             (17)
#define F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_Y             (35)
#define F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X             (12)
#define F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_Y             (18)
#define F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X             (15)
#define F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_Y             (20)
#define F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X             (12)
#define F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_Y             (18)
#define F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X             (17)
#define F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_Y             (27)
#define F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X             (13)
#define F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_Y             (21)
#define F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_X             (13)
#define F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_Y             (22)
#define F1RACE_OPPOSITE_CAR_COUNT                      (8)
#define F1RACE_OPPOSITE_CAR_DEFAULT_APPEAR_RATE        (2)
#define F1RACE_MAX_FLY_COUNT                           (9)
#define F1RACE_TIMER_ELAPSE                            (100)
#define F1RACE_PLAYER_CAR_SHIFT                        (5)
#define F1RACE_PLAYER_CAR_FLY_SHIFT                    (2)
#define F1RACE_DISPLAY_START_X                         (3)
#define F1RACE_DISPLAY_START_Y                         (3)
#define F1RACE_DISPLAY_END_X                           (124)
#define F1RACE_DISPLAY_END_Y                           (124)
#define F1RACE_ROAD_WIDTH                              (23)
#define F1RACE_SEPARATOR_WIDTH                         (3)
#define F1RACE_GRASS_WIDTH                             (7)
#define F1RACE_STATUS_WIDTH                            (32)
#define F1RACE_SEPARATOR_HEIGHT_SPACE                  (3)
#define F1RACE_SEPARATOR_RATIO                         (6)
#define F1RACE_SEPARATOR_HEIGHT                        (F1RACE_SEPARATOR_HEIGHT_SPACE*F1RACE_SEPARATOR_RATIO)
#define F1RACE_STATUS_NUMBER_WIDTH                     (4)
#define F1RACE_STATUS_NUBBER_HEIGHT                    (7)
#define F1RACE_GRASS_0_START_X                         (F1RACE_DISPLAY_START_X)
#define F1RACE_GRASS_0_END_X                           (F1RACE_GRASS_0_START_X + F1RACE_GRASS_WIDTH)-1
#define F1RACE_ROAD_0_START_X                          (F1RACE_GRASS_0_START_X + F1RACE_GRASS_WIDTH)
#define F1RACE_ROAD_0_END_X                            (F1RACE_ROAD_0_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_SEPARATOR_0_START_X                     (F1RACE_ROAD_0_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_SEPARATOR_0_END_X                       (F1RACE_SEPARATOR_0_START_X + F1RACE_SEPARATOR_WIDTH)-1
#define F1RACE_ROAD_1_START_X                          (F1RACE_SEPARATOR_0_START_X + F1RACE_SEPARATOR_WIDTH)
#define F1RACE_ROAD_1_END_X                            (F1RACE_ROAD_1_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_SEPARATOR_1_START_X                     (F1RACE_ROAD_1_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_SEPARATOR_1_END_X                       (F1RACE_SEPARATOR_1_START_X + F1RACE_SEPARATOR_WIDTH)-1
#define F1RACE_ROAD_2_START_X                          (F1RACE_SEPARATOR_1_START_X + F1RACE_SEPARATOR_WIDTH)
#define F1RACE_ROAD_2_END_X                            (F1RACE_ROAD_2_START_X + F1RACE_ROAD_WIDTH)-1
#define F1RACE_GRASS_1_START_X                         (F1RACE_ROAD_2_START_X + F1RACE_ROAD_WIDTH)
#define F1RACE_GRASS_1_END_X                           (F1RACE_GRASS_1_START_X + F1RACE_GRASS_WIDTH)-1
#define F1RACE_STATUS_START_X                          (F1RACE_GRASS_1_START_X + F1RACE_GRASS_WIDTH)
#define F1RACE_STATUS_END_X                            (F1RACE_STATUS_START_X + F1RACE_STATUS_WIDTH)

#define F1RACE_RELEASE_ALL_KEY {                       \
    f1race_key_up_pressed      = SDL_FALSE;            \
    f1race_key_down_pressed    = SDL_FALSE;            \
    f1race_key_left_pressed    = SDL_FALSE;            \
    f1race_key_right_pressed   = SDL_FALSE;            \
    if (f1race_is_crashing == SDL_TRUE)                \
        return;                                        \
}                                                      \

typedef enum MUSIC_TRACKS {
	MUSIC_BACKGROUND,
	MUSIC_BACKGROUND_LOWCOST,
	MUSIC_CRASH,
	MUSIC_GAMEOVER,
	MUSIC_MAX
} MUSIC_TRACK;
static Mix_Music *music_tracks[MUSIC_MAX] = { NULL };
static Sint32 volume_old = -1;

typedef enum TEXTURES {
	TEXTURE_NUMBER_0,
	TEXTURE_NUMBER_1,
	TEXTURE_NUMBER_2,
	TEXTURE_NUMBER_3,
	TEXTURE_NUMBER_4,
	TEXTURE_NUMBER_5,
	TEXTURE_NUMBER_6,
	TEXTURE_NUMBER_7,
	TEXTURE_NUMBER_8,
	TEXTURE_NUMBER_9,
	TEXTURE_SCREEN,
	TEXTURE_PLAYER_CAR,
	TEXTURE_PLAYER_CAR_FLY,
	TEXTURE_PLAYER_CAR_FLY_UP,
	TEXTURE_PLAYER_CAR_FLY_DOWN,
	TEXTURE_PLAYER_CAR_HEAD_LIGHT,
	TEXTURE_PLAYER_CAR_CRASH,
	TEXTURE_LOGO,
	TEXTURE_STATUS_SCORE,
	TEXTURE_STATUS_BOX,
	TEXTURE_STATUS_LEVEL,
	TEXTURE_STATUS_FLY,
	TEXTURE_OPPOSITE_CAR_0,
	TEXTURE_OPPOSITE_CAR_1,
	TEXTURE_OPPOSITE_CAR_2,
	TEXTURE_OPPOSITE_CAR_3,
	TEXTURE_OPPOSITE_CAR_4,
	TEXTURE_OPPOSITE_CAR_5,
	TEXTURE_OPPOSITE_CAR_6,
	TEXTURE_GAMEOVER,
	TEXTURE_GAMEOVER_FIELD,
	TEXTURE_GAMEOVER_CRASH,
	TEXTURE_MAX
} TEXTURE;
static SDL_Texture *textures[TEXTURE_MAX] = { NULL };

typedef struct {
	Sint16 pos_x;
	Sint16 pos_y;
	Sint16 dx;
	Sint16 dy;
	TEXTURE image;
	TEXTURE image_fly;
	TEXTURE image_head_light;
} F1RACE_CAR_STRUCT;

typedef struct {
	Sint16 dx;
	Sint16 dy;
	Sint16 speed;
	Sint16 dx_from_road;
	TEXTURE image;
} F1RACE_OPPOSITE_CAR_TYPE_STRUCT;

typedef struct {
	Sint16 dx;
	Sint16 dy;
	Sint16 speed;
	Sint16 dx_from_road;
	TEXTURE image;
	Sint16 pos_x;
	Sint16 pos_y;
	Uint8 road_id;
	SDL_bool is_empty;
	SDL_bool is_add_score;
} F1RACE_OPPOSITE_CAR_STRUCT;

#ifdef __EMSCRIPTEN__
typedef struct {
	SDL_Texture *texture;
} CONTEXT_EMSCRIPTEN;
#endif

static SDL_bool exit_main_loop = SDL_FALSE;
static SDL_bool using_new_background_ogg = SDL_FALSE;
static SDL_Renderer *render = NULL;

static SDL_bool f1race_is_new_game = SDL_TRUE;
static SDL_bool f1race_is_crashing = SDL_FALSE;
static Sint16 f1race_crashing_count_down;
static Sint16 f1race_separator_0_block_start_y;
static Sint16 f1race_separator_1_block_start_y;
static Sint16 f1race_last_car_road;
static SDL_bool f1race_player_is_car_fly;
static Sint16 f1race_player_car_fly_duration;
static Sint16 f1race_score;
static Sint16 f1race_level;
static Sint16 f1race_pass;
static Sint16 f1race_fly_count;
static Sint16 f1race_fly_charger_count;

static SDL_bool f1race_key_up_pressed = SDL_FALSE;
static SDL_bool f1race_key_down_pressed = SDL_FALSE;
static SDL_bool f1race_key_right_pressed = SDL_FALSE;
static SDL_bool f1race_key_left_pressed = SDL_FALSE;

static F1RACE_CAR_STRUCT f1race_player_car;
static F1RACE_OPPOSITE_CAR_TYPE_STRUCT f1race_opposite_car_type[F1RACE_OPPOSITE_CAR_TYPE_COUNT];
static F1RACE_OPPOSITE_CAR_STRUCT f1race_opposite_car[F1RACE_OPPOSITE_CAR_COUNT];

static void Music_Load(void) {
	music_tracks[MUSIC_BACKGROUND] = Mix_LoadMUS("assets/GAME_F1RACE_BGM.ogg");
	music_tracks[MUSIC_BACKGROUND_LOWCOST] = Mix_LoadMUS("assets/GAME_F1RACE_BGM_LOWCOST.ogg");
	music_tracks[MUSIC_CRASH] = Mix_LoadMUS("assets/GAME_F1RACE_CRASH.ogg");
	music_tracks[MUSIC_GAMEOVER] = Mix_LoadMUS("assets/GAME_F1RACE_GAMEOVER.ogg");
}

static void Music_Play(MUSIC_TRACK track, Sint32 loop) {
	Mix_PlayMusic(music_tracks[track], loop);
}

static void Music_Unload(void) {
	int i = 0;
	for (; i < MUSIC_MAX; ++i)
		if (music_tracks[i])
			Mix_FreeMusic(music_tracks[i]);
}

static void Texture_Create_Bitmap(const char *filepath, TEXTURE texture_id) {
	SDL_Surface *bitmap = SDL_LoadBMP(filepath);
	textures[texture_id] = SDL_CreateTextureFromSurface(render, bitmap);
	SDL_FreeSurface(bitmap);
}

static void Texture_Load(void) {
	textures[TEXTURE_SCREEN] =
		SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_0.bmp", TEXTURE_NUMBER_0);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_1.bmp", TEXTURE_NUMBER_1);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_2.bmp", TEXTURE_NUMBER_2);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_3.bmp", TEXTURE_NUMBER_3);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_4.bmp", TEXTURE_NUMBER_4);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_5.bmp", TEXTURE_NUMBER_5);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_6.bmp", TEXTURE_NUMBER_6);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_7.bmp", TEXTURE_NUMBER_7);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_8.bmp", TEXTURE_NUMBER_8);
	Texture_Create_Bitmap("assets/GAME_F1RACE_NUMBER_9.bmp", TEXTURE_NUMBER_9);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR.bmp", TEXTURE_PLAYER_CAR);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR_FLY.bmp", TEXTURE_PLAYER_CAR_FLY);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR_FLY_UP.bmp", TEXTURE_PLAYER_CAR_FLY_UP);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR_FLY_DOWN.bmp", TEXTURE_PLAYER_CAR_FLY_DOWN);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR_HEAD_LIGHT.bmp", TEXTURE_PLAYER_CAR_HEAD_LIGHT);
	Texture_Create_Bitmap("assets/GAME_F1RACE_PLAYER_CAR_CRASH.bmp", TEXTURE_PLAYER_CAR_CRASH);
	Texture_Create_Bitmap("assets/GAME_F1RACE_LOGO.bmp", TEXTURE_LOGO);
	Texture_Create_Bitmap("assets/GAME_F1RACE_STATUS_SCORE.bmp", TEXTURE_STATUS_SCORE);
	Texture_Create_Bitmap("assets/GAME_F1RACE_STATUS_BOX.bmp", TEXTURE_STATUS_BOX);
	Texture_Create_Bitmap("assets/GAME_F1RACE_STATUS_LEVEL.bmp", TEXTURE_STATUS_LEVEL);
	Texture_Create_Bitmap("assets/GAME_F1RACE_STATUS_FLY.bmp", TEXTURE_STATUS_FLY);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_0.bmp", TEXTURE_OPPOSITE_CAR_0);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_1.bmp", TEXTURE_OPPOSITE_CAR_1);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_2.bmp", TEXTURE_OPPOSITE_CAR_2);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_3.bmp", TEXTURE_OPPOSITE_CAR_3);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_4.bmp", TEXTURE_OPPOSITE_CAR_4);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_5.bmp", TEXTURE_OPPOSITE_CAR_5);
	Texture_Create_Bitmap("assets/GAME_F1RACE_OPPOSITE_CAR_6.bmp", TEXTURE_OPPOSITE_CAR_6);
	Texture_Create_Bitmap("assets/GAME_F1RACE_GAMEOVER.bmp", TEXTURE_GAMEOVER);
	Texture_Create_Bitmap("assets/GAME_F1RACE_GAMEOVER_CRASH.bmp", TEXTURE_GAMEOVER_CRASH);
	Texture_Create_Bitmap("assets/GAME_F1RACE_GAMEOVER_FIELD.bmp", TEXTURE_GAMEOVER_FIELD);
}

static void Texture_Draw(Sint32 x, Sint32 y, TEXTURE texture_id) {
	SDL_Rect rectangle;
	rectangle.x = x;
	rectangle.y = y;
	SDL_QueryTexture(textures[texture_id], NULL, NULL, &rectangle.w, &rectangle.h);
	SDL_RenderCopy(render, textures[texture_id], NULL, &rectangle);
}

static void Texture_Unload(void) {
	int i = 0;
	for (; i < TEXTURE_MAX; ++i)
		if (textures[i])
			SDL_DestroyTexture(textures[i]);
}

static void F1Race_Render_Score(Sint16 x_pos, Sint16 y_pos);

static void F1Race_Show_Game_Over_Screen(void) {
	SDL_SetRenderDrawColor(render, 234, 243, 255, 0); // Light Blue.
	SDL_RenderClear(render);

	Texture_Draw(18, 10, TEXTURE_GAMEOVER);
	Texture_Draw(30, 40, TEXTURE_GAMEOVER_FIELD);

	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = 33;
	rectangle.y = 43;
	rectangle.w = 64;
	rectangle.h = 20;
	SDL_RenderFillRect(render, &rectangle);

	Texture_Draw(36, 50, TEXTURE_STATUS_SCORE);
	Texture_Draw(65, 48, TEXTURE_STATUS_BOX);

	F1Race_Render_Score(64, -2);

	Texture_Draw(47, 80, TEXTURE_GAMEOVER_CRASH);
}

static void F1Race_Render_Separator(void) {
	Sint16 start_y, end_y;

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

static void F1Race_Render_Score(Sint16 x_pos, Sint16 y_pos) {
	Sint16 value;
	Sint16 remain;

	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = x_pos + 4;
	rectangle.y = y_pos + 52;
	rectangle.w = x_pos + 29 + 1 - rectangle.x;
	rectangle.h = y_pos + 58 - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	value = f1race_score % 10;
	remain = f1race_score / 10;

	while (SDL_TRUE) {
		Texture_Draw(x_pos + 25, y_pos + 52, value);

		x_pos -= 5;
		if (remain > 0) {
			value = remain % 10;
			remain = remain / 10;
		}
		else
			break;
	}
}

static void F1Race_Render_Status(void) {
	Sint16 x_pos;
	Sint16 y_pos;
	Sint16 index;

	F1Race_Render_Score(F1RACE_STATUS_START_X, F1RACE_DISPLAY_START_Y);

	SDL_Rect rectangle;
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	rectangle.x = F1RACE_STATUS_START_X + 4;
	rectangle.y = F1RACE_DISPLAY_START_Y + 74;
	rectangle.w = F1RACE_STATUS_START_X + 29 + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_START_Y + 80 - rectangle.y;
	SDL_RenderFillRect(render, &rectangle);

	x_pos = F1RACE_STATUS_START_X + 16;
	y_pos = F1RACE_DISPLAY_START_Y + 74;

	Texture_Draw(x_pos, y_pos, f1race_level);

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

	x_pos = F1RACE_STATUS_START_X + 25;
	y_pos = F1RACE_DISPLAY_START_Y + 96;
	Texture_Draw(x_pos, y_pos, f1race_fly_count);
}

static void F1Race_Render_Player_Car(void) {
	Sint16 dx;
	Sint16 dy;

	TEXTURE image;

	if (f1race_player_is_car_fly == SDL_FALSE)
		Texture_Draw(f1race_player_car.pos_x, f1race_player_car.pos_y, TEXTURE_PLAYER_CAR);
	else {
		dx = (F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_X - F1RACE_PLAYER_CAR_IMAGE_SIZE_X) / 2;
		dy = (F1RACE_PLAYER_CAR_FLY_IMAGE_SIZE_Y - F1RACE_PLAYER_CAR_IMAGE_SIZE_Y) / 2;
		dx = f1race_player_car.pos_x - dx;
		dy = f1race_player_car.pos_y - dy;
		switch (f1race_player_car_fly_duration) {
			case 0:
			case 1:
				image = TEXTURE_PLAYER_CAR_FLY_UP;
				break;
			case (F1RACE_PLAYER_CAR_FLY_FRAME_COUNT - 1):
			case (F1RACE_PLAYER_CAR_FLY_FRAME_COUNT - 2):
				image = TEXTURE_PLAYER_CAR_FLY_DOWN;
				break;
			default:
				image = TEXTURE_PLAYER_CAR_FLY;
				break;
		}
		Texture_Draw(dx, dy, image);
	}
}

static void F1Race_Render_Opposite_Car(void) {
	Sint16 index;
	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		if (f1race_opposite_car[index].is_empty == SDL_FALSE)
			Texture_Draw(f1race_opposite_car[index].pos_x, f1race_opposite_car[index].pos_y,
				f1race_opposite_car[index].image);
	}
}

static void F1Race_Render_Player_Car_Crash(void) {
	Texture_Draw(f1race_player_car.pos_x, f1race_player_car.pos_y - 5, TEXTURE_PLAYER_CAR_CRASH);
}

static void F1Race_Render(void) {
	SDL_Rect rectangle;
	rectangle.x = F1RACE_STATUS_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_STATUS_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderSetClipRect(render, &rectangle);

	F1Race_Render_Status();

	rectangle.x = F1RACE_ROAD_0_START_X;
	rectangle.y = F1RACE_DISPLAY_START_Y;
	rectangle.w = F1RACE_ROAD_2_END_X + 1 - rectangle.x;
	rectangle.h = F1RACE_DISPLAY_END_Y - rectangle.y;
	SDL_RenderSetClipRect(render, &rectangle);

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

	Texture_Draw(F1RACE_STATUS_START_X + 0, F1RACE_DISPLAY_START_Y +  0, TEXTURE_LOGO);
	Texture_Draw(F1RACE_STATUS_START_X + 5, F1RACE_DISPLAY_START_Y + 42, TEXTURE_STATUS_SCORE);
	Texture_Draw(F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 50, TEXTURE_STATUS_BOX);
	Texture_Draw(F1RACE_STATUS_START_X + 6, F1RACE_DISPLAY_START_Y + 64, TEXTURE_STATUS_LEVEL);
	Texture_Draw(F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 72, TEXTURE_STATUS_BOX);
	Texture_Draw(F1RACE_STATUS_START_X + 2, F1RACE_DISPLAY_START_Y + 89, TEXTURE_STATUS_FLY);
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
	f1race_player_car.image = TEXTURE_PLAYER_CAR;
	f1race_player_car.image_fly = TEXTURE_PLAYER_CAR_FLY;
	f1race_player_car.image_head_light = TEXTURE_PLAYER_CAR_HEAD_LIGHT;

	f1race_opposite_car_type[0].dx = F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X;
	f1race_opposite_car_type[0].dy = F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_Y;
	f1race_opposite_car_type[0].image = TEXTURE_OPPOSITE_CAR_0;
	f1race_opposite_car_type[0].speed = 3;
	f1race_opposite_car_type[0].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_0_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[1].dx = F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X;
	f1race_opposite_car_type[1].dy = F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_Y;
	f1race_opposite_car_type[1].image = TEXTURE_OPPOSITE_CAR_1;
	f1race_opposite_car_type[1].speed = 4;
	f1race_opposite_car_type[1].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_1_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[2].dx = F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X;
	f1race_opposite_car_type[2].dy = F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_Y;
	f1race_opposite_car_type[2].image = TEXTURE_OPPOSITE_CAR_2;
	f1race_opposite_car_type[2].speed = 6;
	f1race_opposite_car_type[2].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_2_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[3].dx = F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X;
	f1race_opposite_car_type[3].dy = F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_Y;
	f1race_opposite_car_type[3].image = TEXTURE_OPPOSITE_CAR_3;
	f1race_opposite_car_type[3].speed = 3;
	f1race_opposite_car_type[3].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_3_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[4].dx = F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X;
	f1race_opposite_car_type[4].dy = F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_Y;
	f1race_opposite_car_type[4].image = TEXTURE_OPPOSITE_CAR_4;
	f1race_opposite_car_type[4].speed = 3;
	f1race_opposite_car_type[4].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_4_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[5].dx = F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X;
	f1race_opposite_car_type[5].dy = F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_Y;
	f1race_opposite_car_type[5].image = TEXTURE_OPPOSITE_CAR_5;
	f1race_opposite_car_type[5].speed = 5;
	f1race_opposite_car_type[5].dx_from_road = (F1RACE_ROAD_WIDTH - F1RACE_OPPOSITE_CAR_5_IMAGE_SIZE_X) / 2;

	f1race_opposite_car_type[6].dx = F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_X;
	f1race_opposite_car_type[6].dy = F1RACE_OPPOSITE_CAR_6_IMAGE_SIZE_Y;
	f1race_opposite_car_type[6].image = TEXTURE_OPPOSITE_CAR_6;
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

	F1Race_Render_Background();
	F1Race_Render();

	if (using_new_background_ogg)
		Music_Play(MUSIC_BACKGROUND, -1);
	else
		Music_Play(MUSIC_BACKGROUND_LOWCOST, -1);
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

static void F1Race_Key_Fly_Pressed(void) {
	if (f1race_player_is_car_fly != SDL_FALSE)
		return;

	if (f1race_fly_count > 0) {
		f1race_player_is_car_fly = SDL_TRUE;
		f1race_player_car_fly_duration = 0;
		f1race_fly_count--;
	}
}

static void F1Race_Keyboard_Key_Handler(Sint32 vkey_code, Sint32 key_state) {
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
		case SDLK_2:
		case SDLK_KP_8:
			(key_state) ? F1Race_Key_Up_Pressed() : F1Race_Key_Up_Released();
			break;
		case SDLK_DOWN:
		case SDLK_8:
		case SDLK_KP_2:
			(key_state) ? F1Race_Key_Down_Pressed() : F1Race_Key_Down_Released();
			break;
		case SDLK_SPACE:
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_5:
		case SDLK_KP_5:
			if (key_state)
				F1Race_Key_Fly_Pressed();
			break;
		case SDLK_n:
		case SDLK_TAB:
		case SDLK_0:
		case SDLK_KP_0:
			if (key_state) {
				if (!using_new_background_ogg)
					Music_Play(MUSIC_BACKGROUND, -1);
				else
					Music_Play(MUSIC_BACKGROUND_LOWCOST, -1);
				using_new_background_ogg = !using_new_background_ogg;
			}
			break;
		case SDLK_m:
		case SDLK_7:
		case SDLK_KP_7:
			if (key_state) {
				if (volume_old == -1)
					volume_old = Mix_VolumeMusic(0);
				else {
					Mix_VolumeMusic(volume_old);
					volume_old = -1;
				}
			}
			break;
		case SDLK_ESCAPE:
			if (key_state)
				exit_main_loop = SDL_TRUE;
			break;
	}
}

/* === LOGIC CODE === */

static void F1Race_Crashing(void) {
	Music_Play(MUSIC_CRASH, 0);

	f1race_is_crashing = SDL_TRUE;
	f1race_crashing_count_down = 50;
}

static void F1Race_New_Opposite_Car(void) {
	Sint16 index;
	Sint16 validIndex = 0;
	Sint16 no_slot;
	Sint16 car_type = 0;
	Uint8 road;
	Sint16 car_pos_x = 0;
	Sint16 car_shift;
	Sint16 enough_space;
	Sint16 rand_num;
	Sint16 speed_add;

	no_slot = SDL_TRUE;
	if ((rand() % F1RACE_OPPOSITE_CAR_DEFAULT_APPEAR_RATE) == 0) {
		for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
			if (f1race_opposite_car[index].is_empty != SDL_FALSE) {
				validIndex = index;
				no_slot = SDL_FALSE;
				break;
			}
		}
	}

	if (no_slot != SDL_FALSE)
		return;

	road = rand() % 3;

	if (road == f1race_last_car_road) {
		road++;
		road %= 3;
	}

	if (f1race_level < 3) {
		rand_num = rand() % 11;
		switch (rand_num) {
			case 0:
			case 1:
				car_type = 0;
				break;
			case 2:
			case 3:
			case 4:
				car_type = 1;
				break;
			case 5:
				car_type = 2;
				break;
			case 6:
			case 7:
				car_type = 3;
				break;
			case 8:
				car_type = 4;
				break;
			case 9:
				car_type = 5;
				break;
			case 10:
				car_type = 6;
				break;
		}
	}

	if (f1race_level >= 3) {
		rand_num = rand() % 11;
		switch (rand_num) {
			case 0:
				car_type = 0;
				break;
			case 1:
			case 2:
				car_type = 1;
				break;
			case 3:
			case 4:
				car_type = 2;
				break;
			case 5:
			case 6:
				car_type = 3;
				break;
			case 7:
				car_type = 4;
				break;
			case 8:
			case 9:
				car_type = 5;
				break;
			case 10:
				car_type = 6;
				break;
		}
	}
	enough_space = SDL_TRUE;
	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		if ((f1race_opposite_car[index].is_empty == SDL_FALSE) &&
			(f1race_opposite_car[index].pos_y < (F1RACE_PLAYER_CAR_IMAGE_SIZE_Y * 1.5)))
			enough_space = SDL_FALSE;
	}

	if (enough_space == SDL_FALSE)
		return;

	speed_add = f1race_level - 1;

	f1race_opposite_car[validIndex].is_empty = SDL_FALSE;
	f1race_opposite_car[validIndex].is_add_score = SDL_FALSE;
	f1race_opposite_car[validIndex].dx = f1race_opposite_car_type[car_type].dx;
	f1race_opposite_car[validIndex].dy = f1race_opposite_car_type[car_type].dy;
	f1race_opposite_car[validIndex].speed = f1race_opposite_car_type[car_type].speed + speed_add;
	f1race_opposite_car[validIndex].dx_from_road = f1race_opposite_car_type[car_type].dx_from_road;
	f1race_opposite_car[validIndex].image = f1race_opposite_car_type[car_type].image;

	car_shift = f1race_opposite_car[validIndex].dx_from_road;

	switch (road) {
	case 0:
		car_pos_x = F1RACE_ROAD_0_START_X + car_shift;
		break;
	case 1:
		car_pos_x = F1RACE_ROAD_1_START_X + car_shift;
		break;
	case 2:
		car_pos_x = F1RACE_ROAD_2_START_X + car_shift;
		break;
	}

	f1race_opposite_car[validIndex].pos_x = car_pos_x;
	f1race_opposite_car[validIndex].pos_y = F1RACE_DISPLAY_START_Y - f1race_opposite_car[validIndex].dy;
	f1race_opposite_car[validIndex].road_id = road;

	f1race_last_car_road = road;
}

static void F1Race_CollisionCheck(void) {
	Sint16 index;
	Sint16 minA_x, minA_y, maxA_x, maxA_y;
	Sint16 minB_x, minB_y, maxB_x, maxB_y;

	minA_x = f1race_player_car.pos_x - 1;
	maxA_x = minA_x + f1race_player_car.dx - 1;
	minA_y = f1race_player_car.pos_y - 1;
	maxA_y = minA_y + f1race_player_car.dy - 1;

	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		if (f1race_opposite_car[index].is_empty == SDL_FALSE) {
			minB_x = f1race_opposite_car[index].pos_x - 1;
			maxB_x = minB_x + f1race_opposite_car[index].dx - 1;
			minB_y = f1race_opposite_car[index].pos_y - 1;
			maxB_y = minB_y + f1race_opposite_car[index].dy - 1;
			if (((minA_x <= minB_x) && (minB_x <= maxA_x)) || ((minA_x <= maxB_x) && (maxB_x <= maxA_x))) {
				if (((minA_y <= minB_y) && (minB_y <= maxA_y)) || ((minA_y <= maxB_y) && (maxB_y <= maxA_y))) {
					F1Race_Crashing();
					return;
				}
			}

			if ((minA_x >= minB_x) && (minA_x <= maxB_x) && (minA_y >= minB_y) && (minA_y <= maxB_y)) {
				F1Race_Crashing();
				return;
			}

			if ((minA_x >= minB_x) && (minA_x <= maxB_x) && (maxA_y >= minB_y) && (maxA_y <= maxB_y)) {
				F1Race_Crashing();
				return;
			}

			if ((maxA_x >= minB_x) && (maxA_x <= maxB_x) && (minA_y >= minB_y) && (minA_y <= maxB_y)) {
				F1Race_Crashing();
				return;
			}

			if ((maxA_x >= minB_x) && (maxA_x <= maxB_x) && (maxA_y >= minB_y) && (maxA_y <= maxB_y)) {
				F1Race_Crashing();
				return;
			}

			if ((maxA_y < minB_y) && (f1race_opposite_car[index].is_add_score == SDL_FALSE)) {
				f1race_score++;
				f1race_pass++;
				f1race_opposite_car[index].is_add_score = SDL_TRUE;

				if (f1race_pass == 10)
					f1race_level++; /* level 2 */
				else if (f1race_pass == 20)
					f1race_level++; /* level 3 */
				else if (f1race_pass == 30)
					f1race_level++; /* level 4 */
				else if (f1race_pass == 40)
					f1race_level++; /* level 5 */
				else if (f1race_pass == 50)
					f1race_level++; /* level 6 */
				else if (f1race_pass == 60)
					f1race_level++; /* level 7 */
				else if (f1race_pass == 70)
					f1race_level++; /* level 8 */
				else if (f1race_pass == 100)
					f1race_level++; /* level 9 */

				f1race_fly_charger_count++;
				if (f1race_fly_charger_count >= 6) {
					if (f1race_fly_count < F1RACE_MAX_FLY_COUNT) {
						f1race_fly_charger_count = 0;
						f1race_fly_count++;
					} else
						f1race_fly_charger_count--;
				}
			}
		}
	}
}

static void F1Race_Framemove(void) {
	Sint16 shift;
	Sint16 max;
	Sint16 index;

	f1race_player_car_fly_duration++;
	if (f1race_player_car_fly_duration == F1RACE_PLAYER_CAR_FLY_FRAME_COUNT)
		f1race_player_is_car_fly = SDL_FALSE;

	shift = F1RACE_PLAYER_CAR_SHIFT;
	if (f1race_key_up_pressed) {
		if (f1race_player_car.pos_y - shift < F1RACE_DISPLAY_START_Y)
			shift = f1race_player_car.pos_y - F1RACE_DISPLAY_START_Y - 1;
		if (f1race_player_is_car_fly == SDL_FALSE)
			f1race_player_car.pos_y -= shift;
	}

	if (f1race_key_down_pressed) {
		max = f1race_player_car.pos_y + f1race_player_car.dy;
		if (max + shift > F1RACE_DISPLAY_END_Y)
			shift = F1RACE_DISPLAY_END_Y - max;
		if (f1race_player_is_car_fly == SDL_FALSE)
			f1race_player_car.pos_y += shift;
	}

	if (f1race_key_right_pressed) {
		max = f1race_player_car.pos_x + f1race_player_car.dx;
		if (max + shift > F1RACE_ROAD_2_END_X)
			shift = F1RACE_ROAD_2_END_X - max;
		f1race_player_car.pos_x += shift;
	}

	if (f1race_key_left_pressed) {
		if (f1race_player_car.pos_x - shift < F1RACE_ROAD_0_START_X)
			shift = f1race_player_car.pos_x - F1RACE_ROAD_0_START_X - 1;
		f1race_player_car.pos_x -= shift;
	}

	for (index = 0; index < F1RACE_OPPOSITE_CAR_COUNT; index++) {
		if (f1race_opposite_car[index].is_empty == SDL_FALSE) {
			f1race_opposite_car[index].pos_y += f1race_opposite_car[index].speed;
			if (f1race_opposite_car[index].pos_y > (F1RACE_DISPLAY_END_Y + f1race_opposite_car[index].dy))
				f1race_opposite_car[index].is_empty = SDL_TRUE;
		}
	}

	if (f1race_player_is_car_fly != SDL_FALSE) {
		shift = F1RACE_PLAYER_CAR_FLY_SHIFT;
		if (f1race_player_car.pos_y - shift < F1RACE_DISPLAY_START_Y)
			shift = f1race_player_car.pos_y - F1RACE_DISPLAY_START_Y - 1;
		f1race_player_car.pos_y -= shift;
	} else
		F1Race_CollisionCheck();

	F1Race_New_Opposite_Car();
}

/* === END LOGIC CODE === */

static void F1Race_Cyclic_Timer(void) {
	if (f1race_is_crashing == SDL_FALSE) {
		F1Race_Framemove();
		F1Race_Render();
	} else {
		f1race_crashing_count_down--;
		if (f1race_crashing_count_down >= 40)
			F1Race_Render_Player_Car_Crash();
		else {
			if (f1race_crashing_count_down == 39)
				Music_Play(MUSIC_GAMEOVER, 0);
			F1Race_Show_Game_Over_Screen();
		}
		if (f1race_crashing_count_down <= 0) {
			f1race_is_crashing = SDL_FALSE;
			f1race_is_new_game = SDL_TRUE;
			F1Race_Main();
		}
	}
}

static void main_loop(SDL_Texture *texture) {
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
	SDL_SetRenderTarget(render, texture);
	F1Race_Cyclic_Timer();
	SDL_SetRenderTarget(render, NULL);
	SDL_Rect rectangle;
	rectangle.x = 0;
	rectangle.y = 0;
	rectangle.w = WINDOW_WIDTH;
	rectangle.h = WINDOW_HEIGHT;
	SDL_RenderCopy(render, texture, &rectangle, NULL);
	SDL_RenderPresent(render);
}

#ifdef __EMSCRIPTEN__
static void main_loop_emscripten(void *arguments) {
	CONTEXT_EMSCRIPTEN *context = arguments;
	main_loop(context->texture);
}
#endif

int main(SDL_UNUSED int argc, SDL_UNUSED char *argv[]) {
	srand(time(0));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "SDL_Init Error: %s.\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Enable smoothing for old-style pixel-art game.
	// SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Window *window = SDL_CreateWindow("F1 Race",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s.\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface *icon = SDL_LoadBMP("assets/GAME_F1RACE_ICON.bmp");
	if (icon == NULL)
		fprintf(stderr, "SDL_LoadBMP Error: %s.\n", SDL_GetError());
	else {
		SDL_SetColorKey(icon, SDL_TRUE, SDL_MapRGB(icon->format, 36, 227, 113)); // Icon transparent mask.
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	}

	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (render == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s.\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	Texture_Load();

	int result = Mix_Init(MIX_INIT_OGG);
	if (result != MIX_INIT_OGG) {
		fprintf(stderr, "Mix_Init Error: %s.\n", Mix_GetError());
		return EXIT_FAILURE;
	}
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 4096) == -1) {
		fprintf(stderr, "Mix_OpenAudio Error: %s.\n", Mix_GetError());
		return EXIT_FAILURE;
	}

	Music_Load();

	SDL_SetRenderTarget(render, textures[TEXTURE_SCREEN]);
	SDL_RenderClear(render);
	F1Race_Main();
	SDL_SetRenderTarget(render, NULL);

#ifndef __EMSCRIPTEN__
	while (!exit_main_loop) {
		main_loop(textures[TEXTURE_SCREEN]);
		SDL_Delay(F1RACE_TIMER_ELAPSE); // 10 FPS.
	}
#else
	CONTEXT_EMSCRIPTEN context;
	context.texture = textures[TEXTURE_SCREEN];
	emscripten_set_main_loop_arg(main_loop_emscripten, &context, 10, 1); // 10 FPS.
#endif

	Mix_CloseAudio();
	Music_Unload();
	Texture_Unload();

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
