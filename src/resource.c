#include "resource.h"

#include "raylib.h"
#include "string.h"
#include <stdio.h>

GmResource * _rsx;
i32 _rsx_cnt;

void gm_initResources( void ) {
	// Load all globally accessable resources here
	_rsx = NULL;
	_rsx_cnt = 0;
}

void gm_updateResources( void ) {
	// Update resources. Only relevant for Hotloading or other per frame relevant updates
	
}

void gm_drawResources(  void ) {
	// Resources typically dont need any draws, but it can be used as post-draw updates
	
}

void gm_closeResources( void ) {
	// Release all resources currently loaded

}

GmRsxPath gm_createRsxPath( char const * p_rsx_path ) {
	char * full = NULL;
	char const * file = NULL;
	char const * extension = NULL;
	char * fileNoExtension = NULL;
	GmRsxType baseType = NONE;
	GmRsxTextureType textureType = NONE;
	i32 segmentWidth = 0;
	i32 segmentHeight = 0;
	i32 segmentColumns = 0;
	i32 segmentRows = 0;
	u32 len = TextLength( p_rsx_path );
	full = (char *)MemAlloc( len + 1 );
	memcpy( full, p_rsx_path, (size_t)len + 1 );
	file = GetFileName( full );
	extension = GetFileExtension( file );
	u32 file_len = TextLength( file ) - TextLength( extension );
	fileNoExtension = (char *)MemAlloc( file_len + 1 );
	memcpy( fileNoExtension, file, file_len );

	if ( strcmp( extension, ".png" ) == 0 ) {
		baseType = TEXTURE;
		textureType = GPU;
	} else {
		MemFree( full );
		MemFree( fileNoExtension );
		return (GmRsxPath) { 0 };
	}

	char * crs = fileNoExtension;
	while ( *crs ) {
		if ( strncmp( crs, ".anim.", 6 ) == 0 ) {
			baseType = ANIMATED_TEXTURE;
			*crs = '\0';
			crs += 5;
		} else if ( strncmp( crs, ".tmap.", 6 ) == 0 ) {
			baseType = TILEMAP;
			*crs = '\0';
			crs += 5;
		} else if ( strncmp( crs, ".w", 2 ) == 0 ) {
			char * end = NULL;
			i64 w = strtol( ++crs, &end, 10 );
			if ( !end || (end && *end != '.' ) )
				continue;

			*crs = '\0';
			crs = end;
			segmentWidth = (i32)w;
		} else if ( strncmp( crs, ".h", 2 ) == 0 ) {
			char * end = NULL;
			i64 h = strtol( ++crs, &end, 10 );
			if ( !end || (end && *end != '.' ) )
				continue;

			*crs = '\0';
			crs = end;
			segmentHeight = (i32)h;
		} else if ( strncmp( crs, ".c", 2 ) == 0 ) {
			char * end = NULL;
			i64 c = strtol( ++crs, &end, 10 );
			if ( !end || (end && *end != '.' ) )
				continue;

			*crs = '\0';
			crs = end;
			segmentColumns = (i32)c;
		} else if ( strncmp( crs, ".r", 2 ) == 0 ) {
			char * end = NULL;
			i64 r = strtol( ++crs, &end, 10 );
			if ( !end || (end && *end != '.' ) )
				continue;

			*crs = '\0';
			crs = end;
			segmentRows = (i32)r;
		}
		++crs;
	}
	return (GmRsxPath) {
		.full = full,
		.file = file,
		.extension = extension,
		.fileNoExtension = fileNoExtension,
		.attributes = {
			.baseType = baseType,
			.textureType = textureType,
			.segmentWidth = segmentWidth,
			.segmentHeight = segmentHeight,
			.segmentColumns = segmentColumns,
			.segmentRows = segmentRows
		}
	};
}

void gm_destroyRsxPath( GmRsxPath * p_rsx_path ) {
	if ( !p_rsx_path )
		return;
	if ( p_rsx_path->full )
		MemFree( (void *)p_rsx_path->full );
	if ( p_rsx_path->fileNoExtension )
		MemFree( (void *)p_rsx_path->fileNoExtension );
	*p_rsx_path = (GmRsxPath) { 0 };
}

i32 gm_loadResource( char const * p_rsx_path ) {
	if ( !FileExists( p_rsx_path ) )
		return ERSX_FILENOTFOUND;

	GmRsxPath path = gm_createRsxPath( p_rsx_path );
	if ( !path.full )
		return ERSX_INVALIDRESOURCEFILE;

	i32 rsx_len;
	u8 * rsx = LoadFileData( p_rsx_path, &rsx_len );
	i32 rsx_typ = ~*(i32 *)rsx;
	switch( rsx_typ ) {
	case TEXTURE: {
		u8 * dat = DecompressData(rsx + 4, rsx_len, &rsx_len );
		Image img = LoadImageFromMemory( ".png", dat, rsx_len );
		GmRsxTexture txtr;
		txtr.type = path.attributes.textureType;
		if ( txtr.type == GPU ) {
			txtr._.gpu = LoadTextureFromImage( img );
			UnloadImage( img );
		} else if ( txtr.type == CPU ) {
			txtr._.cpu = img;
		} else {
			UnloadImage( img );
			MemFree( rsx );
			MemFree( dat );
			return ERSX_UNKNOWNTEXTURETYPE;
		}
		int id = _rsx_cnt++;
		MemFree( rsx );
		MemFree( dat );
		_rsx[_rsx_cnt - 1] = (GmResource){
			.id = id,
			.type = TEXTURE,
			.name = path.fileNoExtension,
			.cast.texture = txtr
		};
		return id;
	} break;
	default:
		return ERSX_UNKNOWNRESOURCETYPE;
	}
}

void gm_unloadResource( GmResource p_rsx ) {
	switch( p_rsx.type ) {
	case 'txtr':
		if ( p_rsx.cast.texture.type == GPU )
			UnloadTexture( p_rsx.cast.texture._.gpu );
		else if ( p_rsx.cast.texture.type == CPU )
			UnloadImage( p_rsx.cast.texture._.cpu );

		p_rsx.cast.texture = (GmRsxTexture) { 0 };
		break;
	default:
		return;
	}
}

GmResource getResourceById( i32 p_rsx_id ) {
	if ( p_rsx_id < 0 || p_rsx_id > _rsx_cnt - 1 )
		return (GmResource) { 0 };

	return _rsx[p_rsx_id];
}

GmResource getResourceByName( char const * p_rsx_name ) {
	for ( i32 i = 0; i < _rsx_cnt; ++i ) {
		if ( strcmp( p_rsx_name, _rsx[i].name ) == 0 )
			return _rsx[i];
	}
	return (GmResource) { 0 };
}
