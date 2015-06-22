﻿#include "ShadowMapping.h"
#include "GLWidget3D.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI	3.1415926535
#endif

ShadowMapping::ShadowMapping() {
}

/**
 * シャドウマッピングの初期化。
 * 本関数は、GLWidget3D::initializeGL()内で呼び出すこと。
 *
 * @param programId		シェイダーのprogram id
 * @param width			シャドウマッピングの幅
 * @param height		シャドウマッピングの高さ
 */
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// テクスチャの外側、つまり、光源の外側は、影ってことにする(?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ただ、そもそも光源の外にならないよう、projection行列を設定すべき！
		
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

/**
 * シャドウマップを作成し、GL_TEXTURE6にテクスチャとして保存する。
 *
 * @param glWidget3D		GLWidget3Dクラス。このクラスのdrawScene(1)を呼び出してシーンを描画し、シャドウマップを生成する。
 * @param light_dir			光の進行方向
 */
void ShadowMapping::makeShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir) {
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

	updateShadowMatrix(light_dir);

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

/**
 * Model/View/Projection行列を更新し、シェーダに渡す。
 *
 * @param light_dir		光の進行方向
 */
void ShadowMapping::updateShadowMatrix(const glm::vec3& light_dir) {
	glm::mat4 light_pMatrix = glm::ortho<float>(-10, 10, -10, 10, 0.1, 100);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 10.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 light_mvpMatrix = light_pMatrix * light_mvMatrix;

	// シェーダにmodel view projection行列を渡す
	glUniformMatrix4fv(glGetUniformLocation(programId, "light_mvpMatrix"), 1, GL_FALSE, &light_mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(programId, "lightDir"), light_dir.x, light_dir.y, light_dir.z);
}