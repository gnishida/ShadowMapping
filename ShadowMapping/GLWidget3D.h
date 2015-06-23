#pragma once

#include <glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include "Camera.h"
#include "ShadowMapping.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	void drawScene(int drawMode);
	void loadOBJ(char* filename);
	void createVAO();
	GLuint loadTexture(const QString& filename);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();    
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

private:
	Camera camera;
	GLuint vao;
	GLuint program;
	std::vector<Vertex> vertices;
	glm::vec3 light_dir;

	ShadowMapping shadow;
};

