#include "Texture.h"

std::map<Texture*, int> Texture::active_textures;
int Texture::_next_to_remove = SURE_TEXTURES-1;
GLint Texture::max_textures;

Texture::Texture(std::string path) {
	this->load(path);
}

void Texture::load(std::string path) {
	Image* image = loadBMP(path.c_str());

	glGenTextures(1, &this->_id);
	glBindTexture(GL_TEXTURE_2D, this->_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // how do we interpolate pixels?

	glTexImage2D(GL_TEXTURE_2D, // target
		0, // no mipmap (~low quality)
		GL_RGB, // how do we store the data?
		image->width,
		image->height,
		0, // border
		GL_RGB, // format original data
		GL_UNSIGNED_BYTE, // type of data
		image->pixels); // pointer to the start ( &(image->pixels[0]) )
}

int Texture::activate() {
	int texture_unit = Texture::getNextSlot();

	glActiveTexture(GL_TEXTURE0 + texture_unit); // activate the next texure unit
	glBindTexture(GL_TEXTURE_2D, this->_id); // bind the texture object

	Texture::active_textures.insert(std::pair<Texture*, GLenum>(this, texture_unit)); // insert to the used list

	return texture_unit;
}

int Texture::getTextureUnit() {
	std::map<Texture*, int>::const_iterator it = Texture::active_textures.find(this); // iterador

	if (it == Texture::active_textures.end()) {
		// no se ha encontrado la textura
		return this->activate();
	}
	else {
		// textura encontrada
		return it->second;
	}
}