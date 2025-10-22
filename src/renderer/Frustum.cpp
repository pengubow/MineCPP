#include <cstring>
#include "GL_compat.h"
#include "renderer/Frustum.h"

Frustum Frustum::frustum = Frustum();

array<array<float,4>,6> Frustum::m_Frustum = {};
array<float,16> Frustum::proj = {};
array<float,16> Frustum::modl = {};
array<float,16> Frustum::clip = {};

Frustum::Frustum() {
    m_Frustum.fill({0.0f, 0.0f, 0.0f, 0.0f});
    proj.fill(0.0f);
    modl.fill(0.0f);
    clip.fill(0.0f);
}

Frustum& Frustum::getFrustum() {
    glGetFloatv(GL_PROJECTION_MATRIX, proj.data());
    glGetFloatv(GL_MODELVIEW_MATRIX, modl.data());

    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    m_Frustum[0][0] = clip[3] - clip[0];
    m_Frustum[0][1] = clip[7] - clip[4];
    m_Frustum[0][2] = clip[11] - clip[8];
    m_Frustum[0][3] = clip[15] - clip[12];
    normalizePlane(m_Frustum, 0);

    m_Frustum[1][0] = clip[3] + clip[0];
    m_Frustum[1][1] = clip[7] + clip[4];
    m_Frustum[1][2] = clip[11] + clip[8];
    m_Frustum[1][3] = clip[15] + clip[12];
    normalizePlane(m_Frustum, 1);

    m_Frustum[2][0] = clip[3] + clip[1];
    m_Frustum[2][1] = clip[7] + clip[5];
    m_Frustum[2][2] = clip[11] + clip[9];
    m_Frustum[2][3] = clip[15] + clip[13];
    normalizePlane(m_Frustum, 2);

    m_Frustum[3][0] = clip[3] - clip[1];
    m_Frustum[3][1] = clip[7] - clip[5];
    m_Frustum[3][2] = clip[11] - clip[9];
    m_Frustum[3][3] = clip[15] - clip[13];
    normalizePlane(m_Frustum, 3);

    m_Frustum[4][0] = clip[3] - clip[2];
    m_Frustum[4][1] = clip[7] - clip[6];
    m_Frustum[4][2] = clip[11] - clip[10];
    m_Frustum[4][3] = clip[15] - clip[14];
    normalizePlane(m_Frustum, 4);

    m_Frustum[5][0] = clip[3] + clip[2];
    m_Frustum[5][1] = clip[7] + clip[6];
    m_Frustum[5][2] = clip[11] + clip[10];
    m_Frustum[5][3] = clip[15] + clip[14];
    normalizePlane(m_Frustum, 5);
    return frustum;
}

void Frustum::normalizePlane(array<array<float,4>,6>& frustumArr, int side) {
    float magnitude = sqrt(frustumArr[side][0] * frustumArr[side][0] + frustumArr[side][1] * frustumArr[side][1] + frustumArr[side][2] * frustumArr[side][2]);
    frustumArr[side][0] /= magnitude;
    frustumArr[side][1] /= magnitude;
    frustumArr[side][2] /= magnitude;
    frustumArr[side][3] /= magnitude;
}

bool Frustum::cubeInFrustum(float var1, float var2, float var3, float var4, float var5, float var6) {
    for (int32_t var7 = 0; var7 < 6; ++var7) {
        if (m_Frustum[var7][0] * var1 + m_Frustum[var7][1] * var2 + m_Frustum[var7][2] * var3 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var4 + m_Frustum[var7][1] * var2 + m_Frustum[var7][2] * var3 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var1 + m_Frustum[var7][1] * var5 + m_Frustum[var7][2] * var3 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var4 + m_Frustum[var7][1] * var5 + m_Frustum[var7][2] * var3 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var1 + m_Frustum[var7][1] * var2 + m_Frustum[var7][2] * var6 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var4 + m_Frustum[var7][1] * var2 + m_Frustum[var7][2] * var6 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var1 + m_Frustum[var7][1] * var5 + m_Frustum[var7][2] * var6 + m_Frustum[var7][3] <= 0.0F && m_Frustum[var7][0] * var4 + m_Frustum[var7][1] * var5 + m_Frustum[var7][2] * var6 + m_Frustum[var7][3] <= 0.0F) {
            return false;
        }
    }

    return true;
}

bool Frustum::isVisible(AABB& aabb) {
    return this->cubeInFrustum(aabb.minX, aabb.minY, aabb.minZ, aabb.maxX, aabb.maxY, aabb.maxZ);
}