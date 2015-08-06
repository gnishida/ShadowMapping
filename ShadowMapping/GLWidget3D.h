#pragma once

#include <glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include "Camera.h"
#include "ShadowMapping.h"
#include "RenderManager.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	GLWidget3D(QWidget *parent = 0);

	void drawScene(int drawMode);
	void loadOBJ(const QString& obj_file, const QString& texture_file, const QString& object_name);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();    
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

private:
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;

	RenderManager renderManager;
};

