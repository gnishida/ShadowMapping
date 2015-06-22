#include "ShadowMapping.h"
#include "GLWidget3D.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI	3.1415926535
#endif

ShadowMapping::ShadowMapping() {
}
	
void ShadowMapping::updateShadowMatrix(const glm::vec3& light_pos) {
	glm::mat4 light_pMatrix = glm::ortho<float>(-10, 10, -10, 10, 0.1, 100);
	glm::mat4 light_mvMatrix = glm::lookAt(light_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 light_mvpMatrix = light_pMatrix * light_mvMatrix;

	glm::vec3 light_dir = glm::normalize(-light_pos);

	// シェーダにmodel view projection行列を渡す
	glUniformMatrix4fv(glGetUniformLocation(programId, "light_mvpMatrix"), 1, GL_FALSE, &light_mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(programId, "lightDir"), light_dir.x, light_dir.y, light_dir.z);
}

void ShadowMapping::initShadow(int programId, int width, int height) {
	this->programId = programId;
	this->width = width;
	this->height = height;
			
	// FBO作成
	glGenFramebuffers(1, &fboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, fboDepth);
		
	// 影のデプスバッファを保存するための2Dテクスチャを作成
	glGenTextures(1, &textureDepth);

	// GL_TEXTURE6に、このデプスバッファをbindすることで、
	// シェーダからは6番でアクセスできる
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, textureDepth);

	// テクスチャパラメータの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
    // テクスチャ領域の確保(GL_DEPTH_COMPONENTを用いる)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// 生成した2Dテクスチャを、デプスバッファとしてfboに括り付ける。
	// 以後、このfboに対するレンダリングを実施すると、デプスバッファのデータは
	// この2Dテクスチャに自動的に保存される。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth, 0);

	glActiveTexture(GL_TEXTURE0);
		
	// シェーダに、GL_TEXTURE6がシャドウマッピング用のデプスバッファであることを伝える
	glUniform1i(glGetUniformLocation(programId, "shadowMap"), 6);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void ShadowMapping::makeShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_pos) {
	int origWidth = glWidget3D->width();
	int origHeigh = glWidget3D->height();
				
	// レンダリング結果をFBOに保存するようにする
	// この結果、デプスバッファはtextureDepthに保存される。
    glBindFramebuffer(GL_FRAMEBUFFER, fboDepth);
    glEnable(GL_TEXTURE_2D);

	// ビューポートをシャドウマップの大きさに変更
	glViewport(0, 0, width, height);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.1f, 4.0f);

	updateShadowMatrix(light_pos);

	// 色バッファには描画しない
	glDrawBuffer(GL_NONE);

	// デプスバッファをクリア
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//RENDER
	glWidget3D->drawScene(1);

	
	// この時点で、textureDepthにデプス情報が格納されている

	
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glDisable(GL_POLYGON_OFFSET_FILL);

	// ビューポートを戻す
	glViewport(0,0,origWidth,origHeigh);
}
