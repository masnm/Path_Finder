#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "cqueue.h"
#include "cstack.h"
#include "cvector.h"

typedef struct {
	int first, second;
} pair ;

typedef struct {
	bool left, up, right, down;
} wall ;

typedef struct {
	cvector grid;
	size_t rows, columns;
	size_t cell_width, cell_height;
} grid2d ;

// map create and destroy
grid2d grid2d_create ( size_t rows, size_t columns, size_t cell_width, size_t cell_height );
void grid2d_destroy ( grid2d* map );

// map modifiers
void grid2d_reset ( grid2d* map );

// generate maze using stack
cstack stack;
cvector visited;
bool inside ( pair pr, grid2d* map );
void update ( grid2d map, pair start, pair end );
bool visited_at ( int row, int column );
void visited_set ( int row, int column, bool state );
void grid2d_maze_generate_create ( grid2d* map );
void grid2d_maze_generate_prepare ( grid2d* map );
bool grid2d_maze_generate_step ( grid2d* map, SDL_Renderer* renderer );
bool grid2d_maze_generate_cleanup ();

// find path in maze using bfs algirithm
cqueue queue;
cvector parent;
pair end_pos;
void parent_set ( int row, int column, pair pr );
pair parent_get ( int row, int column );
void grid2d_bfs_path_finder_create ( grid2d* map );
void grid2d_bfs_path_finder_prepare ( grid2d* map, int start_x, int start_y, int end_x, int end_y );
void grid2d_bfs_path_finder_show_parent ( grid2d* map, SDL_Renderer* renderer );
void grid2d_bfs_path_finder_keep_one_path ( grid2d* map );
void grid2d_bfs_path_finder_clear_parent ( grid2d* map );
bool grid2d_bfs_path_finder_step ( grid2d* map, SDL_Renderer* renderer );
bool grid2d_bfs_path_finder_cleanup ();

// sdl drawing interfaces
void grid2d_render ( grid2d* map, SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a );

// implementations start
grid2d grid2d_create ( size_t rows, size_t columns, size_t cell_width, size_t cell_height )
{
	grid2d map = { .grid = cvector_create(sizeof(cvector)), .rows = rows, .columns = columns,
					.cell_width = cell_width, .cell_height = cell_height };
	cvector_reserve ( &map.grid, columns );
	for ( int i = 0 ; i < columns ; ++i ) {
		cvector row = cvector_create ( sizeof(wall) );
		cvector_reserve ( &row, rows );
		for ( int j = 0 ; j < rows ; ++j ) {
			wall cell = { true, true, true, true };
			cvector_emplace_back ( &row, &cell );
		}
		cvector_emplace_back ( &map.grid, &row );
	}
	return map;
}

void grid2d_destroy ( grid2d* map )
{
	for ( int i = 0 ; i < (int)cvector_size ( &map->grid ) ; ++i ) {
		cvector_destroy ( (cvector*)cvector_at ( &map->grid, i ) );
	}
	cvector_destroy ( &map->grid );
	map->rows = 0;
	map->columns = 0;
}

void grid2d_reset ( grid2d* map )
{
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector row = *(cvector*)cvector_at ( &map->grid, i );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			wall* cell = (wall*)cvector_at ( &row, j );
			cell->left = cell->up = cell->right = cell->down = true;
		}
	}
}

void grid2d_render ( grid2d* map, SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	for ( int i = 0 ; i < (int)cvector_size ( &map->grid ) ; ++i ) {
		cvector row = *(cvector*)cvector_at ( &map->grid, i );
		for ( int j = 0 ; j < (int)cvector_size ( &row ) ; ++j ) {
			wall dir = *(wall*)cvector_at ( &row, j );
			int x1, y1, x2, y2;
			if ( dir.left == true ) {
				x1 = j * map->cell_width; y1 = i * map->cell_height;
				x2 = x1; y2 = y1 + map->cell_height;
				SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
			}
			if ( dir.up == true ) {
				x1 = j * map->cell_width; y1 = i * map->cell_height;
				x2 = x1 + map->cell_width; y2 = y1;
				SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
			}
			if ( dir.right == true ) {
				x1 = (j+1) * map->cell_width; y1 = i * map->cell_height;
				x2 = x1; y2 = y1 + map->cell_height;
				SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
			}
			if ( dir.down == true ) {
				x1 = j * map->cell_width; y1 = (i+1) * map->cell_height;
				x2 = x1 + map->cell_width; y2 = y1;
				SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
			}
		}
	}
}

bool inside ( pair pr, grid2d* map )
{
	return (pr.first >= 0 && pr.first < map->rows 
		&& pr.second >= 0 && pr.second < map->columns);
}

bool visited_at ( int row, int column )
{
	cvector* cv_row = (cvector*)cvector_at ( &visited, row );
	bool state = *(bool*)cvector_at ( cv_row, column );
	return state;
}

void visited_set ( int row, int column, bool state )
{
	cvector* cv_row = (cvector*)cvector_at ( &visited, row );
	*(bool*)cvector_at ( cv_row, column ) = state;
}

void update ( grid2d map, pair start, pair end )
{
	cvector cv = *(cvector*)cvector_at ( &map.grid, start.first );
	wall* spr = (wall*)cvector_at ( &cv, start.second );
	cv = *(cvector*)cvector_at ( &map.grid, end.first );
	wall* epr = (wall*)cvector_at ( &cv, end.second );
	if ( start.first == end.first && start.second > end.second ) { // left
		spr->left = false;
		epr->right = false;
	} else if ( start.second == end.second && start.first > end.first ) { // up
		spr->up = false;
		epr->down = false;
	} else if ( start.first == end.first && start.second < end.second ) { // right
		spr->right = false;
		epr->left = false;
	} else if ( start.second == end.second && start.first < end.first ) { // down
		spr->down = false;
		epr->up = false;
	}
}

void grid2d_maze_generate_create ( grid2d* map )
{
	stack = cstack_create ( sizeof(pair) );
	visited = cvector_create ( sizeof(cvector) );
	cvector_reserve ( &visited, map->columns );
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector row = cvector_create ( sizeof(bool) );
		cvector_reserve ( &row, map->rows );
		bool state = false;
		for ( int j = 0 ; j < map->rows ; ++j )
			cvector_emplace_back ( &row, &state );
		cvector_emplace_back ( &visited, &row );
	}
}

void grid2d_maze_generate_prepare ( grid2d* map )
{
	cstack_make_empty ( &stack );
	pair st = { .first = 0, .second = 0 };
	cstack_push ( &stack, &st );
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &visited, i );
		for ( int j = 0 ; j < map->rows ; ++j )
			*(bool*)cvector_at ( row, j ) = false;
	}
	grid2d_reset ( map );
}

bool grid2d_maze_generate_step ( grid2d* map, SDL_Renderer* renderer )
{
	if ( cstack_empty ( &stack ) ) return true;
	pair pr = *(pair*)cstack_top ( &stack );
	SDL_Rect rect = {
		.x = 1 + pr.second * map->cell_width, .y = 1 + pr.first * map->cell_height,
		.w = map->cell_width - 2, .h = map->cell_height - 2
	};
	SDL_SetRenderDrawColor ( renderer, 255, 255, 255, 255 );
	SDL_RenderDrawRect ( renderer, &rect );
	cvector op = cvector_create ( sizeof ( pair ) );
	for ( int i = -1 ; i < 2 ; i += 2 ) {
		pair np;
		np.first = pr.first + i; np.second = pr.second + 0;
		if ( inside ( np, map ) && !visited_at ( np.first, np.second ) )
			cvector_emplace_back ( &op, &np );
		np.first = pr.first + 0; np.second = pr.second + i;
		if ( inside ( np, map ) && !visited_at ( np.first, np.second ) )
			cvector_emplace_back ( &op, &np );
	}
	if ( cvector_empty ( &op ) )
		cstack_pop ( &stack );
	else {
		int ind = rand() % cvector_size ( &op );
		pair nxt = *(pair*)cvector_at ( &op, ind );
		cstack_push ( &stack, &nxt );
		update ( *map, pr, nxt );
		visited_set ( nxt.first, nxt.second, true );
	}
	cvector_destroy ( &op );
	return false;
}

bool grid2d_maze_generate_cleanup ()
{
	cstack_destroy ( &stack );
	for ( int i = 0 ; i < (int)cvector_size(&visited) ; ++i )
		cvector_destroy ( (cvector*)cvector_at ( &visited, i ) );
	cvector_destroy ( &visited );
}

void parent_set ( int rows, int column, pair par )
{
	cvector* row = (cvector*)cvector_at ( &parent, rows );
	pair* pr = (pair*)cvector_at ( row, column );
	pr -> first = par.first;
	pr -> second = par.second;
}

pair parent_get ( int rows, int column )
{
	cvector* row = (cvector*)cvector_at ( &parent, rows );
	pair pr = *(pair*)cvector_at ( row, column );
	return pr;
}

void grid2d_bfs_path_finder_create ( grid2d* map )
{
	queue = cqueue_create ( sizeof(pair) );
	parent = cvector_create ( sizeof(cvector) );
	visited = cvector_create ( sizeof(cvector) );
	cvector_reserve ( &visited, map->columns );
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector row = cvector_create ( sizeof(bool) );
		cvector_reserve ( &row, map->rows );
		bool state = false;
		for ( int j = 0 ; j < map->rows ; ++j )
			cvector_emplace_back ( &row, &state );
		cvector_emplace_back ( &visited, &row );
	}
	cvector_reserve ( &parent, map->columns );
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector row = cvector_create ( sizeof(pair) );
		cvector_reserve ( &row, map->rows );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			pair state = { .first = i, .second = j };
			cvector_emplace_back ( &row, &state );
		}
		cvector_emplace_back ( &parent, &row );
	}
}

void grid2d_bfs_path_finder_prepare ( grid2d* map, int start_x, int start_y, int end_x, int end_y )
{
	cqueue_make_empty ( &queue );
	pair pr = { .first = start_x, .second = start_y };
	cqueue_push ( &queue, &pr );
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &visited, i );
		for ( int j = 0 ; j < map->rows ; ++j )
			*(bool*)cvector_at ( row, j ) = false;
	}
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &parent, i );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			((pair*)cvector_at ( row, j )) -> first = i;
			((pair*)cvector_at ( row, j )) -> second = j;
		}
	}
	end_pos.first = end_x;
	end_pos.second = end_y;
}

void grid2d_bfs_path_finder_clear_parent ( grid2d* map )
{
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &parent, i );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			((pair*)cvector_at ( row, j )) -> first = i;
			((pair*)cvector_at ( row, j )) -> second = j;
		}
	}
}

void grid2d_bfs_path_finder_keep_one_path ( grid2d* map )
{
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &visited, i );
		for ( int j = 0 ; j < map->rows ; ++j )
			*(bool*)cvector_at ( row, j ) = false;
	}
	pair current = end_pos;
	while ( true ) {
		pair par = parent_get ( current.first, current.second );
		visited_set ( current.first, current.second, true );
		if ( par.first == current.first && par.second == current.second )
			break;
		current = par;
	}
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &parent, i );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			if ( !visited_at ( i, j ) ) {
				((pair*)cvector_at ( row, j )) -> first = i;
				((pair*)cvector_at ( row, j )) -> second = j;
			}
		}
	}
}

void grid2d_bfs_path_finder_show_parent ( grid2d* map, SDL_Renderer* renderer )
{
	for ( int i = 0 ; i < map->columns ; ++i ) {
		cvector* row = (cvector*)cvector_at ( &parent, i );
		for ( int j = 0 ; j < map->rows ; ++j ) {
			pair pr = *(pair*)cvector_at ( row, j );
			if ( !(pr.first==i&&pr.second==j) ) {
				SDL_SetRenderDrawColor ( renderer, 255, 255, 255, 255 );
				// SDL_RenderDrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2);
				int x1 = (map->cell_width/2)+pr.second*map->cell_width, y1 = (map->cell_width/2)+pr.first*map->cell_height;
				int x2 = (map->cell_width/2)+j*map->cell_width, y2 = (map->cell_width/2)+i*map->cell_height;
				SDL_RenderDrawLine ( renderer, x1, y1, x2, y2 );
			}
		}
	}
}

bool grid2d_bfs_path_finder_step ( grid2d* map, SDL_Renderer* renderer )
{
	if ( cqueue_empty ( &queue ) ) return true;
	pair cu = *(pair*)cqueue_front ( &queue ); cqueue_pop ( &queue );
	// fprintf ( stderr, "%d %d\n", cu.first, cu.second );
	if ( visited_at(cu.first,cu.second) ) return false;
	visited_set ( cu.first, cu.second, true );
	cvector* sr; wall* sd; pair next;
	sr = cvector_at ( &map->grid, cu.first );
	sd = cvector_at ( sr, cu.second );
	if ( sd->right == false ) {
		next.first = cu.first; next .second = cu.second + 1;
		if ( inside ( next, map )
				&& !visited_at (next.first,next.second) ) {
			cqueue_push ( &queue, &next );
			parent_set ( next.first, next.second, cu );
		}
	}
	if ( sd->up == false ) {
		next.first = cu.first - 1; next .second = cu.second;
		if ( inside ( next, map )
				&& !visited_at(next.first,next.second) ) {
			cqueue_push ( &queue, &next );
			parent_set ( next.first, next.second, cu );
		}
	}
	if ( sd->left == false ) {
		next.first = cu.first; next .second = cu.second - 1;
		if ( inside ( next, map )
				&& !visited_at(next.first,next.second) ) {
			cqueue_push ( &queue, &next );
			parent_set ( next.first, next.second, cu );
		}
	}
	if ( sd->down == false ) {
		next.first = cu.first + 1; next .second = cu.second;
		if ( inside ( next, map )
				&& !visited_at(next.first,next.second) ) {
			cqueue_push ( &queue, &next );
			parent_set ( next.first, next.second, cu );
		}
	}
	if ( cu.first == end_pos.first && cu.second == end_pos.second ) {
		return true;
		cqueue_make_empty ( &queue );
	}
	return false;
}

bool grid2d_bfs_path_finder_cleanup ()
{
	cqueue_destroy ( &queue );
	for ( int i = 0 ; i < (int)cvector_size(&parent) ; ++i )
		cvector_destroy ( (cvector*)cvector_at ( &parent, i ) );
	cvector_destroy ( &parent );
}
// implementations end

#endif /* MAP_H */
