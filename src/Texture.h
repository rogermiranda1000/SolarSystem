#pragma once

// librerias generales
#include <iostream>
#include <string.h>
#include <map>

// librerias de OpenGL
#include "glfunctions.h"

// librerias de carga de imagenes
#include "imageloader.h"

// las primeras texturas son texturas por defecto (se usan mucho, no las descargues)
// tiene que ser menor que 16 (número de texturas aseguradas por OpenGL)
#define SURE_TEXTURES 3

class Texture {
private:
	static int _next_to_remove;

	// figura
	GLuint _id;

	void Texture::load(std::string path);
	int Texture::activate();

	static int Texture::getNextSlot() {
		int size = Texture::active_textures.size();

		if (size > Texture::max_textures) {
			// demasiadas texturas; hay que eliminar una
			Texture::_next_to_remove++;
			if (Texture::_next_to_remove >= Texture::max_textures) Texture::_next_to_remove = SURE_TEXTURES;

			// elimina la textura con el ID a reemplazar
			std::map<Texture*, int>::const_iterator it = Texture::active_textures.begin(); // iterador
			while (it != Texture::active_textures.end() && it->second != Texture::_next_to_remove) it++;
			if (it != Texture::active_textures.end()) Texture::active_textures.erase(it);

			return Texture::_next_to_remove;
		}

		return size;
	}

public:
	static GLint max_textures;
	static std::map<Texture*, int> active_textures;

	Texture(std::string path);

	int Texture::getTextureUnit();
};