#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "cvector.h"
#include "cqueue.h"

// start custom struct to store in cvector
typedef struct {
	bool left, up, down, right;
} directions ;

typedef struct {
	int first, second;
} pair ;
// end custom struct to store in cvector
// custom enum for state machine
typedef enum {
	get_start,
	get_end,
	calculate_path
} state ;
// end custom enum for state machine

const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 800;
const int GRID_WIDTH = 20, GRID_HEIGHT = 20;
const int CELL_WIDTH = 37, CELL_HEIGHT = 37;
pair start, end;

bool inside ( pair p )
{
	return (p.first >= 0 && p.first < GRID_WIDTH
			&& p.second >= 0 && p.second < GRID_HEIGHT);
}

void update ( cvector map, pair start, pair end )
{
	cvector cv;
	directions* pr;
	if ( start.first == end.first && start.second > end.second ) { // left
		cv = *(cvector*)cvector_at ( &map, start.first );
		pr = (directions*)cvector_at ( &cv, start.second );
		pr->up = false;
		cv = *(cvector*)cvector_at ( &map, end.first );
		pr = (directions*)cvector_at ( &cv, end.second );
		pr->down = false;
	} else if ( start.second == end.second && start.first > end.first ) { // up
		cv = *(cvector*)cvector_at ( &map, start.first );
		pr = (directions*)cvector_at ( &cv, start.second );
		pr->left = false;
		cv = *(cvector*)cvector_at ( &map, end.first );
		pr = (directions*)cvector_at ( &cv, end.second );
		pr->right = false;
	} else if ( start.first == end.first && start.second < end.second ) { // right
		cv = *(cvector*)cvector_at ( &map, start.first );
		pr = (directions*)cvector_at ( &cv, start.second );
		pr->down = false;
		cv = *(cvector*)cvector_at ( &map, end.first );
		pr = (directions*)cvector_at ( &cv, end.second );
		pr->up = false;
	} else if ( start.second == end.second && start.first < end.first ) { // down
		cv = *(cvector*)cvector_at ( &map, start.first );
		pr = (directions*)cvector_at ( &cv, start.second );
		pr->right = false;
		cv = *(cvector*)cvector_at ( &map, end.first );
		pr = (directions*)cvector_at ( &cv, end.second );
		pr->left = false;
	} else {
		assert ( false );
	}
}

void generate_grid ( cvector map )
{
	bool visited[GRID_WIDTH][GRID_HEIGHT];
	for ( int i = 0 ; i < GRID_WIDTH ; ++i ) {
		for ( int j = 0 ; j < GRID_HEIGHT ; ++j ) {
			visited[i][j] = false;
		}
	}
	srand ( time(0) );
	cvector stack = cvector_create ( sizeof ( pair ) );
	pair pr = { .first = 0, .second = 0, };
	visited[pr.first][pr.second] = true;
	cvector_emplace_back ( &stack, &pr );

	while ( !cvector_empty ( &stack ) ) {
		pr = *(pair*)cvector_back ( &stack );
		cvector op = cvector_create ( sizeof ( pair ) );
		for ( int i = -1 ; i < 2 ; i += 2 ) {
			pair np;
			np.first = pr.first + i; np.second = pr.second + 0;
			if ( inside ( np ) && !visited[np.first][np.second] )
				cvector_emplace_back ( &op, &np );
			np.first = pr.first + 0; np.second = pr.second + i;
			if ( inside ( np ) && !visited[np.first][np.second] )
				cvector_emplace_back ( &op, &np );
		}
		if ( cvector_empty ( &op ) ) {
			cvector_pop_back ( &stack );
		} else {
			int ind = rand() % cvector_size ( &op );
			pair nxt = *(pair*)cvector_at ( &op, ind );
			cvector_emplace_back ( &stack, &nxt );
			update ( map, pr, nxt );
			visited[nxt.first][nxt.second] = true;
		}
		cvector_destroy ( &op );
	}
	cvector_destroy ( &stack );
}

void mprint ( void* data )
{
	pair pr = *(pair*)data;
	printf ( "[%d, %d], ", pr.first, pr.second );
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize sdl2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window *window = SDL_CreateWindow("Snake-C",
										  100, 100,
										  SCREEN_WIDTH, SCREEN_HEIGHT,
										  SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
												SDL_RENDERER_ACCELERATED |
												SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// typedef struct SDL_Rect { int x, y; int w, h; } SDL_Rect;
	cvector map = cvector_create ( sizeof(cvector) );
	for ( int i = 0 ; i < GRID_HEIGHT ; ++i ) {
		cvector row = cvector_create ( sizeof ( directions ) );
		for ( int j = 0 ; j < GRID_WIDTH ; ++j ) {
			directions cell = { true, true, true, true };
			cvector_emplace_back ( &row, &cell );
		}
		cvector_emplace_back ( &map, &row );
	}
	// end of preparing location grid

	// start create grid
	generate_grid ( map );
	// end create grid
	// debug start
	// for ( int i = 0 ; i < (int)cvector_size ( &map ) ; ++i ) {
	// 	cvector row = *(cvector*)cvector_at ( &map, i );
	// 	for ( int j = 0 ; j < (int)cvector_size ( &row ) ; ++j ) {
	// 		directions cell = *(directions*)cvector_at ( &row, j );
	// 		printf ( "[%d%d%d%d] ", cell.left, cell.up, cell.right, cell.down );
	// 	}
	// 	printf ( "\n" );
	// }
	// debug end
	
	cqueue que = cqueue_create ( sizeof(pair) );
	pair cu = { .first = 0, .second = 0 };
	// cqueue_push ( &que, &cu );
	bool visited[GRID_WIDTH][GRID_HEIGHT];
	for ( int i = 0 ; i < GRID_WIDTH ; ++i ) {
		for ( int j = 0 ; j < GRID_HEIGHT ; ++j ) {
			visited[i][j] = false;
		}
	}
	pair parent[GRID_WIDTH][GRID_HEIGHT];
	for ( int i = 0 ; i < GRID_WIDTH ; ++i ) {
		for ( int j = 0 ; j < GRID_HEIGHT ; ++j ) {
			parent[i][j].first = i;
			parent[i][j].second = j;
		}
	}

	bool found_solution = false;
	state current_state = get_start;

	int xmouse, ymouse;
	bool execute = false;
	bool done = false;
	SDL_Event e;
	while ( !done ) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				switch( e.key.keysym.sym ){
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;
					case SDLK_SPACE:
						if ( current_state == calculate_path && found_solution == true ) {
							for ( int i = 0 ; i < GRID_WIDTH ; ++i )
								for ( int j = 0 ; j < GRID_HEIGHT ; ++j )
									visited[i][j] = false;
							for ( int i = 0 ; i < GRID_WIDTH ; ++i )
								for ( int j = 0 ; j < GRID_HEIGHT ; ++j )
									parent[i][j].first = i, parent[i][j].second = j;
							current_state = get_start;
						}
						found_solution = false;
						for ( int i = 0 ; i < GRID_HEIGHT ; ++i ) {
							cvector row = *(cvector*)cvector_at ( &map, i );
							for ( int j = 0 ; j < GRID_WIDTH ; ++j ) {
								directions* cell = (directions*)cvector_at ( &row, j );
								cell->left = cell->up = cell->right = cell->down = true;
							}
						}
						generate_grid ( map );
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch ( e.button.button ) {
					case SDL_BUTTON_LEFT:
						if ( current_state == get_start ) {
							SDL_GetMouseState(&xmouse, &ymouse);
							start.first = xmouse / CELL_WIDTH; start.second = ymouse / CELL_HEIGHT;
							cu.first = start.first; cu.second = start.second;
							cqueue_push ( &que, &cu );
							current_state = get_end;
						} else if ( current_state == get_end ) {
							SDL_GetMouseState(&xmouse, &ymouse);
							end.first = xmouse / CELL_WIDTH; end.second = ymouse / CELL_HEIGHT;
							current_state = calculate_path;
						}
						break;
					default:
						break;
				}
			default: {}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		// board rendering
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		for ( int i = 0 ; i < (int)cvector_size ( &map ) ; ++i ) {
			cvector row = *(cvector*)cvector_at ( &map, i );
			for ( int j = 0 ; j < (int)cvector_size ( &row ) ; ++j ) {
				directions dir = *(directions*)cvector_at ( &row, j );
				int x1, y1, x2, y2;
				if ( dir.left == true ) {
					x1 = i * CELL_HEIGHT; y1 = j * CELL_WIDTH;
					x2 = x1; y2 = y1 + CELL_WIDTH;
					SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
				}
				if ( dir.up == true ) {
					x1 = i * CELL_HEIGHT; y1 = j * CELL_WIDTH;
					x2 = x1 + CELL_HEIGHT; y2 = y1;
					SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
				}
				if ( dir.right == true ) {
					x1 = (i+1) * CELL_HEIGHT; y1 = j * CELL_WIDTH;
					x2 = x1; y2 = y1 + CELL_WIDTH;
					SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
				}
				if ( dir.down == true ) {
					x1 = i * CELL_HEIGHT; y1 = (j+1) * CELL_WIDTH;
					x2 = x1 + CELL_HEIGHT; y2 = y1;
					SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		if ( !found_solution ) {
			for ( int i = 0 ; i < GRID_WIDTH ; ++i ) {
				for ( int j = 0 ; j < GRID_HEIGHT ; ++j ) {
					if ( !(parent[i][j].first == i && parent[i][j].second == j) ) {
						SDL_RenderDrawLine ( renderer, i*CELL_HEIGHT+(CELL_HEIGHT/2), j*CELL_WIDTH+(CELL_WIDTH/2),
								parent[i][j].first*CELL_HEIGHT+(CELL_HEIGHT/2), parent[i][j].second*CELL_WIDTH+(CELL_WIDTH/2) );
					}
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		if ( found_solution ) {
			int mx = end.first, my = end.second;
			pair stp = {.first = end.first, .second = end.second }, val = parent[mx][my];
			while ( !(parent[mx][my].first==mx && parent[mx][my].second==my) ) {
				SDL_RenderDrawLine ( renderer, mx*CELL_HEIGHT+(CELL_HEIGHT/2), my*CELL_WIDTH+(CELL_WIDTH/2),
						val.first*CELL_HEIGHT+(CELL_HEIGHT/2), val.second*CELL_WIDTH+(CELL_WIDTH/2) );
				mx = val.first; my = val.second;
				val = parent[mx][my];
			}
		}

		// while ( execute && !cqueue_empty ( &que ) ) {
		if ( current_state == calculate_path && !cqueue_empty ( &que ) ) {
			execute = false;
			// cqueue_print ( &que, mprint );
			cu = *(pair*)cqueue_front ( &que );
			cqueue_pop ( &que );
			if ( visited[cu.first][cu.second] ) break;
			visited[cu.first][cu.second] = true;
			cvector* sr; directions* sd; pair next;
			sr = cvector_at ( &map, cu.first );
			sd = cvector_at ( sr, cu.second );
			if ( sd->down == false ) {
				next.first = cu.first; next .second = cu.second + 1;
				if ( inside ( next ) && !visited[next.first][next.second] ) {
					cqueue_push ( &que, &next );
					parent[next.first][next.second] = cu;
				}
			}
			if ( sd->left == false ) {
				next.first = cu.first - 1; next .second = cu.second;
				if ( inside ( next ) && !visited[next.first][next.second] ) {
					cqueue_push ( &que, &next );
					parent[next.first][next.second] = cu;
				}
			}
			if ( sd->up == false ) {
				next.first = cu.first; next .second = cu.second - 1;
				if ( inside ( next ) && !visited[next.first][next.second] ) {
					cqueue_push ( &que, &next );
					parent[next.first][next.second] = cu;
				}
			}
			if ( sd->right == false ) {
				next.first = cu.first + 1; next .second = cu.second;
				if ( inside ( next ) && !visited[next.first][next.second] ) {
					cqueue_push ( &que, &next );
					parent[next.first][next.second] = cu;
				}
			}
			if ( cu.first == end.first && cu.second == end.second ) {
				found_solution = true;
				while ( !cqueue_empty ( &que ) ) {
					cqueue_pop ( &que );
				}
			}
		}

		// usleep ( 100 * 1000 );
		SDL_RenderPresent(renderer);
	}
	cqueue_destroy ( &que );

	for ( int i = 0 ; i < (int)cvector_size ( &map ) ; ++i ) {
		cvector_destroy ( (cvector*)cvector_at ( &map, i ) );
	}
	cvector_destroy ( &map );
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
