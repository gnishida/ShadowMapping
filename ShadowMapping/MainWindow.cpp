#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	// setup the OpenGL widget
	glWidget = new GLWidget3D();
	setCentralWidget(glWidget);
}

void MainWindow::onFileOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open OBJ file..."), "", tr("OBJ Files (*.obj)"));
	if (filename.isEmpty()) return;

	QString filename2 = QFileDialog::getOpenFileName(this, tr("Open texture file..."), "", tr("texture Files (*.png)"));
	if (!filename2.isEmpty()) {
		glWidget->loadOBJ(filename.toUtf8().data(), filename2.toUtf8().data(), "object2");
	} else {
		glWidget->loadOBJ(filename.toUtf8().data(), "", "object2");
	}

	glWidget->updateGL();
}
