#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define CELL_SIZE 20
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLUMNS SCREEN_WIDTH / CELL_SIZE
#define GRID_WIDTH 1 
#define GRID_COLOR 0x706260
#define ALIVE 1
#define DEAD 0
#define LIVE_COLOR 0xffffff
#define DEAD_COLOR 0x000000
#define TITLE_1 "Game Of Life - Draw on the screen to start"
#define TITLE_2 "Game of Life - Press SPACE to RESET"
typedef struct Cell {
	int x;
	int y;
	int alive;
} Cell;

void draw_grid(SDL_Surface* surface) {
	for (int i = 0; i < COLUMNS; i++) {
		SDL_Rect colmn = {i * CELL_SIZE, 0, GRID_WIDTH, SCREEN_HEIGHT};
		SDL_FillRect(surface, &colmn, GRID_COLOR);
	}
	for (int i = 0; i < ROWS; i++) {
		SDL_Rect row = {0, i * CELL_SIZE, SCREEN_WIDTH, GRID_WIDTH};
		SDL_FillRect(surface, &row, GRID_COLOR);
	}
}

void color_cell(SDL_Surface *surface, int x, int y, int state) {
	int x_coord = x * CELL_SIZE;
	int y_coord = y * CELL_SIZE;
	SDL_Rect rect = {x_coord, y_coord, CELL_SIZE, CELL_SIZE};
	SDL_FillRect(surface, &rect, state == ALIVE ? LIVE_COLOR : DEAD_COLOR);

}

void initialise_cells(SDL_Surface *surface, Cell environment[ROWS][COLUMNS]) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			Cell Cell = {j, i, 0};
			environment[i][j] = Cell;
			color_cell(surface, i, j, DEAD);
		}
	}

}


void simulate(SDL_Surface *surface, Cell environment[ROWS][COLUMNS]) {
	Cell** temp_environment = (Cell** )malloc(ROWS * sizeof(long int));
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
	free(temp_environment);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(TITLE_1,  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_Event event;
	int running = 1, up = 0;
	Cell environment[ROWS][COLUMNS];
	initialise_cells(surface, environment);
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			}	
			if (event.type == SDL_MOUSEMOTION) {
				if (!up && event.motion.state != 0) {
					int cell_x = event.motion.x / CELL_SIZE;
					int cell_y = event.motion.y / CELL_SIZE;
					environment[cell_y][cell_x].x = cell_x;
					environment[cell_y][cell_x].y = cell_y;
					environment[cell_y][cell_x].alive = 1;
					color_cell(surface, cell_x, cell_y, ALIVE);
				}
			}
//			if (!up && event.type == SDL_MOUSEBUTTONUP) up = !up;
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_SPACE) {
					if (up) {
						SDL_SetWindowTitle(window, TITLE_1);
						initialise_cells(surface, environment);
					}
					up = !up;
				}
			}
		}
		if (up) {
			SDL_SetWindowTitle(window, TITLE_2);
			simulate(surface, environment);
		}
		draw_grid(surface);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(up ? 105 : 0);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
