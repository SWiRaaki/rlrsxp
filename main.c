#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "resource.h"

#define ARG_RSX_PATH argv[1]
#define ARG_ASSET_PATH argv[2]

i32 const pref_txtr = ~TEXTURE;
i32 const pref_tmap = ~TILEMAP;
i32 const pref_anim = ~ANIMATED_TEXTURE;

void autofillAttributes( i32 p_img_w, i32 p_img_h, GmRsxPath * p_meta ) {
	if ( !p_meta )
		return;

	// Derive missing values based on given attributes
	if ( p_meta->attributes.segmentWidth <= 0 && p_meta->attributes.segmentColumns > 0 )
		p_meta->attributes.segmentWidth = p_img_w / p_meta->attributes.segmentColumns;

	if ( p_meta->attributes.segmentHeight <= 0 && p_meta->attributes.segmentRows > 0 )
		p_meta->attributes.segmentHeight = p_img_h / p_meta->attributes.segmentRows;

	if ( p_meta->attributes.segmentColumns <= 0 && p_meta->attributes.segmentWidth > 0 )
		p_meta->attributes.segmentColumns = p_img_w / p_meta->attributes.segmentWidth;

	if ( p_meta->attributes.segmentRows <= 0 && p_meta->attributes.segmentHeight > 0 )
		p_meta->attributes.segmentRows = p_img_h / p_meta->attributes.segmentHeight;

	// Default to whole image size if still not set
	if ( p_meta->attributes.segmentWidth <= 0 )
		p_meta->attributes.segmentWidth = p_img_w;

	if ( p_meta->attributes.segmentHeight <= 0 )
		p_meta->attributes.segmentHeight = p_img_h;

	if ( p_meta->attributes.segmentColumns <= 0 )
		p_meta->attributes.segmentColumns = p_img_w / p_meta->attributes.segmentWidth;

	if ( p_meta->attributes.segmentRows <= 0 )
		p_meta->attributes.segmentRows = p_img_h / p_meta->attributes.segmentHeight;
}

GmRsxTileMap convertImageToTilemap( Image p_image, GmRsxPath p_meta ) {
	autofillAttributes( p_image.width, p_image.height, &p_meta);
	GmRsxTileMap map = {
		.tileWidth = p_meta.attributes.segmentWidth,
		.tileHeight = p_meta.attributes.segmentHeight,
		.columns = p_meta.attributes.segmentColumns,
		.rows = p_meta.attributes.segmentRows,
		.tiles = (GmRsxTexture **)MemAlloc( sizeof( GmRsxTexture ) * (u32)p_meta.attributes.segmentRows )
	};
	for ( i32 row = 0; row < p_meta.attributes.segmentRows; ++row ) {
		map.tiles[row] = (GmRsxTexture *)MemAlloc( sizeof( GmRsxTexture ) * (u32)p_meta.attributes.segmentColumns );
		for ( i32 col = 0; col < p_meta.attributes.segmentColumns; ++col ) {
			Rectangle tile_rect = (Rectangle) {
				.x = (f32)(col * p_meta.attributes.segmentWidth),
				.y = (f32)(row * p_meta.attributes.segmentHeight),
				.width = (f32)p_meta.attributes.segmentWidth,
				.height = (f32)p_meta.attributes.segmentHeight
			};
			Image tile = ImageFromImage( p_image, tile_rect );
			map.tiles[row][col] = (GmRsxTexture) {
				.type = CPU,
				._.cpu = tile
			};
		}
	}
	return map;
}

void processTexture( GmRsxPath p_rsx_path, char const * p_out_path ) {
	size_t len = TextLength( p_rsx_path.fileNoExtension ) + TextLength( p_out_path ) + 6;
	char path[len];
	snprintf( path, len, "%s/%s.rsx", p_out_path, p_rsx_path.fileNoExtension );
	int cnt_dat, cnt_cpr;
	Byte * dat = LoadFileData( p_rsx_path.full, &cnt_dat );
	
	printf( "Compressing data..\n" );
	Byte * cpr = CompressData( dat, cnt_dat, &cnt_cpr );
	dat = MemRealloc( dat, (u32)cnt_cpr + 4 );
	memcpy( dat + 4, cpr, (u64)cnt_cpr );
	memcpy( dat, &pref_txtr, 4 );

	printf( "Saving %s..\n", path );
	SaveFileData( path, dat, cnt_cpr + 4 );
	MemFree( dat );
	MemFree( cpr );
}

void processTileMap( GmRsxPath p_rsx_path, char const * p_out_path ) {
	size_t len = TextLength( p_rsx_path.fileNoExtension ) + TextLength( p_out_path ) + 6;
	char path[len];
	snprintf( path, len, "%s/%s.rsx", p_out_path, p_rsx_path.fileNoExtension );
	TraceLog( LOG_INFO, "Loading image into memory..\n" );
	Image img = LoadImage( p_rsx_path.full );
	TraceLog( LOG_INFO, "Splitting image into tiles..\n" );
	GmRsxTileMap map = convertImageToTilemap( img, p_rsx_path );
	FILE * cpr = fopen( path, "w+" );
	i32 tile_cnt = map.columns * map.rows;
	fwrite( &pref_tmap, sizeof( i32 ), 1, cpr );
	fwrite( &tile_cnt, sizeof( i32 ), 1, cpr );
	for ( i32 row = 0; row < map.rows; ++row ) {
		for ( i32 col = 0; col < map.columns; ++col ) {
			printf( "Compressing tile (%d|%d) (row|col)..\n", row, col );
			i32 dat_cnt;
			Byte * dat = CompressData((Byte *)map.tiles[row][col]._.cpu.data, map.tileHeight * map.tileWidth * 4, &dat_cnt );
			fwrite( &dat_cnt, sizeof( i32 ), 1, cpr );
			fwrite( dat, sizeof ( Byte ), (u64)dat_cnt, cpr );
			MemFree( dat );
		}
	}
	fclose( cpr );
}

int main( int argc, char * argv[] ) {
	SetTraceLogLevel( LOG_NONE );

	if ( argc != 3 ) {
		printf( "Invalid arguments!\n" );
		printf( "rsxpipeline <rsx_path> <asset_path>\n" );
		printf( "<rsx_path>: Input path of the raw resources\n" );
		printf( "<asset_path>: Output path for the assets (\".rsx\"-files)\n" );
		return -1;
	}
	printf( "Resource Pipeline v0.1\n" );
	printf( "Searching for unmanaged resources in %s..\n", argv[1] );

	//FilePathList files = LoadDirectoryFilesEx( argv[1], "png", true );
	FilePathList files = LoadDirectoryFiles( argv[1] );

	printf( "Found %d files!\n", files.count );

	for ( int unsigned i = 0; i < files.count; ++i ) {
		GmRsxPath path = gm_createRsxPath( files.paths[i] );
		if ( !path.full ) {
			printf( "%s is not a known resource. Skipped!\n", files.paths[i] );
		} else if ( path.attributes.baseType == TEXTURE ) {
			processTexture( path, argv[2] );
		}
		gm_destroyRsxPath( &path );
	}

	printf( "Finished processing resources!\n" );

	UnloadDirectoryFiles( files );
}
