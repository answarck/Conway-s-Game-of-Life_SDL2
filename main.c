#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define CELL_SIZE 5
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLUMNS SCREEN_WIDTH / CELL_SIZE
#define GRID_WIDTH 1 
#define GRID_COLOR 0x706260
#define ALIVE 1
#define DEAD 0
#define LIVE_COLOR 0xffffff
#define DEAD_COLOR 0x000000
#define TITLE_DRAW "Game Of Life - Mode: Draw (Press space to start)"
#define TITLE_RESET "Game of Life - Press SPACE to RESET"
#define TITLE_DELETE "Game of Life - Mode: Delete (Press space to start)"
typedef struct Cell {
	int x;
	int y;
	int alive;
} Cell;

void free_environment(Cell** environment) {
	for (int i = 0; i < ROWS; i++) free(environment[i]);
	free(environment);
}
Cell** create_environment() {
	Cell** environment = (Cell** )malloc(ROWS * sizeof(long int));
	for (int i = 0; i < ROWS; i++) environment[i] = (Cell* )malloc(COLUMNS * sizeof(Cell));
	return environment;
}

void draw_grid(SDL_Surface* surface, Uint32 color) {
	for (int i = 0; i < ROWS; i++) {
		SDL_Rect colmn = {i * CELL_SIZE, 0, GRID_WIDTH, SCREEN_HEIGHT};
		SDL_FillRect(surface, &colmn, color);
	}
	for (int i = 0; i < ROWS; i++) {
		SDL_Rect row = {0, i * CELL_SIZE, SCREEN_WIDTH, GRID_WIDTH};
		SDL_FillRect(surface, &row, color);
	}
}

void color_cell(SDL_Surface *surface, int x, int y, int state) {
	int x_coord = x * CELL_SIZE;
	int y_coord = y * CELL_SIZE;
	SDL_Rect rect = {x_coord, y_coord, CELL_SIZE, CELL_SIZE};
	SDL_FillRect(surface, &rect, state == ALIVE ? LIVE_COLOR : DEAD_COLOR);

}

void initialise_cells(SDL_Surface *surface, Cell** environment) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			Cell Cell = {j, i, DEAD};
			environment[i][j] = Cell;
			color_cell(surface, i, j, DEAD);
		}
	}

}


void simulate(SDL_Surface *surface, Cell** environment) {
	Cell** temp_environment = create_environment();
	for (int i = 0; i < ROWS; i++) {
		temp_environment[i] = (Cell* )malloc(COLUMNS * sizeof(Cell));
		for (int j = 0; j < COLUMNS; j++) {
			temp_environment[i][j] = environment[i][j];
		}
	}
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			int alive = 0;
			Cell cell = temp_environment[i][j];
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue;
					int ni = i + dx, nj = j + dy;
					if ((ni >= 0 && ni < ROWS) && (nj >= 0 && nj < COLUMNS)) alive += temp_environment[ni][nj].alive;
				}
			}
			if (cell.alive) {
				if (alive < 2) environment[i][j].alive = 0; 
				else if (alive > 3) environment[i][j].alive = 0; 
			}
			else {
				if (alive == 3) environment[i][j].alive = 1;
			}
		}
	}	
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			color_cell(surface, environment[i][j].x, environment[i][j].y, environment[i][j].alive ? ALIVE : DEAD);
		}
	}
	free_environment(temp_environment);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(TITLE_DRAW,  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_Event event;
	int running = 1, up = 0, grid = 0, delete = 0;
	Cell** environment = create_environment();
	initialise_cells(surface, environment);
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = 0;
			if (event.type == SDL_MOUSEMOTION) {
				if (!up && event.motion.state != 0) {
					SDL_MouseMotionEvent pos = event.motion;
					if ((pos.x >= 0 && pos.x <= SCREEN_WIDTH) && (pos.y >= 0 && pos.y <= SCREEN_HEIGHT)) {
						int cell_x = event.motion.x / CELL_SIZE;
						int cell_y = event.motion.y / CELL_SIZE;
						environment[cell_y][cell_x].x = cell_x;
						environment[cell_y][cell_x].y = cell_y;
						environment[cell_y][cell_x].alive = delete ? 0 : 1;
						color_cell(surface, cell_x, cell_y, delete ? DEAD : ALIVE);
					}
				}
			}
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_SPACE) {
					if (up) {
						SDL_SetWindowTitle(window, TITLE_DRAW);
						initialise_cells(surface, environment);
						delete = 0;
					}
					else SDL_SetWindowTitle(window, TITLE_RESET);
					up = !up;
				}
				if (event.key.keysym.sym == SDLK_g) {
					grid = !grid;
					draw_grid(surface, 0x0000);
				}
				if (event.key.keysym.sym == SDLK_d) {
					delete = !delete;
					if (delete) SDL_SetWindowTitle(window, TITLE_DELETE);
					else SDL_SetWindowTitle(window, TITLE_DRAW);
				}
			}
		}
		if (up) simulate(surface, environment);
		if (grid) draw_grid(surface, GRID_COLOR);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(up ? 105 : 0);
	}
	free_environment(environment);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
