/********************************************************************
	Author           : raki 
	Email            : siwiraki@gmail.com 
	Editor           : raki 
	Created at       : 2025/03/19 
	Last modified at : 2025/03/19 22:18 by raki 
********************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "raylib.h"
#include "survivor.h"

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
	i32					tileWidth;
	i32 				tileHeight;
	i32 				columns;
	i32 				rows;
} GmRsxTileMap;

typedef struct rsx_anim_texture_s {
	GmRsxTexture 	frames;
	i32 		frameWidth;
	i32 		frameHeight;
	i32 		frameCount;
} GmRsxAnimatedTexture;

typedef struct rsx_resource_path_s {
	char const *			full;
	char const * 			file;
	char const * 			extension;
	char const * 			fileNoExtension;
	struct {
		GmRsxType 			baseType;
		GmRsxTextureType 	textureType;
		i32 				segmentWidth;
		i32 				segmentHeight;
		i32 				segmentColumns;
		i32 				segmentRows;
	} attributes;
} GmRsxPath;

typedef struct resource_s {
	i32 id;
	i32 type;
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
 * @brief Update function that is called at the start of the loop
 */
void gm_updateResources( void );

/**
 * @brief Draw function that is called after update is finished
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
i32 gm_loadResource( char const * rsx_path );

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
GmResource getResourceById( i32 rsx_id );

/**
 * @brief Retrieve the resource handle of a loaded resource with the resource name
 * @param rsx_name Resource name
 * @return Resource handle if found or an empty handle on error
 */
GmResource getResourceByName( char const * rsx_name );

#endif // RESOURCE_H
