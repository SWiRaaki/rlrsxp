/********************************************************************
	Author           : raki 
	Email            : siwiraki@gmail.com 
	Editor           : raki 
	Created at       : 2025/03/19 
	Last modified at : 2025/03/19 22:18 by raki 
********************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#include <bits/stdint-intn.h>
#include <stdint.h>

#include "raylib.h"

#define NONE 0

typedef enum rsx_type_e {
	TEXTURE 			= 'txtr',
	TILEMAP 			= 'tmap',
	ANIMATED_TEXTURE 	= 'anim'
} GmRsxType;

typedef enum rsx_txtr_type_e {
	CPU 	= 'CPU',
	GPU 	= 'GPU'
} GmRsxTextureType;

typedef enum rsx_error_e {
	ERSX_FILENOTFOUND = -1,
	ERSX_UNKNOWNRESOURCETYPE = -2,
	ERSX_INVALIDRESOURCEFILE = -3,
	ERSX_UNKNOWNTEXTURETYPE = -4
} GmRsxError;

typedef struct rsx_texture_s {
	union {
		Texture 		gpu;
		Image 			cpu;
	} _;
	GmRsxTextureType 	type;
} GmRsxTexture;

typedef struct rsx_tiled_texture_s {
	GmRsxTexture ** 	tiles;
	int32_t				tileWidth;
	int32_t				tileHeight;
	int32_t				columns;
	int32_t				rows;
} GmRsxTileMap;

typedef struct rsx_anim_texture_s {
	GmRsxTexture 	frames;
	int32_t 		frameWidth;
	int32_t 		frameHeight;
	int32_t 		frameCount;
} GmRsxAnimatedTexture;

typedef struct rsx_resource_path_s {
	char const *			full;
	char const * 			file;
	char const * 			extension;
	char const * 			fileNoExtension;
	struct {
		GmRsxType 			baseType;
		GmRsxTextureType 	textureType;
		int32_t 			segmentWidth;
		int32_t 			segmentHeight;
		int32_t 			segmentColumns;
		int32_t 			segmentRows;
	} attributes;
} GmRsxPath;

typedef struct resource_s {
	int32_t id;
	int32_t	type;
	char const * name;
	union {
		void * 					resource;
		GmRsxTexture 			texture;
		GmRsxTileMap 			tileMap;
		GmRsxAnimatedTexture 	animatedTexture;
	} cast;
} GmResource;

// -------- Component Interface -------- //

/**
 * @brief Initialization function that prepares the pre-loop resources
 */
void gm_initResources( void );

/**
 * @brief Update function that should be called at the start of the loop
 * @details Only relevant if the game should be able to hotload resources
 */
void gm_updateResources( void );

/**
 * @brief Draw function that should be called after update is finished
 * @details Only relevant if the game should be able to hotload resources
 */
void gm_drawResources(  void );

/**
 * @brief Close function that marks the end of the lifecycle
 */
void gm_closeResources( void );

// -------- Resource Interface -------- //

/**
 * @brief Create a resource path with information about the resource to load
 * @param rsx_path bath to the  
 */
GmRsxPath gm_createRsxPath( char const * rsx_path );

/**
 * @brief Release a Resource Path from memory where memory is allocated
 */
void gm_destroyRsxPath( GmRsxPath * rsx_path );

/**
 * @brief Load a resource from a file and store it internally
 * @param rsx_path Path of the file to load
 * @return Resource id if the resource is loaded, error code ( < 0 ) otherwise
 */
int32_t gm_loadResource( char const * rsx_path );

/**
 * @brief Releases a loaded resource from memory.
 * @param rsx Resource handle
 */
void gm_unloadResource( GmResource rsx );

/**
 * @brief Retrieve the resource handle of a loaded resource with the resource id;
 * @param rsx_id Resource id
 * @return Resource handle if found or an empty handle on error
 */
GmResource getResourceById( int32_t rsx_id );

/**
 * @brief Retrieve the resource handle of a loaded resource with the resource name
 * @param rsx_name Resource name
 * @return Resource handle if found or an empty handle on error
 */
GmResource getResourceByName( char const * rsx_name );

#endif // RESOURCE_H
