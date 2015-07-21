#include "RenderManager.h"
#include <iostream>
#include "Shader.h"
#include <QImage>
#include <QGLWidget>

GeometryObject::GeometryObject() {
	vaoCreated = false;
	vaoUpdated = false;
}

GeometryObject::GeometryObject(const std::vector<Vertex>& vertices) : vertices(vertices) {
	vaoCreated = false;
	vaoUpdated = false;
}

void GeometryObject::addVertices(const std::vector<Vertex>& vertices) {
	this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
	vaoUpdated = false;
}

/**
 * Create VAO according to the vertices.
 */
void GeometryObject::createVAO() {
	// VAOが作成済みで、最新なら、何もしないで終了
	if (vaoCreated && vaoUpdated) return;

	if (vaoCreated) {
		// 古いVAO, VBOを解放する
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	// create vao and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create VBO and tranfer the vertices data to GPU buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	
	// configure the attributes in the vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	// unbind the vao
	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAOを作成済み、最新とマークする
	vaoCreated = true;
	vaoUpdated = true;
}

RenderManager::RenderManager() {
}

void RenderManager::init() {
	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	// load shaders
	Shader shader;
	program = shader.createProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl");
	glUseProgram(program);

	// ダミーのtexture idを作成する。
	// これにより、実際に使われるtexture idは1以上の値となる
	GLuint texId;
	glGenTextures(1, &texId);

	shadow.initShadow(program, 4096, 4096);
}

void RenderManager::addObject(const QString& object_name, const QString& texture_file, const std::vector<Vertex>& vertices) {
	GLuint texId;
	
	if (texture_file.length() > 0) {
		// テクスチャファイルがまだ読み込まれていない場合は、ロードする
		if (!textures.contains(texture_file)) {
			texId = loadTexture(texture_file);
			textures[texture_file] = texId;
		} else {
			texId = textures[texture_file];
		}
	} else {
		texId = 0;
	}

	if (objects.contains(object_name)) {
		if (objects[object_name].contains(texId)) {
			objects[object_name][texId].addVertices(vertices);
		} else {
			objects[object_name][texId] = GeometryObject(vertices);
		}
	} else {
		objects[object_name][texId] = GeometryObject(vertices);
	}
}

void RenderManager::removeObject(const QString& object_name) {
	// VAO、VBOを解放する
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		glDeleteBuffers(1, &it->vbo);
		glDeleteVertexArrays(1, &it->vao);
	}

	objects.remove(object_name);
}

void RenderManager::render(const QString& object_name) {
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		GLuint texId = it.key();
		
		// vaoを作成
		it->createVAO();

		if (texId > 0) {
			// テクスチャなら、バインドする
			glBindTexture(GL_TEXTURE_2D, texId);
			glUniform1i(glGetUniformLocation(program, "mode"), 2);
			glUniform1i(glGetUniformLocation (program, "tex0"), 0);
		} else {
			glUniform1i(glGetUniformLocation(program, "mode"), 1);
		}

		// 描画
		glBindVertexArray(it->vao);
		glDrawArrays(GL_TRIANGLES, 0, it->vertices.size());

		glBindVertexArray(0);
	}
}

void RenderManager::updateShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir) {
	shadow.makeShadowMap(glWidget3D, light_dir);
}


GLuint RenderManager::loadTexture(const QString& filename) {
	QImage img;
	if (!img.load(filename)) {
		printf("ERROR: loading %s\n",filename.toUtf8().data());
		return INT_MAX;
	}

	QImage GL_formatted_image;
	GL_formatted_image = QGLWidget::convertToGLFormat(img);
	if (GL_formatted_image.isNull()) {
		printf("ERROR: GL_formatted_image\n");
		return INT_MAX;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(), GL_formatted_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

