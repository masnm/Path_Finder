#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wctype.h>

#include "ab_sdl.h"
#include "map.h"

const int window_widht = 800, window_height = 800;
const int grid_rows = 15, grid_columns = 15;
const int cell_width = 50, cell_height = 50;

SDL_Rect start_cell = {
	.x = 1, .y = 1,
	.w = cell_width - 2, .h = cell_height - 2
};
SDL_Rect end_cell = {
	.x = (grid_rows - 1)*cell_height + 1, .y = (grid_columns - 1)*cell_width + 1,
	.w = cell_width - 2, .h = cell_height - 2
};

typedef enum {
	generate_maze,
	get_start_pos,
	get_end_pos,
	finding_path,
	waiting
} state ;

int main ( int argc, char* argv[] )
{
	srand ( time(0) );
	ab_sdl_start ( window_widht, window_height );
	grid2d map = grid2d_create ( grid_rows, grid_columns, cell_width, cell_height );
	grid2d_maze_generate_create ( &map );
	grid2d_bfs_path_finder_create ( &map );

	// for debugging purpose setting value
	state current_state = waiting;

	bool done = false;
	SDL_Event e;
	while ( !done ) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				switch( e.key.keysym.sym ){
					case SDLK_s:
						if ( current_state == waiting ) {
							current_state = get_start_pos;
							grid2d_bfs_path_finder_clear_parent ( &map );
						}
						break;
					case SDLK_g:
						if ( current_state == waiting ) {
							current_state = generate_maze;
							grid2d_maze_generate_prepare ( &map );
							grid2d_bfs_path_finder_clear_parent ( &map );
						}
						break;
					case SDLK_f:
						if ( current_state == waiting ) {
							current_state = finding_path;
							grid2d_bfs_path_finder_prepare ( &map, start_cell.y/map.cell_width, start_cell.x/map.cell_height,
									end_cell.y/map.cell_width, end_cell.x/map.cell_height );
						}
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch ( e.button.button ) {
					case SDL_BUTTON_LEFT:
						if ( current_state == get_start_pos ) current_state = get_end_pos;
						else if ( current_state == get_end_pos ) current_state = waiting;
						break;
					default:
						break;
				}
			case SDL_MOUSEMOTION:
				int pos_x, pos_y;
				SDL_GetMouseState ( &pos_x, &pos_y );
				if ( current_state == get_start_pos ) {
					if ( pos_x < 0 ) pos_x = 0; if ( pos_x >= cell_width*(grid_columns-1) ) pos_x = cell_width*(grid_columns-1);
					if ( pos_y < 0 ) pos_y = 0; if ( pos_y >= cell_height*(grid_rows-1) ) pos_y = cell_height*(grid_rows-1);
					start_cell.x = 1+(int)(pos_x / cell_width)*cell_width;
					start_cell.y = 1+(int)(pos_y / cell_height)*cell_height;
				} else if ( current_state == get_end_pos ) {
					if ( pos_x < 0 ) pos_x = 0; if ( pos_x >= cell_width*grid_columns ) pos_x = cell_width*grid_columns;
					if ( pos_y < 0 ) pos_y = 0; if ( pos_y >= cell_height*grid_rows ) pos_y = cell_height*grid_rows;
					end_cell.x = 1+(int)(pos_x / cell_width)*cell_width;
					end_cell.y = 1+(int)(pos_y / cell_height)*cell_height;
				}
				break;
			default: {}
			}
		}
		grid2d_render ( &map, renderer, 255, 0, 0, 255 );
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderDrawRect ( renderer, &start_cell );
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
		SDL_RenderDrawRect ( renderer, &end_cell );
		if ( current_state == generate_maze )
			if ( grid2d_maze_generate_step ( &map, renderer ) )
				current_state = waiting;
		if ( current_state == finding_path ) {
			if ( grid2d_bfs_path_finder_step ( &map, renderer ) ) {
				grid2d_bfs_path_finder_keep_one_path ( &map );
				current_state = waiting;
			}
		}
		grid2d_bfs_path_finder_show_parent ( &map, renderer );
		SDL_RenderPresent(renderer);
	}
	grid2d_maze_generate_cleanup ();
	grid2d_bfs_path_finder_cleanup ();

	grid2d_destroy ( &map );
	ab_sdl_quit ();

	return 0;
}
