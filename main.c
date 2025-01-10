#include <stdio.h>
#include <SDL2/SDL.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define CELL_SIZE 5
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLUMNS SCREEN_WIDTH / CELL_SIZE
#define LINE_WIDTH 1
#define LINE_COLOR 0x706260
#define ALIVE 1
#define DEAD 0
#define LIVE_COLOR 0xffffff
#define DEAD_COLOR 0x000000

typedef struct Cell {
	int x;
	int y;
	int alive;
} Cell;

void draw_grid(SDL_Surface* surface) {
	for (int i = 0; i < COLUMNS; i++) {
		SDL_Rect colmn = {i * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT};
		SDL_FillRect(surface, &colmn, LINE_COLOR);
	}
	for (int i = 0; i < ROWS; i++) {
		SDL_Rect row = {0, i * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH};
		SDL_FillRect(surface, &row, LINE_COLOR);
	}
}

void initialise_cells(Cell environment[ROWS * COLUMNS]) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			Cell Cell = {i, j, 0};
			environment[j + COLUMNS * i] = Cell;
		}
	}

}


void color_cell(SDL_Surface *surface, int x, int y, int state) {
	int x_coord = x * CELL_SIZE;
	int y_coord = y * CELL_SIZE;
	SDL_Rect rect = {x_coord, y_coord, CELL_SIZE, CELL_SIZE};
	SDL_FillRect(surface, &rect, state == ALIVE ? LIVE_COLOR : DEAD_COLOR);

}

void simulate(SDL_Surface *surface, Cell environment[ROWS * COLUMNS]) {
	Cell temp_environment[ROWS * COLUMNS];
	for (int i = 0; i < ROWS * COLUMNS; i++) temp_environment[i] = environment[i];
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			int alive = 0;
			Cell cell = temp_environment[j + COLUMNS * i];
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue;
					int ni = i + dx, nj = j + dy;
					if ((ni >= 0 && ni < ROWS) && (nj >= 0 && nj < COLUMNS)) alive += temp_environment[nj + COLUMNS * ni].alive;
				}
			}
			if (cell.alive) {
				if (alive < 2) environment[j + COLUMNS * i].alive = 0; 
				else if (alive > 3) environment[j + COLUMNS * i].alive = 0; 
			}
			else {
				if (alive == 3) environment[j + COLUMNS * i].alive = 1;
			}
		}
	}	
	for (int i = 0; i < ROWS * COLUMNS; i++) color_cell(surface, environment[i].x, environment[i].y, environment[i].alive ? ALIVE : DEAD);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Game Of Life",  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_Event event;
	int running = 1, up = 0;
	Cell environment[ROWS * COLUMNS];
	initialise_cells(environment);
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			}	
			if (event.type == SDL_MOUSEMOTION) {
				if (event.motion.state != 0) {
					int cell_x = event.motion.x / CELL_SIZE;
					int cell_y = event.motion.y / CELL_SIZE;
					environment[cell_x + COLUMNS * cell_y].x = cell_x;
					environment[cell_x + COLUMNS * cell_y].y = cell_y;
					environment[cell_x + COLUMNS * cell_y].alive = 1;
					color_cell(surface, cell_x, cell_y, ALIVE);
				}
			}
			if (event.type == SDL_MOUSEBUTTONUP) up = !up;
		}
		if (up) simulate(surface, environment);
		draw_grid(surface);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(50);
	}
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
